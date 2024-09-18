#include "ExtendibleHashing.hpp"
#include "Bucket.cpp"
#include <cmath>
#include <functional>
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
int ExtendibleHashing::Hash(char key[12]) {
  // convert key to hash using hash function (char*)
  string str_key(key, 12);
  hash<string> hash_str;
  size_t key_hash = hash_str(str_key);

  // return the global depth most significant bits of the key - index of the bucket in the directory
  return key_hash & ((1 << Global_depth) - 1); 
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
  File_buckets.flush();

  // save the bucket address in the directory
  Bucket_addresses[bucket_directory_index] = bucket_offset;
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

// insert record handling hash function and split bucket
bool ExtendibleHashing::Insert(Record record) {
  int bucket_directory_index = Hash(record.isrc);

  while (true) {
    Bucket* bucket = Load_bucket(bucket_directory_index);

    if (bucket->Current_size < bucket->Max_bucket_size) {
      bucket->Insert(record);
      cout << "[+] Inserted record and saved key " << record.isrc << " in bucket in position " << bucket_directory_index << endl;
      Save_bucket(bucket, bucket_directory_index);
      return true;
    }

    cout << "[!] Bucket is full. Splitting bucket" << endl;
    Split_bucket(record.isrc, bucket_directory_index);

    // Recompute bucket index after splitting
    bucket_directory_index = Hash(record.isrc);
  }
}


// handle split bucket and expand directory
void ExtendibleHashing::Split_bucket(char key[12], int bucket_directory_index) {
  Bucket* old_bucket = Load_bucket(bucket_directory_index);

  // if local depth is equal to global depth, expand directory
  if (old_bucket->Local_depth == Global_depth) {
    cout << "[!] Expanding directory" << endl;
    Expand_directory();
  }

  // increment the local depth of the bucket
  old_bucket->Local_depth++;

  // create new bucket with the same max size
  Bucket* new_bucket = new Bucket(old_bucket->Max_bucket_size);
  new_bucket->Local_depth = old_bucket->Local_depth;

  // calculate new bucket index
  int new_bucket_index = bucket_directory_index | (1 << (old_bucket->Local_depth - 1));

  // redistribute the records between the two buckets (old and new)
  cout << "[+] Redistributing records between buckets" << endl;

  for (int i = 0; i < old_bucket->Current_size; i++) {
    Record* record = &old_bucket->Records[i];
    int hash = Hash(record->isrc);

    if ((hash & ((1 << old_bucket->Local_depth) - 1)) == new_bucket_index) {
      new_bucket->Insert(*record);
      old_bucket->Remove(record->isrc);
      i--; // Adjust index because we removed an element
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
void ExtendibleHashing::Expand_directory() {
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
  File_data.write((char*) (&Bucket_addresses[0]), Num_cells * sizeof(int));
  File_data.flush();
}

// print all records
void ExtendibleHashing::Print() {
  cout << endl << "[!] Print directory with " << Num_cells << " cells" << endl;
  
  for (int i = 0; i < Num_cells; i++) {
    cout << "[+] Cell " << i << " with bucket address " << Bucket_addresses[i] << endl;
    Bucket* bucket = Load_bucket(i);
    bucket->Print();
    cout << endl;
  };
};

// load from csv and insert into extendible hashing
void ExtendibleHashing::Load_csv(string filename) {
  ifstream file(filename, ios::in);
  if (!file.is_open()) {
    cout << "[!] Error. Can't open file: " << filename << endl;
    throw runtime_error("Error. Can't open file");
  }

  string line;
  getline(file, line); // skip header

  cout << "[!] Loading data from file " << filename << endl;
  while (getline(file, line)) {
    char track[100], album_name[100], artist[50], release_date[10], isrc[12];
    int all_time_rank = 0, track_score = 0, spotify_streams = 0, spotify_popularity = 0;
    int youtube_views = 0, tiktok_views = 0, airplay_spins = 0, siriusmxm_spins = 0;
    int pandora_streams = 0, soundcloud_streams = 0, shazam_counts = 0, explicit_tracks = 0;

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

    getline(ss, token, ','); strncpy(track, token.c_str(), sizeof(track) - 1); track[sizeof(track) - 1] = '\0';
    getline(ss, token, ','); strncpy(album_name, token.c_str(), sizeof(album_name) - 1); album_name[sizeof(album_name) - 1] = '\0';
    getline(ss, token, ','); strncpy(artist, token.c_str(), sizeof(artist) - 1); artist[sizeof(artist) - 1] = '\0';
    getline(ss, token, ','); strncpy(release_date, token.c_str(), sizeof(release_date) - 1); release_date[sizeof(release_date) - 1] = '\0';
    getline(ss, token, ','); strncpy(isrc, token.c_str(), sizeof(isrc) - 1); isrc[sizeof(isrc) - 1] = '\0';

    getline(ss, token, ','); all_time_rank = safe_stoi(token);
    getline(ss, token, ','); track_score = safe_stoi(token);
    getline(ss, token, ','); spotify_streams = safe_stoi(token);
    getline(ss, token, ','); spotify_popularity = safe_stoi(token);
    getline(ss, token, ','); youtube_views = safe_stoi(token);
    getline(ss, token, ','); tiktok_views = safe_stoi(token);
    getline(ss, token, ','); airplay_spins = safe_stoi(token);
    getline(ss, token, ','); siriusmxm_spins = safe_stoi(token);
    getline(ss, token, ','); pandora_streams = safe_stoi(token);
    getline(ss, token, ','); soundcloud_streams = safe_stoi(token);
    getline(ss, token, ','); shazam_counts = safe_stoi(token);
    getline(ss, token, ','); explicit_tracks = safe_stoi(token);

    Record new_record(track, album_name, artist, release_date, isrc, all_time_rank, track_score, spotify_streams, spotify_popularity, youtube_views, tiktok_views, airplay_spins, siriusmxm_spins, pandora_streams, soundcloud_streams, shazam_counts, explicit_tracks);
    new_record.Print();

    Insert(new_record);
  }
}
