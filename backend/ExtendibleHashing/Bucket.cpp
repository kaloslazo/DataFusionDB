#include "Bucket.hpp"
#include "Record.cpp"

Bucket::Bucket(int max_keys) {
  // initialize local depth with 1 and max bucket size (records in bucket)
  Local_depth = 1;
  Max_bucket_size = max_keys;
  
  // create empty array of max_keys records
  Current_size = 0;
  Records = new Record[max_keys];
};

Bucket::~Bucket() {
  delete[] Records;
};

int Bucket::Insert(Record& record) {
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

void Bucket::Remove(char key[12]) {
  // search for record in bucket (o(k^2))
  for (int i = 0; i < Current_size; i++) {
    if (Records[i].isrc == key) {
      // remove record from bucket shifting all records to the left (new space complexity O(k))
      for (int j = i; j < Current_size - 1; j++) {
        Records[j] = Records[j + 1];
      };
      Current_size--;
      break;
    };
  };
};

void Bucket::Print() {
  cout << "Local depth: " << Local_depth << endl;
  cout << "Number of records: " << Current_size << endl;

  for (int i = 0; i < Current_size; i++) {
    Records[i].Print();
  };
};
