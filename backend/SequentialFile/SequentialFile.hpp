#include "Record.hpp"
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

    bool compare(Record a, Record b);

    void merge();
    void insert_sort(Record record);
    void insert_aux(Record record);

public:
    SequentialFile(string filename_data, int record_size);
    ~SequentialFile();
    int get_num_records(string name);
    int total_records();
    void insert(Record record);
    void remove_record(int key);
    optional<Record> search(int key);
};