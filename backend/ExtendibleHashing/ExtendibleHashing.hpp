#ifndef EXTENDIBLEHASHING_HPP
#define EXTENDIBLEHASHING_HPP

#include <vector>
#include <fstream>
#include <optional>
#include <string>
#include <cstring>
#include <cmath>
#include <iostream>
#include <sstream>
#include "Bucket.hpp"

template <class RECORD, class TK>
class ExtendibleHashing {
private:
    int Global_depth;
    int Num_cells;
    int Bucket_size;
    std::vector<int> Bucket_addresses;
    std::fstream File_data;
    std::fstream File_buckets;

    void Save_bucket(Bucket<RECORD, TK>* bucket, int bucket_directory_index);
    Bucket<RECORD, TK>* Load_bucket(int bucket_directory_index);
    void Split_bucket(TK key, int bucket_directory_index);
    void Expand_directory();

public:
    ExtendibleHashing(int global_depth, int bucket_size);
    ~ExtendibleHashing();

    void Close();
    int Hash(TK key);
    void Create(std::string filename_data, std::string filename_buckets);
    bool Open(std::string filedata, std::string filebuckets);
    bool Insert(RECORD record);
    std::optional<RECORD> Search(TK key);
    void Print();
    void Load_csv(std::string filename);
};

// #include "ExtendibleHashing.cpp" // Include the implementation file

#endif // EXTENDIBLEHASHING_HPP
