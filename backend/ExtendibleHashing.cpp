#include "ExtendibleHashing.hpp"
#include <cmath>
#include <functional>
#include <iostream>
#include <typeinfo>
#
#include "Bucket.cpp"

using namespace std;

template <class RECORD, class TK>
ExtendibleHashing<RECORD, TK>::ExtendibleHashing(int global_depth, int bucket_size) {
    Global_depth = global_depth;
    Num_cells = pow(2, Global_depth);
    Bucket_size = bucket_size;
    Bucket_addresses.resize(Num_cells, -1);
}

template <class RECORD, class TK>
ExtendibleHashing<RECORD, TK>::~ExtendibleHashing() {
    Close();
}

template <class RECORD, class TK>
void ExtendibleHashing<RECORD, TK>::Close() {
    std::cout << "[!] Closing files" << std::endl;
    if (File_data.is_open()) {
        std::cout << "[-] Closing file data" << std::endl;
        File_data.close();
    }
    if (File_buckets.is_open()) {
        std::cout << "[-] Closing file buckets" << std::endl;
        File_buckets.close();
    }
}

template <class RECORD, class TK>
int ExtendibleHashing<RECORD, TK>::Hash(TK key) {
    std::hash<TK> hash_key;
    size_t key_hash = hash_key(key);
    int bucket_index = key_hash & ((1 << Global_depth) - 1);
    std::cout << "Hashing key: " << key << ", Hash value: " << bucket_index << std::endl;
    return bucket_index;
}

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
            std::cout << "[+] Inserted record and saved key " << record.key()
                      << " in bucket in position " << bucket_directory_index << std::endl;
            Save_bucket(bucket, bucket_directory_index);
            delete bucket;
            return true;
        }

        std::cout << "[!] Bucket is full. Splitting bucket" << std::endl;
        Split_bucket(record.key(), bucket_directory_index);
        delete bucket;

        // Recompute bucket index after splitting
        bucket_directory_index = Hash(record.key());
    }
}

template <class RECORD, class TK>
std::optional<RECORD> ExtendibleHashing<RECORD, TK>::Search(TK key) {
    int bucket_index = Hash(key);
    std::cout << "Searching in bucket: " << bucket_index << std::endl;
    Bucket<RECORD, TK>* bucket = Load_bucket(bucket_index);
    std::cout << "Loaded bucket at address: " << bucket << std::endl;
    std::optional<RECORD> result = bucket->Search(key);
    if (result) {
        std::cout << "Record found in bucket" << std::endl;
    } else {
        std::cout << "Record not found in bucket" << std::endl;
    }
    delete bucket;
    return result;
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
void ExtendibleHashing<RECORD, TK>::Load_csv(std::string filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "[!] Error. Can't open file: " << filename << std::endl;
        throw std::runtime_error("Error. Can't open file");
    }

    std::string line;
    std::getline(file, line);  // skip header

    std::cout << "[!] Loading data from file " << filename << std::endl;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string token;

        char id[23], name[521], album[244], album_id[23], artists[1124];

        std::getline(ss, token, ',');
        std::strncpy(id, token.c_str(), sizeof(id) - 1);
        id[sizeof(id) - 1] = '\0';

        std::getline(ss, token, ',');
        std::strncpy(name, token.c_str(), sizeof(name) - 1);
        name[sizeof(name) - 1] = '\0';

        std::getline(ss, token, ',');
        std::strncpy(album, token.c_str(), sizeof(album) - 1);
        album[sizeof(album) - 1] = '\0';

        std::getline(ss, token, ',');
        std::strncpy(album_id, token.c_str(), sizeof(album_id) - 1);
        album_id[sizeof(album_id) - 1] = '\0';

        std::getline(ss, token, ',');
        std::strncpy(artists, token.c_str(), sizeof(artists) - 1);
        artists[sizeof(artists) - 1] = '\0';

        std::cout << "ID: " << id << std::endl;

        RECORD record(id, name, album, album_id, artists);
        record.Print();

        Insert(record);
    }
}