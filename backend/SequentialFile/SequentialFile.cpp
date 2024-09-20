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
    // Read the records in aux file
    File_data.open(aux_data, ios::in | ios::out | ios::binary);
    File_data.seekg(0, ios::end);

    Record aux;
    while(File_data.read((char*) &aux, Record_size)) {
        if (aux.key == key) {
            // Erase the record
            File_data.seekg(-Record_size, ios::cur);
            Record empty;
            File_data.write((char*) &empty, Record_size);
            break;
        }
    }

    //Compact the file
    File_data.seekg(0, ios::beg);
    vector<Record> aux_records;
    while(File_data.read((char*) &aux, Record_size)) 
        if (aux.key != -1) aux_records.push_back(aux);
    
    File_data.close();

    File_data.open(aux_data, ios::in | ios::out | ios::binary);
    File_data.seekp(0, ios::beg);
    for (Record record : aux_records) {
        File_data.write((char*) &record, Record_size);
    }

    File_data.close();
    return;
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