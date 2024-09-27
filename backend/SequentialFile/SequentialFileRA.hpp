#pragma once

#include <fstream>
#include <string>
#include <vector>

#include "SequentialRA.hpp"
using namespace std;

class SequentialFileA {
    string filename_data;
    string aux_data;
    fstream File_data;
    int Record_size;
    int aux_max_size;

    void merge();
    void insert_aux(SequentialRA record);

public:
    SequentialFileA(string filename_data, string aux_data, int record_size);
    ~SequentialFileA();
    int get_num_records(string name);
    int total_records();    
    void insert(SequentialRA record);
    void remove_record(string key);
    vector<SequentialRA> range_search(string begin_key, string end_key);
    SequentialRA search(string key);
    void show_records(string name, int max_records);
    void destroy();
    void description();
    void create_file(vector<SequentialRA> &records);
};
