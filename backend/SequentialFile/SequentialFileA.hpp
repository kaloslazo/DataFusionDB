#pragma once

#include "RecordA.hpp"
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <sstream>
#include <stdio.h>
#include <string.h>


using namespace std;

class SequentialFileA {
    string filename_data;
    string aux_data;
    fstream File_data;
    int Record_size;
    int aux_max_size;

    void merge();
    void insert_aux(SeqRecordA record);

public:
    SequentialFileA(string filename_data, string aux_data, int record_size);
    ~SequentialFileA();
    int get_num_records(string name);
    int total_records();    
    void insert(SeqRecordA record);
    void remove_record(string key);
    vector<SeqRecordA> range_search(string begin_key, string end_key);
    SeqRecordA search(string key);
    void show_records(string name, int max_records);
    void destroy();
    void description();
    void create_file(vector<SeqRecordA>&records);
};

SequentialFileA::SequentialFileA(string filename_data, string aux_data, int record_size){
    this->filename_data = filename_data;
    this->aux_data = aux_data;
    this->Record_size = record_size;

    //Create the files if they don't exist
    ofstream file(filename_data, ios::app);
    file.close();

    ofstream file2(aux_data, ios::app);
    file2.close();

    this->aux_max_size = 100;
};

SequentialFileA::~SequentialFileA(){
    File_data.close();
};

int SequentialFileA::get_num_records(string name){
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

int SequentialFileA::total_records(){
    return get_num_records("data") + get_num_records("aux");
};

void SequentialFileA::insert_aux(SeqRecordA record){
    File_data.open(aux_data, ios::in | ios::out | ios::binary);
    File_data.seekp(0, ios::end);
    File_data.write((char*) &record, Record_size);
    File_data.flush();
    File_data.close();
};

void SequentialFileA::insert(SeqRecordA record){
    cout<<"Inserting record"<<endl;
    if (get_num_records("aux") == this->aux_max_size)
        merge();
    
    insert_aux(record);

    return;
};

void SequentialFileA::remove_record(string key) {
    SeqRecordA record;
    vector<SeqRecordA> aux_records;
    bool flag = false;
    //Search linearly in the auxiliary file
    File_data.open(aux_data, ios::in | ios::out | ios::binary);
    File_data.seekg(0, ios::beg);
    while (File_data.read((char*) &record, Record_size)) {
        if (record.key() == key){
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
        for (SeqRecordA record : aux_records) {
            File_data.write((char*) &record, Record_size);
        }
        File_data.close();
        return;
    }

    //Search linearly in the data file
    vector<SeqRecordA> data_records;
    File_data.open(filename_data, ios::in | ios::out | ios::binary);
    File_data.seekg(0, ios::beg);
    while (File_data.read((char*) &record, Record_size)) {
        if (record.key() == key){
            cout<<"Record found in main data"<<endl;
            flag = true;
            continue;
        }
        data_records.push_back(record);
    }

    // if (!flag) {
    //     cout << "Record not found" << endl;
    //     return;
    // }

    File_data.close();

    remove(filename_data.c_str());

    ofstream file(filename_data, ios::app);
    file.close();

    File_data.open(filename_data, ios::in | ios::out | ios::binary);
    for (SeqRecordA record : data_records) {
        File_data.write((char*) &record, Record_size);
    }
    File_data.close();

}

void SequentialFileA::show_records(string name, int max_records=3){
    SeqRecordA record;

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
    int i=0;
    while (File_data.read((char*) &record, Record_size) && i<max_records) {
        record.Print();    
        i++;
    }
    
    File_data.close();

    return;
}
void SequentialFileA::destroy() {
    remove(filename_data.c_str());
    remove(aux_data.c_str());
}
void SequentialFileA::description() {
    cout<<"Number of records in data file: "<<get_num_records("data")<<endl;
    cout<<"Number of records in aux file: "<<get_num_records("aux")<<endl;
    cout<<"Total number of records: "<<total_records()<<endl;
    
    cout<<"Record in data file: "<<endl;
    show_records("data");

    cout<<"Record in aux file: "<<endl;
    show_records("aux");

};

auto compareA = [](SeqRecordA a, SeqRecordA b) -> bool {
    return a.key() < b.key();
};

void SequentialFileA::merge()
{
    vector<SeqRecordA> aux_records;
    
    //Open the auxiliary file
    File_data.open(aux_data, ios::in | ios::out | ios::binary);
    File_data.seekg(0, ios::beg);
    
    //Read every record in the auxiliary file and save it in a vector
    SeqRecordA aux;
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
    sort(aux_records.begin(), aux_records.end(), compareA);

    //Write the sorted vector in the data file
    File_data.close();
    File_data.open(filename_data, ios::in | ios::out | ios::binary);
    File_data.seekp(0, ios::beg);
    for (SeqRecordA record : aux_records) {
        File_data.write((char*) &record, Record_size);
    }

    //Delete the auxiliary file
    File_data.close();
    remove(aux_data.c_str());

    ofstream file2(aux_data, ios::app);
    file2.close();

    return;
};

vector<SeqRecordA> SequentialFileA::range_search(string begin_key, string end_key) {
    vector<SeqRecordA> records;
    SeqRecordA record;
    
    File_data.open(filename_data, ios::in | ios::out | ios::binary);
    
    if (!File_data) {
        cerr << "Error opening file!" << endl;
        return records;
    }
    
    File_data.seekg(0, ios::beg);
    
    // first record >= begin_key using binary search
    int left = 0, right, middle;
    File_data.seekg(0, ios::end);
    right = File_data.tellg() / Record_size - 1;
    
    int first_pos = -1;
    
    while (left <= right) {
        middle = left + (right - left) / 2;
        File_data.seekg(middle * Record_size, ios::beg);
        File_data.read(reinterpret_cast<char*>(&record), Record_size);
        
        if (record.key() >= begin_key) {
            first_pos = middle; 
            right = middle - 1;
        } else {
            left = middle + 1;
        }
    }
    
    if (first_pos == -1) first_pos = File_data.tellg() / Record_size - 1;


    //last record <= end_key using binary search
    left = first_pos;

    File_data.seekg(0, ios::end);
    right = File_data.tellg() / Record_size - 1;
    
    int last_pos = -1;
    
    while (left <= right) {
        middle = left + (right - left) / 2;
        File_data.seekg(middle * Record_size, ios::beg);
        File_data.read(reinterpret_cast<char*>(&record), Record_size);
        
        if (record.key() <= end_key) {
            last_pos = middle;
            left = middle + 1;
        } else {
            right = middle - 1;
        }
    }
    
    //all records between first_pos and last_pos inclusive
    if (last_pos != -1) {
        for (int i = first_pos; i <= last_pos; ++i) {
            File_data.seekg(i * Record_size, ios::beg);
            File_data.read(reinterpret_cast<char*>(&record), Record_size);
            records.push_back(record);
        }
    }
    
    File_data.close();
    
    File_data.open(aux_data, ios::in | ios::out | ios::binary);

    File_data.seekg(0, ios::beg);

    while (File_data.read((char*) &record, Record_size)) {
        if (record.key() >= begin_key && record.key() <= end_key) 
            records.push_back(record);
        
    }

    File_data.close();

    sort(records.begin(), records.end(), compareA);

    return records;
}
SeqRecordA SequentialFileA::search(string key){

    SeqRecordA record;
    
    File_data.open(filename_data, ios::in | ios::out | ios::binary);
    
    if (!File_data) {
        cerr << "Error opening file!" << endl;
        return record;
    }
    
    File_data.seekg(0, ios::beg);
    
    int left = 0, right, middle;
    File_data.seekg(0, ios::end);
    right = File_data.tellg() / Record_size - 1;
    
    int first_pos = -1;
    
    while (left <= right) {
        middle = (left + right) / 2;
        File_data.seekg(middle * Record_size, ios::beg);
        File_data.read(reinterpret_cast<char*>(&record), Record_size);
        
        if (record.key() == key) {
            File_data.close();
            return record;
        }
        else if (record.key() > key) 
            right = middle - 1;
        else
            left = middle + 1;
    }
    File_data.close(); 

    File_data.open(aux_data, ios::in | ios::out | ios::binary);
    File_data.seekg(0, ios::beg);
    while (File_data.read((char*) &record, Record_size)) {
        if (record.key() == key) {
            File_data.close();
            return record;
        }
    }
    File_data.close();

    cout << "Record not found" << endl;
    return record;

    
};

vector<SeqRecordA> read_from_csvA(string filename){
    vector<SeqRecordA> records;
    ifstream file(filename);
    string line;
    bool header = true;
    while (getline(file, line)) {
        if (header) {
            header = false;
            continue;
        }
        auto line_stream = stringstream(line);
        //id,name,album,album_id,artists
        SeqRecordA record;
        string id, name, album, album_id, artists;
        getline(line_stream, id, ',');
        getline(line_stream, name, ',');
        getline(line_stream, album, ',');
        getline(line_stream, album_id, ',');
        getline(line_stream, artists, ',');
        strncpy(record.id, id.c_str(), sizeof(record.id));
        record.id[sizeof(record.id) - 1] = '\0';
        strncpy(record.name, name.c_str(), sizeof(record.name));
        record.name[sizeof(record.name) - 1] = '\0';
        strncpy(record.album, album.c_str(), sizeof(record.album));
        record.album[sizeof(record.album) - 1] = '\0';
        strncpy(record.album_id, album_id.c_str(), sizeof(record.album_id));
        record.album_id[sizeof(record.album_id) - 1] = '\0';
        strncpy(record.artists, artists.c_str(), sizeof(record.artists));
        record.artists[sizeof(record.artists) - 1] = '\0';
        records.push_back(record);
    
    }

    return records;
};

void SequentialFileA::create_file(vector<SeqRecordA>& records){
    sort(records.begin(), records.end(), compareA);

    File_data.open(filename_data, ios::in | ios::out | ios::binary);
    File_data.seekp(0, ios::beg);

    for (SeqRecordA record : records) {
        File_data.write((char*) &record, Record_size);
    }

    File_data.close();

    return;

};