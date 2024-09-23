#include "SequentialFile.hpp"
#include <fstream>
#include <cmath>
#include <functional>
#include <sstream>

using namespace std;

SequentialFile::SequentialFile(string filename_data, string aux_data, int record_size){
    this->filename_data = filename_data;
    this->aux_data = aux_data;
    this->Record_size = record_size;

    //Create the files if they don't exist
    ofstream file(filename_data, ios::app);
    file.close();

    ofstream file2(aux_data, ios::app);
    file2.close();

    this->aux_max_size = 10;
};

SequentialFile::~SequentialFile(){
    File_data.close();
};

int SequentialFile::get_num_records(string name){
    // open file for save data
    if (name == "data") {
        File_data.open(filename_data, ios::in | ios::out | ios::binary);
    }
    if (name == "aux") {
        File_data.open(aux_data, ios::in | ios::out | ios::binary);
    }
    
    // check if file is open
    if (!File_data.is_open()) {
        cout << "[!] Error opening file " << filename_data << endl;
        return -1;
    }

    // get the number of records in the file
    File_data.seekg(0, ios::end);
    int num_records = File_data.tellg() / Record_size;

    // close file
    File_data.close();

    return num_records;
};

int SequentialFile::total_records(){
    return get_num_records("data") + get_num_records("aux");
};

void SequentialFile::insert_aux(Record record){
    File_data.open(aux_data, ios::in | ios::out | ios::binary);
    File_data.seekp(0, ios::end);
    File_data.write((char*) &record, Record_size);
    File_data.flush();
    File_data.close();
};

void SequentialFile::insert(Record record){
    if (get_num_records("aux") == this->aux_max_size)
        merge();
    
    insert_aux(record);

    return;
};

void SequentialFile::remove_record(int key) {
    Record record;
    vector<Record> aux_records;
    bool flag = false;
    //Search linearly in the auxiliary file
    File_data.open(aux_data, ios::in | ios::out | ios::binary);
    File_data.seekg(0, ios::beg);
    while (File_data.read((char*) &record, Record_size)) {
        if (record.key == key){
            cout<<"Record found in aux data"<<endl;
            flag = true;
            continue;
        }
        aux_records.push_back(record);
    }
    File_data.close();

    if (flag) {
        remove(aux_data.c_str());

        ofstream file2(aux_data, ios::app);
        file2.close();

        File_data.open(aux_data, ios::in | ios::out | ios::binary);
        for (Record record : aux_records) {
            File_data.write((char*) &record, Record_size);
        }
        File_data.close();
        return;
    }

    //Search linearly in the data file
    vector<Record> data_records;
    File_data.open(filename_data, ios::in | ios::out | ios::binary);
    File_data.seekg(0, ios::beg);
    while (File_data.read((char*) &record, Record_size)) {
        if (record.key == key){
            cout<<"Record found in main data"<<endl;
            flag = true;
            continue;
        }
        data_records.push_back(record);
    }

    if (!flag) {
        cout << "Record not found" << endl;
        return;
    }

    File_data.close();

    remove(filename_data.c_str());

    ofstream file(filename_data, ios::app);
    file.close();

    File_data.open(filename_data, ios::in | ios::out | ios::binary);
    for (Record record : data_records) {
        File_data.write((char*) &record, Record_size);
    }
    File_data.close();

}

void SequentialFile::show_records(string name) {
    Record record;

    if (name == "data") {
        cout<< "Opening data file" << endl;
        File_data.open(filename_data, ios::in | ios::out | ios::binary);
    }
    else if (name == "aux") {
        cout<< "Opening aux file" << endl;
        File_data.open(aux_data, ios::in | ios::out | ios::binary);
    };

    if (!File_data.is_open()) {
        cout << "[!] Error opening file " << filename_data << endl;
        return;
    };

    File_data.seekg(0, ios::beg);
    while (File_data.read((char*) &record, Record_size)) {
        cout << record.key << " " << record.name << " " << record.age << " " << record.term << endl;
    }

    File_data.close();
}
void SequentialFile::destroy() {
    remove(filename_data.c_str());
    remove(aux_data.c_str());
}
void SequentialFile::description() {
    cout<<"Number of records in data file: "<<get_num_records("data")<<endl;
    cout<<"Number of records in aux file: "<<get_num_records("aux")<<endl;
    cout<<"Total number of records: "<<total_records()<<endl;
    
    cout<<"Record in data file: "<<endl;
    show_records("data");

    cout<<"Record in aux file: "<<endl;
    show_records("aux");

};

auto compare = [](Record a, Record b) -> bool {
    return a.key < b.key;
};

void SequentialFile::merge()
{
    vector<Record> aux_records;
    
    //Open the auxiliary file
    File_data.open(aux_data, ios::in | ios::out | ios::binary);
    File_data.seekg(0, ios::beg);
    
    //Read every record in the auxiliary file and save it in a vector
    Record aux;
    while (File_data.read((char*) &aux, Record_size)) {
        aux_records.push_back(aux);
    }
    aux_records.push_back(aux);

    //Read every record in the data file and save it in the same vector
    File_data.close();
    File_data.open(filename_data, ios::in | ios::out | ios::binary);
    File_data.seekg(0, ios::beg);
    while (File_data.read((char*) &aux, Record_size)) {
        aux_records.push_back(aux);
    }

    //Sort the vector
    sort(aux_records.begin(), aux_records.end(), compare);

    //Write the sorted vector in the data file
    File_data.close();
    File_data.open(filename_data, ios::in | ios::out | ios::binary);
    File_data.seekp(0, ios::beg);
    for (Record record : aux_records) {
        File_data.write((char*) &record, Record_size);
    }

    //Delete the auxiliary file
    File_data.close();
    remove(aux_data.c_str());

    ofstream file2(aux_data, ios::app);
    file2.close();

    return;
};

vector<Record> SequentialFile::range_search(int begin_key, int end_key) {
    vector<Record> records;
    Record record;
    
    File_data.open(filename_data, ios::in | ios::out | ios::binary);
    
    if (!File_data) {
        cerr << "Error opening file!" << endl;
        return records;
    }
    
    File_data.seekg(0, ios::beg);
    
    // first record >= begin_key using binary search
    int left = 0, right, middle;
    File_data.seekg(0, ios::end);
    right = File_data.tellg() / sizeof(Record) - 1;
    
    int first_pos = -1;
    
    while (left <= right) {
        middle = (left + right) / 2;
        File_data.seekg(middle * sizeof(Record), ios::beg);
        File_data.read(reinterpret_cast<char*>(&record), sizeof(Record));
        
        if (record.key >= begin_key) {
            first_pos = middle; 
            right = middle - 1;
        } else {
            left = middle + 1;
        }
    }
    
    if (first_pos == -1) first_pos = File_data.tellg() / sizeof(Record) - 1;


    //last record <= end_key using binary search
    left = first_pos;

    File_data.seekg(0, ios::end);
    right = File_data.tellg() / sizeof(Record) - 1;
    
    int last_pos = -1;
    
    while (left <= right) {
        middle = left + (right - left) / 2;
        File_data.seekg(middle * sizeof(Record), ios::beg);
        File_data.read(reinterpret_cast<char*>(&record), sizeof(Record));
        
        if (record.key <= end_key) {
            last_pos = middle;
            left = middle + 1;
        } else {
            right = middle - 1;
        }
    }
    
    //all records between first_pos and last_pos inclusive
    if (last_pos != -1) {
        for (int i = first_pos; i <= last_pos; ++i) {
            File_data.seekg(i * sizeof(Record), ios::beg);
            File_data.read(reinterpret_cast<char*>(&record), sizeof(Record));
            records.push_back(record);
        }
    }
    
    File_data.close();
    
    File_data.open(aux_data, ios::in | ios::out | ios::binary);

    File_data.seekg(0, ios::beg);

    while (File_data.read((char*) &record, Record_size)) {
        if (record.key >= begin_key && record.key <= end_key) 
            records.push_back(record);
        
    }

    File_data.close();

    sort(records.begin(), records.end(), compare);

    return records;
}
Record SequentialFile::search(int key){

    Record record;
    
    File_data.open(filename_data, ios::in | ios::out | ios::binary);
    
    if (!File_data) {
        cerr << "Error opening file!" << endl;
        return record;
    }
    
    File_data.seekg(0, ios::beg);
    
    int left = 0, right, middle;
    File_data.seekg(0, ios::end);
    right = File_data.tellg() / sizeof(Record) - 1;
    
    int first_pos = -1;
    
    while (left <= right) {
        middle = (left + right) / 2;
        File_data.seekg(middle * sizeof(Record), ios::beg);
        File_data.read(reinterpret_cast<char*>(&record), sizeof(Record));
        
        if (record.key == key) {
            File_data.close();
            return record;
        }
        else if (record.key > key) 
            right = middle - 1;
        else
            left = middle + 1;
    }
    File_data.close(); 

    File_data.open(aux_data, ios::in | ios::out | ios::binary);
    File_data.seekg(0, ios::beg);
    while (File_data.read((char*) &record, Record_size)) {
        if (record.key == key) {
            File_data.close();
            return record;
        }
    }
    File_data.close();

    cout << "Record not found" << endl;
    return record;

    
};

vector<Record> read_from_csv(string filename){
    vector<Record> records;
    ifstream file(filename);
    string line;
    bool header = true;
    while (getline(file, line)) {
        if (header) {
            header = false;
            continue;
        }
        auto line_stream = stringstream(line);
        string key, name, age, term;
        getline(line_stream, key, ',');
        getline(line_stream, name, ',');
        getline(line_stream, age, ',');
        getline(line_stream, term, ',');

        int key_int = stoi(key);
        Record record(key_int, name.c_str(), stoi(age), stoi(term));

        records.push_back(record);
    
    }

    return records;
};