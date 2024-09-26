#ifndef BUCKET_HPP
#define BUCKET_HPP

#include <optional>
#include <iostream>
#include "EHRecordA.hpp"
#include "EHRecordB.hpp"

template <class RECORD, class TK>
class Bucket {
public:
    int Local_depth;
    int Max_bucket_size;
    int Current_size;
    RECORD* Records;

    Bucket(int max_keys);
    ~Bucket();

    int Insert(RECORD& record);
    std::optional<RECORD> Search(TK key);
    void Remove(TK key);
    void Print();
};

#endif // BUCKET_HPP 
