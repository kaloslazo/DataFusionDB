#ifndef BUCKET_HPP
#define BUCKET_HPP

#include "ExtendibleHRecordA.hpp"
#include "ExtendibleHRecordB.hpp"

template <class RECORD, class TK> class ExtendibleHashing;
template <class RECORD, class TK>
class Bucket {
public:
  // initialize with max keys (records in bucket)
  Bucket(int max_keys);
  ~Bucket();

  // insert record into bucket
  int Insert(RECORD& record);

  // remove record from bucket
  void Remove(TK key);

  // search for record in bucket
  RECORD* Search(TK key);

  // redistribute records between two buckets
  void Redistribute(Bucket<RECORD, TK>& bucket);
  
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
  RECORD* Records;

  // allow class ExtendibleHashing to access protected members
  friend class ExtendibleHashing<RECORD, TK>;
};

#endif // BUCKET_HPP
