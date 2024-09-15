#include "ExtendibleHashing.hpp"
#include "Bucket.cpp"
#include <cmath>
#include <iostream>

using namespace std;

ExtendibleHashing::ExtendibleHashing(int global_depth, int bucket_size) {
  // instance global depth and number of cells 2^global_depth
  Global_depth = global_depth;
  Num_cells = pow(2, Global_depth);
  Bucket_size = bucket_size;
  Bucket_addresses.resize(Num_cells, -1);
};

ExtendibleHashing::~ExtendibleHashing() {
  Close();
};

void ExtendibleHashing::Close() {
  // close files cuz we are done :)
  if (File_data.is_open()) {
    cout << "[-] Closing file data" << endl;
    File_data.close();
  };

  if (File_buckets.is_open()) {
    cout << "[-] Closing file buckets" << endl;
    File_buckets.close();
  };
};

void ExtendibleHashing::Create(string filename_data, string filename_buckets) {
  // open file for save data and save buckets
  File_data.open(filename_data, ios::in | ios::out | ios::binary);
  File_buckets.open(filename_buckets, ios::in | ios::out | ios::binary);

  // check if files are open else create new files
  if (!File_data.is_open()) {
    cout << "[+] File not exists. Creating file " << filename_data << endl;
    File_data.open(filename_data, ios::out | ios::in | ios::binary | ios::trunc);
    File_data.close();

    File_data.open(filename_data, ios::in | ios::out | ios::binary);
  };

  if (!File_buckets.is_open()) {
    cout << "[+] File not exists. Creating file " << filename_buckets << endl;
    File_buckets.open(filename_buckets, ios::out | ios::in | ios::binary | ios::trunc);
    File_buckets.close();

    File_buckets.open(filename_buckets, ios::in | ios::out | ios::binary);
  };

  // create empty two empty buckets with bucket size (handle 0 and 1)
  Bucket* bucket_zero = new Bucket(Bucket_size);
  Bucket* bucket_one = new Bucket(Bucket_size);

  // no hash needed, just save the buckets with address 0 and 1 (considering global depth = 1 always)
  Save_bucket(bucket_zero, 0);
  Save_bucket(bucket_one, 1);

  // save directory (with 0 and 1) to file, save num_cells * sizeof(int) bytes a bucket address
  File_data.seekp(0, ios::beg);
  File_data.write((char*) (&Bucket_addresses[0]), Num_cells * sizeof(Bucket*));
  
  File_data.flush();
};

// trying opening file for save data , if not return false
bool ExtendibleHashing::Open(string filedata, string filebuckets) {
  // open file for save data and save buckets
  File_data.open(filedata, ios::in | ios::out | ios::binary);
  File_buckets.open(filebuckets, ios::in | ios::out | ios::binary);

  if (!File_data.is_open()) {
    cout << "[!] Error opening file " << filedata << endl;
    return false;
  };

  if (!File_buckets.is_open()) {
    cout << "[!] Error opening file " << filebuckets << endl;
    return false;
  };

  // load the bucket addresses from the file and save it in the directory
  File_data.seekg(0, ios::beg);
  File_data.read((char*) (&Bucket_addresses[0]), Num_cells * sizeof(Bucket*));

  return true;
};

void ExtendibleHashing::Save_bucket(Bucket* bucket, int bucket_directory_index) {
  // move to the end of the file for append new bucket
  // tellp return the position of current pointer
  File_buckets.seekp(0, ios::end);
  int bucket_offset = File_buckets.tellp();

  // save the bucket to the file
  File_buckets.write((char*) bucket, sizeof(Bucket));

  // save the bucket address in the directory
  Bucket_addresses[bucket_directory_index] = bucket_offset;
  
  File_buckets.flush();
};

Bucket* ExtendibleHashing::Load_bucket(int bucket_directory_index) {
  // get the bucket address from the directory
  int bucket_offset = Bucket_addresses[bucket_directory_index];

  // move to the bucket address in the file
  File_buckets.seekg(bucket_offset, ios::beg);

  // read the bucket from the file
  Bucket* bucket = new Bucket(Bucket_size);
  File_buckets.read((char*) bucket, sizeof(Bucket));

  return bucket;
};
