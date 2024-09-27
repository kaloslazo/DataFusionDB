#include <fstream>
#include <string>
#include <vector>
#include <math.h>

#include "SequentialRB.hpp"
using namespace std;

class SequentialFileB {
    string filename_data;
    string aux_data;
    fstream File_data;
    int Record_size;
    int aux_max_size;

    void merge();
    void insert_aux(SequentialRB record);

public:
    SequentialFileB(string filename_data, string aux_data, int record_size);
    ~SequentialFileB();

    int get_num_records(string name);
    int total_records();    
    void insert(SequentialRB record);
    void remove_record(string key);
    vector<SequentialRB> range_search(string begin_key, string end_key);
    SequentialRB search(string key);
    void show_records(string name, int max_records);
    void destroy();
    void description();
    void create_file(vector<SequentialRB>&records);
};
