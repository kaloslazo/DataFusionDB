#ifndef BUCKET_HPP
#define BUCKET_HPP

#include "Record.hpp"

class Bucket {
public:
  // initialize with max keys (records in bucket)
  Bucket(int max_keys);
  ~Bucket();

  // insert record into bucket
  int Insert(Record& record);

  // remove record from bucket
  void Remove(int& key);

  // search for record in bucket
  Record* Search(int& key);

  // redistribute records between two buckets
  void Redistribute(Bucket& bucket);
  
  // print all records in bucket
  void Print();

protected:
  // local depth of bucket
  int Local_depth;

  // number of records in bucket (keys)
  int Max_bucket_size;

  // current number of records in bucket
  int Current_size;

  // array of records of type Record
  Record* Records;

  // allow class ExtendibleHashing to access protected members
  friend class ExtendibleHashing;
};

#endif // BUCKET_HPP
