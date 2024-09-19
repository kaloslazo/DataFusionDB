#include "SequentialFile.hpp"
#include <fstream>
#include <cmath>
#include <functional>

using namespace std;

SequentialFile::SequentialFile(string filename_data, int record_size){
    this->filename_data = filename_data;
    this->Record_size = record_size;
};

SequentialFile::~SequentialFile(){
    File_data.close();
};

bool compare(Record a, Record b) {
    return a.key < b.key;
};

int SequentialFile::get_num_records(string name){
    // open file for save data
    if (name == "data") {
        File_data.open(filename_data, ios::in | ios::out | ios::binary);
    };
    if (name == "aux") {
        File_data.open(aux_data, ios::in | ios::out | ios::binary);
    };
    
    // check if file is open
    if (!File_data.is_open()) {
        cout << "[!] Error opening file " << filename_data << endl;
        return -1;
    };

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
optional<Record> SequentialFile::search(int key){

    // Search in the data file
    File_data.open(filename_data, ios::in | ios::out | ios::binary);
    File_data.seekg(0, ios::beg);

    Record aux;
    while (File_data.read((char*) &aux, Record_size)) {
        if (aux.key == key) {
            File_data.close();
            return aux;
        }
    }

    // Search in the aux file
    File_data.open(aux_data, ios::in | ios::out | ios::binary);
    File_data.seekg(0, ios::beg);

    while (File_data.read((char*) &aux, Record_size)) {
        if (aux.key == key) {
            File_data.close();
            return aux;
        }
    }

    File_data.close();
    return {};
};

void SequentialFile::insert_sort(Record record){

    File_data.open(filename_data, ios::in | ios::out | ios::binary);
    File_data.seekg(0, ios::end);

    //Read until you find a record with a key greater than the key of the record to be inserted
    Record aux;
    bool flag = false;
    while (File_data.read((char*) &aux, Record_size)) {
        if (aux.key > record.key) {
            //Go back one record
            File_data.seekg(-Record_size, ios::cur);

            //Insert the record in the correct position
            File_data.write((char*) &record, Record_size);
            flag = true;
        }
    }

    //If flag = false, insert the record at the end of the file
    if (!flag) 
        File_data.write((char*) &record, Record_size);
    

    //If flag = true, in Record aux is the record that was overwritten, so we are going to move every record one position to the right
    if (flag) {
        File_data.write((char*) &aux, Record_size);
        while (File_data.read((char*) &aux, Record_size)) 
            File_data.write((char*) &aux, Record_size);
    }

    File_data.flush();
    File_data.close();
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

    //Sort the vector
    sort(aux_records.begin(), aux_records.end(), compare);

    //Insert every record in the data file sorted
    for (Record record : aux_records) {
        insert_sort(record);
    }

    File_data.close();

    //Delete the auxiliary file
    remove(aux_data.c_str());


    //Create a new auxiliary file
    File_data.open(aux_data, ios::in | ios::out | ios::binary);
    File_data.close();
};

