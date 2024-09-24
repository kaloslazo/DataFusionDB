#ifndef AVLRECORDA_HPP
#define AVLRECORDA_HPP

#include <cstring>
#include "Record.hpp"

struct AvlRecordA {
    char id[23];
    char name[521];
    char album[244];
    char album_id[23];
    char artists[1124];
    int left = -1;
    int right = -1;
    int nextdel = -1;

    RecordA to_record();
    void Print();
    std::string key();
    
    AvlRecordA();
    AvlRecordA(const RecordA& record);
};

#endif // AVLRECORDA_HPP
