#include "Bucket.hpp"
#include "ExtendibleHRecordA.hpp"
#include "ExtendibleHRecordB.hpp"

template <class RECORD, class TK>
Bucket<RECORD, TK>::Bucket(int max_keys) {
  // initialize local depth with 1 and max bucket size (records in bucket)
  Local_depth = 1;
  Max_bucket_size = max_keys;
  
  // create empty array of max_keys records
  Current_size = 0;
  Records = new RECORD[Max_bucket_size];
};

template <class RECORD, class TK>
Bucket<RECORD, TK>::~Bucket() {
  delete[] Records;
};


template <class RECORD, class TK>
int Bucket<RECORD, TK>::Insert(RECORD& record) {
  // check if bucket is full
  if (Current_size == Max_bucket_size) {
    cout << "[!] Bucket is full" << endl;
    return -1;
  }
  
  // insert record into bucket
  Records[Current_size] = record;
  Current_size++;
  
  return 0;
};

template <class RECORD, class TK>
void Bucket<RECORD, TK>::Remove(TK key) {
  // search for record in bucket (o(k^2))
  for (int i = 0; i < Current_size; i++) {
    if (Records[i].key() == key) {
      // remove record from bucket shifting all records to the left (new space complexity O(k))
      for (int j = i; j < Current_size - 1; j++) {
        Records[j] = Records[j + 1];
      };
      Current_size--;
      break;
    };
  };
};

template <class RECORD, class TK>
void Bucket<RECORD, TK>::Print() {
  cout << "Local depth: " << Local_depth << endl;
  cout << "Number of records: " << Current_size << endl;

  for (int i = 0; i < Current_size; i++) {
    Records[i].Print();
  };
};
