#include "ExtendibleHashing.hpp"
#include <cmath>
#include <functional>
#include <iostream>
#include <typeinfo>
#include "Bucket.cpp"

using namespace std;

template <class RECORD, class TK>
ExtendibleHashing<RECORD, TK>::ExtendibleHashing(int global_depth,
                                                 int bucket_size) {
  // instance global depth and number of cells 2^global_depth
  Global_depth = global_depth;
  Num_cells = pow(2, Global_depth);
  Bucket_size = bucket_size;
  Bucket_addresses.resize(Num_cells, -1);
};

template <class RECORD, class TK>
ExtendibleHashing<RECORD, TK>::~ExtendibleHashing() {
  Close();
};

template <class RECORD, class TK>
void ExtendibleHashing<RECORD, TK>::Close() {
  // close files cuz we are done :)
  cout << "[!] Closing files" << endl;

  if (File_data.is_open()) {
    cout << "[-] Closing file data" << endl;
    File_data.close();
  };

  if (File_buckets.is_open()) {
    cout << "[-] Closing file buckets" << endl;
    File_buckets.close();
  };
};

// hash function converts key to hash value using binary approach
template <class RECORD, class TK>
int ExtendibleHashing<RECORD, TK>::Hash(TK key) {
  // convert key to hash using hash function (char*)
  hash<TK> hash_key;
  size_t key_hash = hash_key(key);

  // return the global depth most significant bits of the key - index of the
  // bucket in the directory
  return key_hash & ((1 << Global_depth) - 1);
};

template <class RECORD, class TK>
void ExtendibleHashing<RECORD, TK>::Create(string filename_data,
                                           string filename_buckets) {
  // open file for save data and save buckets
  File_data.open(filename_data, ios::in | ios::out | ios::binary);
  File_buckets.open(filename_buckets, ios::in | ios::out | ios::binary);

  // check if files are open else create new files
  if (!File_data.is_open()) {
    cout << "[+] File not exists. Creating file " << filename_data << endl;
    File_data.open(filename_data,
                   ios::out | ios::in | ios::binary | ios::trunc);
    File_data.close();

    File_data.open(filename_data, ios::in | ios::out | ios::binary);
  };

  if (!File_buckets.is_open()) {
    cout << "[+] File not exists. Creating file " << filename_buckets << endl;
    File_buckets.open(filename_buckets,
                      ios::out | ios::in | ios::binary | ios::trunc);
    File_buckets.close();

    File_buckets.open(filename_buckets, ios::in | ios::out | ios::binary);
  };

  // create empty two empty buckets with bucket size (handle 0 and 1)
  Bucket<RECORD, TK>* bucket_zero = new Bucket<RECORD, TK>(Bucket_size);
  Bucket<RECORD, TK>* bucket_one = new Bucket<RECORD, TK>(Bucket_size);

  // no hash needed, just save the buckets with address 0 and 1 (considering
  // global depth = 1 always)
  Save_bucket(bucket_zero, 0);
  Save_bucket(bucket_one, 1);

  // save directory (with 0 and 1) to file, save num_cells * sizeof(int) bytes a
  // bucket address
  File_data.seekp(0, ios::beg);
  File_data.write((char*)(&Bucket_addresses[0]),
                  Num_cells * sizeof(Bucket<RECORD, TK>*));

  File_data.flush();
};

// trying opening file for save data , if not return false
template <class RECORD, class TK>
bool ExtendibleHashing<RECORD, TK>::Open(string filedata, string filebuckets) {
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
  File_data.read((char*)(&Bucket_addresses[0]),
                 Num_cells * sizeof(Bucket<RECORD, TK>*));

  return true;
};

template <class RECORD, class TK>
void ExtendibleHashing<RECORD, TK>::Save_bucket(Bucket<RECORD, TK>* bucket,
                                                int bucket_directory_index) {
  // move to the end of the file for append new bucket
  // tellp return the position of current pointer
  File_buckets.seekp(0, ios::end);
  int bucket_offset = File_buckets.tellp();

  // save the bucket to the file
  File_buckets.write((char*)bucket, sizeof(Bucket<RECORD, TK>));
  File_buckets.flush();

  // save the bucket address in the directory
  Bucket_addresses[bucket_directory_index] = bucket_offset;
};

template <class RECORD, class TK>
Bucket<RECORD, TK>* ExtendibleHashing<RECORD, TK>::Load_bucket(
    int bucket_directory_index) {
  // get the bucket address from the directory
  int bucket_offset = Bucket_addresses[bucket_directory_index];

  // move to the bucket address in the file
  File_buckets.seekg(bucket_offset, ios::beg);

  // read the bucket from the file
  Bucket<RECORD, TK>* bucket = new Bucket<RECORD, TK>(Bucket_size);
  File_buckets.read((char*)bucket, sizeof(Bucket<RECORD, TK>));

  return bucket;
};

// insert record handling hash function and split bucket
template <class RECORD, class TK>
bool ExtendibleHashing<RECORD, TK>::Insert(RECORD record) {
  int bucket_directory_index = Hash(record.key());

  while (true) {
    Bucket<RECORD, TK>* bucket = Load_bucket(bucket_directory_index);

    if (bucket->Current_size < bucket->Max_bucket_size) {
      bucket->Insert(record);
      cout << "[+] Inserted record and saved key " << record.key()
           << " in bucket in position " << bucket_directory_index << endl;
      Save_bucket(bucket, bucket_directory_index);
      return true;
    }

    cout << "[!] Bucket is full. Splitting bucket" << endl;
    Split_bucket(record.key(), bucket_directory_index);

    // Recompute bucket index after splitting
    bucket_directory_index = Hash(record.key());
  }
}

// search with key in bucket
template <class RECORD, class TK>
optional<RECORD> ExtendibleHashing<RECORD, TK>::Search(TK key) {
  // Calculate the bucket index using the hash function
  int bucket_index = Hash(key);

  // Get the bucket address
  int bucket_address = Bucket_addresses[bucket_index];
  cout << "[+] Bucket address: " << bucket_address << endl;

  // Load the bucket from the file
  Bucket<RECORD, TK>* bucket = Load_bucket(bucket_address);

  // Search the bucket for the record with the given key
  RECORD* result = bucket->Search(key);

  if (result != nullptr) {
    return *result;
  }
  return nullopt;
}

// handle split bucket and expand directory
template <class RECORD, class TK>
void ExtendibleHashing<RECORD, TK>::Split_bucket(TK key,
                                                 int bucket_directory_index) {
  Bucket<RECORD, TK>* old_bucket = Load_bucket(bucket_directory_index);

  // if local depth is equal to global depth, expand directory
  if (old_bucket->Local_depth == Global_depth) {
    cout << "[!] Expanding directory" << endl;
    Expand_directory();
  }

  // increment the local depth of the bucket
  old_bucket->Local_depth++;

  // create new bucket with the same max size
  Bucket<RECORD, TK>* new_bucket =
      new Bucket<RECORD, TK>(old_bucket->Max_bucket_size);
  new_bucket->Local_depth = old_bucket->Local_depth;

  // calculate new bucket index
  int new_bucket_index =
      bucket_directory_index | (1 << (old_bucket->Local_depth - 1));

  // redistribute the records between the two buckets (old and new)
  cout << "[+] Redistributing records between buckets" << endl;

  for (int i = 0; i < old_bucket->Current_size; i++) {
    RECORD* record = &old_bucket->Records[i];
    int hash = Hash(record->key());

    if ((hash & ((1 << old_bucket->Local_depth) - 1)) == new_bucket_index) {
      new_bucket->Insert(*record);
      old_bucket->Remove(record->key());
      i--;  // Adjust index because we removed an element
    }
  }

  // Update directory
  for (int i = 0; i < Num_cells; i++) {
    if ((i & ((1 << old_bucket->Local_depth) - 1)) == new_bucket_index) {
      Bucket_addresses[i] = File_buckets.tellp();
    }
  };

  cout << "[+] Redistributed records" << endl;
  Save_bucket(old_bucket, bucket_directory_index);
  Save_bucket(new_bucket, new_bucket_index);
};

// expand directory (double the size of the directory)
template <class RECORD, class TK>
void ExtendibleHashing<RECORD, TK>::Expand_directory() {
  // double size of directory
  int new_num_cells = 2 * Num_cells;
  vector<int> new_bucket_addresses(new_num_cells);

  // copy the old directory to the new directory
  for (int i = 0; i < Num_cells; i++) {
    new_bucket_addresses[i] = Bucket_addresses[i];
    new_bucket_addresses[i + Num_cells] = Bucket_addresses[i];
  }

  Global_depth++;
  Num_cells = new_num_cells;
  Bucket_addresses = new_bucket_addresses;

  // Update the file with the new directory
  File_data.seekp(0, ios::beg);
  File_data.write((char*)(&Bucket_addresses[0]), Num_cells * sizeof(int));
  File_data.flush();
}

// print all records
template <class RECORD, class TK>
void ExtendibleHashing<RECORD, TK>::Print() {
  cout << endl << "[!] Print directory with " << Num_cells << " cells" << endl;

  for (int i = 0; i < Num_cells; i++) {
    cout << "[+] Cell " << i << " with bucket address " << Bucket_addresses[i]
         << endl;
    Bucket<RECORD, TK>* bucket = Load_bucket(i);
    bucket->Print();
    cout << endl;
  };
};

// load from csv and insert into extendible hashing
template <class RECORD, class TK>
void ExtendibleHashing<RECORD, TK>::Load_csv(string filename) {
  ifstream file(filename, ios::in);
  if (!file.is_open()) {
    cout << "[!] Error. Can't open file: " << filename << endl;
    throw runtime_error("Error. Can't open file");
  }

  string line;
  getline(file, line);  // skip header

  cout << "[!] Loading data from file " << filename << endl;
  while (getline(file, line)) {
    // one by each RECORD class atributes
    RECORD record;
    stringstream ss(line);
    string token;
    auto safe_stoi = [](const string& str, int default_value = 0) -> int {
      try {
        return str.empty() ? default_value : stoi(str);
      } catch (const std::invalid_argument&) {
        return default_value;
      } catch (const std::out_of_range&) {
        return default_value;
      }
    };
    // if (typeid(RECORD) == typeid(EHRecordA)) {
    char id[23];
    char name[521];
    char album[244];
    char album_id[23];
    char artists[1124];
    getline(ss, token, ',');
    strncpy(id, token.c_str(), sizeof(id) - 1);
    id[sizeof(id) - 1] = '\0';
    getline(ss, token, ',');
    strncpy(name, token.c_str(), sizeof(name) - 1);
    name[sizeof(name) - 1] = '\0';
    getline(ss, token, ',');
    strncpy(album, token.c_str(), sizeof(album) - 1);
    album[sizeof(album) - 1] = '\0';
    getline(ss, token, ',');
    strncpy(album_id, token.c_str(), sizeof(album_id) - 1);
    album_id[sizeof(album_id) - 1] = '\0';
    getline(ss, token, ',');
    strncpy(artists, token.c_str(), sizeof(artists) - 1);
    artists[sizeof(artists) - 1] = '\0';
    cout << "ID: " << id << endl;

    record = RECORD(id, name, album, album_id, artists);
    record.Print();

    Insert(record);
    // } else if (typeid(RECORD) == typeid(EHRecordB)) {
    //   int year;
    //   char make[30];
    //   char model[50];
    //   char vin[18];
    //   getline(ss, token, ',');
    //   // to int year
    //   year = stoi(token);
    //   getline(ss, token, ',');
    //   strncpy(make, token.c_str(), sizeof(make) - 1);
    //   make[sizeof(make) - 1] = '\0';
    //   getline(ss, token, ',');
    //   strncpy(model, token.c_str(), sizeof(model) - 1);
    //   model[sizeof(model) - 1] = '\0';
    //   getline(ss, token, ',');
    //   strncpy(vin, token.c_str(), sizeof(vin) - 1);
    //   vin[sizeof(vin) - 1] = '\0';
    //   getline(ss, token, ',');
    //   strncpy(vin, token.c_str(), sizeof(vin) - 1);
    //   vin[sizeof(vin) - 1] = '\0';

    //   record = RECORD(year, make, model, vin);

    //   record.Print();

    //   Insert(record);

    //   return;
    // }
  }
}
