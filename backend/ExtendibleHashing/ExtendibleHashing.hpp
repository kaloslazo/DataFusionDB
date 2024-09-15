#ifndef EXTENDIBLE_HASHING_HPP
#define EXTENDIBLE_HASHING_HPP

#include "Record.hpp"
#include "Bucket.hpp"
#include <fstream>
#include <optional>
#include <string>
#include <vector>

using namespace std;

class ExtendibleHashing {
public:
  // we need the global depth and the bucket size
  ExtendibleHashing(int global_depth, int bucket_size);
  ~ExtendibleHashing();

  // create empty hashing index with filename for save data and save buckets
  void Create(string filename_data, string filename_buckets);

  // open existing hashing index with filename
  bool Open(string filedata, string filebuckets);

  // handle the close of the file
  void Close();

  // insert key into hashing index, receive key and address of record
  bool Insert(int& key, int rec_address);
  
  // remove record using key from hashing index
  bool Remove(int& key);

  // search for record, returns the the record
  optional<Record> Search(int& key);

  // print the directory and buckets
  void Print(ofstream& out); 

protected:
  // files for save data and save buckets
  fstream File_data;
  fstream File_buckets;

  // number of bits to consider in hash function
  int Global_depth;

  // array of pointers to buckets (vector int cuz we need to save the address of the bucket)
  vector<int> Bucket_addresses;

  // size of the bucket (max records in bucket)
  int Bucket_size;
  
  // pointer to current bucket
  Bucket* Current_bucket;

  // the number of cells (rows) in the directory equals 2^global_depth
  int Num_cells;

  // hash function converts key to hash value using binary approach
  int Hash(int& key);
  
  // directory expansion for double the current size
  void Expand_directory();
  
  // split the bucket, handle the insertion of the overflow record
  void Split_bucket(int& key, int first, int last);

  // takes bucket and save it in secondary memory
  void Save_bucket(Bucket* bucket, int bucket_directory_address);

  // load bucket from secondary memory
  Bucket* Load_bucket(int bucket_directory_address);
};

#endif // EXTENDIBLE_HASHING_HPP
