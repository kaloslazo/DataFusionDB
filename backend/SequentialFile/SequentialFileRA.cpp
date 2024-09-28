#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <sstream>
#include <stdio.h>
#include <string.h>

#include "SequentialRA.hpp"
#include "SequentialFileRA.hpp"
using namespace std;

vector<SequentialRA> SequentialFileA::read_csv(string filename) {
  vector<SequentialRA> records;
  ifstream file(filename);
  string line;
  bool header = true;
  while (getline(file, line)) {
    if (header) {
      header = false;
      continue;
    }
    stringstream line_stream(line);
    string id, name, album, album_id, artists;

    getline(line_stream, id, ',');
    getline(line_stream, name, ',');
    getline(line_stream, album, ',');
    getline(line_stream, album_id, ',');
    getline(line_stream, artists);

    SequentialRA record;
    record.setId(id);
    record.setName(name);
    record.setAlbum(album);
    record.setAlbumId(album_id);
    record.setArtists(artists);

    record.Print();
    records.push_back(record);
  }
  return records;
}

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

void SequentialFileA::insert_aux(SequentialRA record){
  File_data.open(aux_data, ios::in | ios::out | ios::binary);
  File_data.seekp(0, ios::end);
  File_data.write((char*) &record, Record_size);
  File_data.flush();
  File_data.close();
};

void SequentialFileA::insert(SequentialRA record) {
  if (get_num_records("aux") >= this->aux_max_size)
    merge();

  File_data.open(aux_data, ios::app | ios::binary);
  File_data.write(reinterpret_cast<const char*>(&record), sizeof(SequentialRA));
  File_data.close();
}

void SequentialFileA::remove_record(string key) {
  SequentialRA record;
  vector<SequentialRA> aux_records;
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
    for (SequentialRA record : aux_records) {
      File_data.write((char*) &record, Record_size);
    }
    File_data.close();
    return;
  }

  //Search linearly in the data file
  vector<SequentialRA> data_records;
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
  for (SequentialRA record : data_records) {
    File_data.write((char*) &record, Record_size);
  }
  File_data.close();

}

void SequentialFileA::show_records(string name, int max_records=3){
  SequentialRA record;

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

auto compareA = [](SequentialRA a, SequentialRA b) -> bool {
  return a.key() < b.key();
};

void SequentialFileA::merge()
{
  vector<SequentialRA> aux_records;

  //Open the auxiliary file
  File_data.open(aux_data, ios::in | ios::out | ios::binary);
  File_data.seekg(0, ios::beg);

  //Read every record in the auxiliary file and save it in a vector
  SequentialRA aux;
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
  for (SequentialRA record : aux_records) {
    File_data.write((char*) &record, Record_size);
  }

  //Delete the auxiliary file
  File_data.close();
  remove(aux_data.c_str());

  ofstream file2(aux_data, ios::app);
  file2.close();

  return;
};

vector<SequentialRA> SequentialFileA::range_search(string begin_key, string end_key) {
  vector<SequentialRA> records;
  SequentialRA record;

  File_data.open(filename_data, ios::in | ios::out | ios::binary);
  if (!File_data) {
    cerr << "Error opening file!" << endl;
    return records;
  }

  File_data.seekg(0, ios::end);
  int total_records = File_data.tellg() / Record_size;
  if (total_records == 0) {
    File_data.close();
    return records;
  }

  // Si begin_key está vacío, empezamos desde el principio
  int first_pos = 0;
  if (!begin_key.empty()) {
    // Búsqueda binaria para encontrar la primera posición
    int left = 0, right = total_records - 1;
    while (left <= right) {
      int middle = left + (right - left) / 2;
      File_data.seekg(middle * Record_size, ios::beg);
      File_data.read(reinterpret_cast<char*>(&record), Record_size);
      if (record.key() >= begin_key) {
        first_pos = middle;
        right = middle - 1;
      } else {
        left = middle + 1;
      }
    }
  }

  // Si end_key es "~" o está vacío, leemos hasta el final
  int last_pos = total_records - 1;
  if (!end_key.empty() && end_key != "~") {
    // Búsqueda binaria para encontrar la última posición
    int left = first_pos, right = total_records - 1;
    while (left <= right) {
      int middle = left + (right - left) / 2;
      File_data.seekg(middle * Record_size, ios::beg);
      File_data.read(reinterpret_cast<char*>(&record), Record_size);
      if (record.key() <= end_key) {
        last_pos = middle;
        left = middle + 1;
      } else {
        right = middle - 1;
      }
    }
  }

  // Leer todos los registros entre first_pos y last_pos
  for (int i = first_pos; i <= last_pos; ++i) {
    File_data.seekg(i * Record_size, ios::beg);
    if (File_data.read(reinterpret_cast<char*>(&record), Record_size)) {
      records.push_back(record);
    }
  }

  File_data.close();

  // Leer registros del archivo auxiliar
  File_data.open(aux_data, ios::in | ios::out | ios::binary);
  if (File_data) {
    File_data.seekg(0, ios::beg);
    while (File_data.read(reinterpret_cast<char*>(&record), Record_size)) {
      if ((begin_key.empty() || record.key() >= begin_key) && 
        (end_key.empty() || end_key == "~" || record.key() <= end_key)) {
        records.push_back(record);
      }
    }
    File_data.close();
  }
  
  cout << "Records found: " << records.size() << endl;
  sort(records.begin(), records.end(), compareA);
  return records;
}

SequentialRA SequentialFileA::search(string key) {
  SequentialRA record;

  // Search in main data file
  File_data.open(filename_data, ios::in | ios::binary);
  while (File_data.read(reinterpret_cast<char*>(&record), sizeof(SequentialRA))) {
    if (record.key() == key) {
      File_data.close();
      return record;
    }
  }
  File_data.close();

  // Search in auxiliary file
  File_data.open(aux_data, ios::in | ios::binary);
  while (File_data.read(reinterpret_cast<char*>(&record), sizeof(SequentialRA))) {
    if (record.key() == key) {
      File_data.close();
      return record;
    }
  }
  File_data.close();

  cout << "Record not found" << endl;
  return SequentialRA();
}

void SequentialFileA::create_file(vector<SequentialRA>& records){
  sort(records.begin(), records.end(), compareA);

  File_data.open(filename_data, ios::in | ios::out | ios::binary);
  File_data.seekp(0, ios::beg);

  for (SequentialRA record : records) {
    File_data.write((char*) &record, Record_size);
  }

  File_data.close();
  return;
};
