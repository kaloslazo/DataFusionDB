#ifndef EXTENDIBLE_HASHING_HPP
#define EXTENDIBLE_HASHING_HPP

#include "Bucket.hpp"
#include <fstream>
#include <optional>
#include <string>
#include <vector>

using namespace std;


template <class RECORD, class TK>
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

  // insert receives object record, we determine the key and adress inside
  bool Insert(RECORD record);
  
  // remove record using key from hashing index
  bool Remove(TK key);

  // search for record, returns the the record
  optional<RECORD> Search(TK key);

  // print the directory and buckets
  void Print(); 

  // load from csv and insert into extendible hashing
  void Load_csv(string filename);

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
  Bucket<RECORD, TK>* Current_bucket;

  // the number of cells (rows) in the directory equals 2^global_depth
  int Num_cells;

  // hash function converts key to hash value using binary approach
  int Hash(TK key);
  
  // directory expansion for double the current size
  void Expand_directory();
  
  // split the bucket, handle the insertion of the overflow record
  void Split_bucket(TK key, int bucket_directory_address);

  // takes bucket and save it in secondary memory
  void Save_bucket(Bucket<RECORD, TK>* bucket, int bucket_directory_address);

  // load bucket from secondary memory
  Bucket<RECORD, TK>* Load_bucket(int bucket_directory_address);
};

#endif // EXTENDIBLE_HASHING_HPP
