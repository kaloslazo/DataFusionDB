#include "Bucket.hpp"
#include "Record.cpp"

Bucket::Bucket(int max_keys) {
  // initialize local depth with 1 and max bucket size (records in bucket)
  Local_depth = 0;
  Max_bucket_size = max_keys;
  
  // create empty array of max_keys records
  Records = new Record[max_keys];
};
