#pragma once

#include "Record.cpp"
#include <fstream>
#include <optional>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <stdio.h>
#include <string.h>

using namespace std;

class SequentialFile {
    string filename_data;
    string aux_data;
    fstream File_data;
    int Record_size;
    int aux_max_size;

    void merge();
    void insert_aux(Record record);

public:
    SequentialFile(string filename_data, string aux_data, int record_size);
    ~SequentialFile();
    int get_num_records(string name);
    int total_records();    
    void insert(Record record);
    void remove_record(int key);
    vector<Record> range_search(int begin_key, int end_key);
    Record search(int key);
    void show_records(string name);
    void destroy();
    void description();
};