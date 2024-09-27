#include "ExtendibleHashing.hpp"

int hash_function(string key, int depth) {
  hash<string> hash;
  return hash(key) % int(std::pow(2, depth));
}

template <class RECORD, class TK>
void ExtendibleHashing<RECORD, TK>::loadCSV(const string& filename) {
  std::ifstream file(filename);
  string line;
  vector<EHRecordB> records;

  if (!file.is_open()) {
    cerr << "No se pudo abrir el archivo.\n";
    return;
  }

  getline(file, line);

  while (getline(file, line)) {
    stringstream ss(line);
    string token;
    EHRecordB record;

    getline(ss, token, ',');
    record.year = stoi(token);

    getline(ss, token, ',');
    strncpy(record.make, token.c_str(), sizeof(record.make));
    record.make[sizeof(record.make) - 1] = '\0';

    getline(ss, token, ',');
    strncpy(record.model, token.c_str(), sizeof(record.model));
    record.model[sizeof(record.model) - 1] = '\0';

    getline(ss, token, '\n');
    strncpy(record.vin, token.c_str(), sizeof(record.vin));
    record.vin[sizeof(record.vin) - 1] = '\0';
    this->add(record);
  }

  file.close();
}
template <class RECORD, class TK>
// Constructor
ExtendibleHashing<RECORD, TK>::ExtendibleHashing(std::string _filename) {
  this->filename = _filename;
  if (!fileExists(this->dataName())) {
    this->init_file();
  }
}

template <class RECORD, class TK>
void ExtendibleHashing<RECORD, TK>::init_file() {
  std::cout << "Init starting..." << std::endl;

  std::fstream file(dataName(), std::ios::binary | std::ios::app |
                                    std::ios::in | std::ios::out);

  file.seekg(0, std::ios::end);
  int bytes = file.tellg();

  if (bytes) {
    std::cout << "Found file. Reading it..." << std::endl;
    // leer header free list
    file.seekg(0, std::ios::beg);
    file.read((char*)&this->pos_free_list, sizeof(this->pos_free_list));
    this->n_access++;
    file.close();
  } else {
    std::cout << "No file found. Creating it..." << std::endl;
    file.seekp(0, std::ios::beg);

    file.write((char*)&this->pos_free_list, sizeof(this->pos_free_list));
    this->n_access++;
    file.flush();

    Bucket<RECORD, TK> bucket(1, 0);
    file.write((char*)&bucket, sizeof(Bucket<RECORD, TK>));
    this->n_access++;
    file.flush();
    bucket = Bucket<RECORD, TK>(1, 1);
    file.write((char*)&bucket, sizeof(Bucket<RECORD, TK>));
    this->n_access++;
    file.flush();

    file.close();

    std::fstream index(idxName(), std::ios::binary | std::ios::app |
                                      std::ios::in | std::ios::out);

    for (int i = 0; i < std::pow(2, this->global_depth); i++) {
      Index_Page_H<RECORD, TK> idx;
      if (i % 2 == 0) {
        idx.pointer = 0;
      } else {
        idx.pointer = 1;
      }
      index.write((char*)&idx, sizeof(Index_Page_H<RECORD, TK>));
      this->n_access++;
      index.flush();
    }
    index.close();
  }
  std::cout << "Init done!" << std::endl;
}

template <class RECORD, class TK>
bool ExtendibleHashing<RECORD, TK>::add(RECORD reg) {
  std::fstream index_file(idxName(),
                          std::ios::binary | std::ios::in | std::ios::out);

  std::fstream data_file(dataName(),
                         std::ios::binary | std::ios::in | std::ios::out);

  int dir_pointer = hash_function(reg.key(), this->global_depth);
  int pointer = get_pointer(index_file, dir_pointer);  // pointer to bucket

  Bucket<RECORD, TK> bucket = read_data_page(data_file, pointer);
  if (bucket.size < MD_H) {
    bucket.registers[bucket.size] = reg;
    bucket.size++;
    write_data_page(data_file, bucket, pointer);
  } else if (bucket.local_depth < this->global_depth) {
    Bucket<RECORD, TK> new_bucket(bucket.local_depth, bucket.bits);

    add_bit(bucket, 0);
    add_bit(new_bucket, 1);

    RECORD* temp = new RECORD[bucket.size + 1];
    for (int i = 0; i < bucket.size; i++) {
      temp[i] = bucket.registers[i];
    }
    temp[bucket.size] = reg;
    int length_temp = bucket.size + 1;

    bucket.size = 0;
    new_bucket.size = 0;

    bucket.next = -1;
    new_bucket.next = -1;

    write_data_page(data_file, bucket, pointer);
    int pointer_new_page = write_new_page(data_file, new_bucket);

    re_linked_index(index_file, bucket, new_bucket, pointer, pointer_new_page);

    for (int i = 0; i < length_temp; i++) {
      add(temp[i]);
    }

    delete[] temp;

  } else {
    Bucket<RECORD, TK> new_bucket(bucket.local_depth, bucket.bits);
    new_bucket.size = 0;
    new_bucket.registers[new_bucket.size] = reg;
    new_bucket.size++;

    int pointer_new_page = write_new_page(data_file, new_bucket);

    bucket.next = pointer_new_page;
    write_data_page(data_file, bucket, pointer);
  }

  index_file.close();
  data_file.close();
  return true;
}

template <class RECORD, class TK>
bool ExtendibleHashing<RECORD, TK>::remove(TK key) {
  std::fstream index_file(idxName(),
                          std::ios::binary | std::ios::in | std::ios::out);

  std::fstream data_file(dataName(),
                         std::ios::binary | std::ios::in | std::ios::out);

  int dir_pointer = hash_function(key, this->global_depth);
  int pointer = get_pointer(index_file, dir_pointer);

  Bucket<RECORD, TK> bucket_prev;

  int pointer_prev = -1;
  int count_overflow = 0;
  bool found = false;

  do {
    Bucket<RECORD, TK> bucket = read_data_page(data_file, pointer);
    for (int i = 0; i < bucket.size; i++) {
      if (bucket.registers[i].key() == key) {
        found = true;
        if (bucket.size > 1) {
          bucket.registers[i] = bucket.registers[bucket.size - 1];
          bucket.size--;
          write_data_page(data_file, bucket, pointer);
        } else if (count_overflow == 0 && bucket.next < 0) {
          bucket.size = 0;

          bucket.free_list = this->pos_free_list;
          this->pos_free_list = pointer;
          write_data_page(data_file, bucket, pointer);

          re_linked_index_merge(index_file, bucket);
        } else {  // overflow
          if (count_overflow == 0) {
            Bucket<RECORD, TK> next_bucket =
                read_data_page(data_file, bucket.next);

            write_data_page(data_file, next_bucket, pointer);

            bucket.free_list = this->pos_free_list;
            this->pos_free_list = pointer;
            bucket.size = 0;
            bucket.next = -1;
            write_data_page(data_file, bucket, pointer);
          } else {
            bucket_prev.next = bucket.next;

            bucket.size = 0;
            bucket.free_list = this->pos_free_list;
            this->pos_free_list = pointer;
            write_data_page(data_file, bucket, pointer);
            write_data_page(data_file, bucket_prev, pointer_prev);
          }
        }
      }
    }
    if (bucket.next < 0)
      break;
    pointer_prev = pointer;
    pointer = bucket.next;
    bucket_prev = bucket;
    count_overflow++;

  } while (true);

  index_file.close();
  data_file.close();
  return found;
}

template <class RECORD, class TK>
std::vector<RECORD> ExtendibleHashing<RECORD, TK>::search(TK key) {
  std::fstream index_file(idxName(),
                          std::ios::binary | std::ios::in | std::ios::out);

  std::fstream data_file(dataName(),
                         std::ios::binary | std::ios::in | std::ios::out);

  int dir_pointer = hash_function(key, this->global_depth);
  int pointer = get_pointer(index_file, dir_pointer);

  index_file.close();

  std::vector<RECORD> result;
  do {
    Bucket<RECORD, TK> bucket = read_data_page(data_file, pointer);
    for (int i = 0; i < bucket.size; i++) {
      if (bucket.registers[i].key() == key) {
        result.push_back(bucket.registers[i]);
      }
    }
    if (bucket.next < 0)
      break;

    pointer = bucket.next;

  } while (true);

  return result;
}

template <class RECORD, class TK>
std::vector<RECORD> ExtendibleHashing<RECORD, TK>::range_search(TK begin_key,
                                                                TK end_key) {
  std::cout << "Extendible Hashing does not support range search" << std::endl;
  return std::vector<RECORD>();
}

template <class RECORD, class TK>
std::string ExtendibleHashing<RECORD, TK>::dataName() {
  return this->filename + DAT_EXT;
}

template <class RECORD, class TK>
std::string ExtendibleHashing<RECORD, TK>::idxName() {
  return this->filename + IDX_EXT;
}

template <class RECORD, class TK>
bool ExtendibleHashing<RECORD, TK>::fileExists(const std::string& name) {
  std::ifstream f(name.c_str(),
                  std::ios::in | std::ios::binary | std::ios::ate);
  return f.good();
}

template <class RECORD, class TK>
void ExtendibleHashing<RECORD, TK>::add_bit(Bucket<RECORD, TK>& bucket,
                                            int value) {
  bucket.bits.set(bucket.local_depth, value);
  bucket.local_depth++;
}

template <class RECORD, class TK>
int ExtendibleHashing<RECORD, TK>::get_pointer(std::fstream& file, int index) {
  file.seekg(index * sizeof(Index_Page_H<RECORD, TK>), std::ios::beg);
  Index_Page_H<RECORD, TK> idx;
  file.read((char*)&idx, sizeof(Index_Page_H<RECORD, TK>));
  this->n_access++;
  return idx.pointer;
}

template <class RECORD, class TK>
Bucket<RECORD, TK> ExtendibleHashing<RECORD, TK>::read_data_page(
    std::fstream& file,
    int index) {
  file.seekg(index * sizeof(Bucket<RECORD, TK>) + sizeof(int), std::ios::beg);
  Bucket<RECORD, TK> bucket;
  file.read((char*)&bucket, sizeof(Bucket<RECORD, TK>));
  this->n_access++;
  return bucket;
}
template <class RECORD, class TK>
void ExtendibleHashing<RECORD, TK>::write_data_page(std::fstream& file,
                                                    Bucket<RECORD, TK> bucket,
                                                    int index) {
  file.seekp(index * sizeof(Bucket<RECORD, TK>) + sizeof(int), std::ios::beg);
  file.write((char*)&bucket, sizeof(Bucket<RECORD, TK>));
  this->n_access++;
  file.flush();
}

template <class RECORD, class TK>
int ExtendibleHashing<RECORD, TK>::write_new_page(std::fstream& file,
                                                  Bucket<RECORD, TK>& bucket) {
  if (this->pos_free_list == -1) {
    file.seekp(0, std::ios::end);
    file.write((char*)&bucket, sizeof(Bucket<RECORD, TK>));
    this->n_access++;
    file.flush();

    std::streampos pos = file.tellp();
    std::streamoff offset = pos - std::streamoff(sizeof(int));
    return (offset / std::streamoff(sizeof(Bucket<RECORD, TK>))) - 1;
  } else {
    Bucket<RECORD, TK> temp = read_data_page(file, this->pos_free_list);
    file.seekp(this->pos_free_list * sizeof(Bucket<RECORD, TK>) + sizeof(int),
               std::ios::beg);
    file.write((char*)&bucket, sizeof(Bucket<RECORD, TK>));
    this->n_access++;
    file.flush();
    this->pos_free_list = temp.free_list;

    std::streampos pos = file.tellp();
    std::streamoff offset = pos - std::streamoff(sizeof(int));
    return (offset / std::streamoff(sizeof(Bucket<RECORD, TK>))) - 1;
  }
}

template <class RECORD, class TK>
void ExtendibleHashing<RECORD, TK>::re_linked_index(
    std::fstream& file,
    Bucket<RECORD, TK>& bucket_1,
    Bucket<RECORD, TK>& bucket_2,
    int pointer1,
    int pointer2) {
  int sub_j = this->global_depth - bucket_1.local_depth;
  int limit = pow(2, sub_j);

  std::string bits_bucket =
      bucket_1.bits.to_string().substr(sub_j, bucket_1.local_depth);
  std::string bits_new_bucket =
      bucket_2.bits.to_string().substr(sub_j, bucket_2.local_depth);

  for (int i = 0; i < limit; i++) {
    std::bitset<GD> i_bit(i);
    std::string i_str = i_bit.to_string().substr(i_bit.size() - sub_j, sub_j);

    std::string i_str_1 = i_str + bits_bucket;
    std::string i_str_2 = i_str + bits_new_bucket;

    std::bitset<GD> i_str_bit_1(i_str_1);
    int index_1 = i_str_bit_1.to_ulong();

    std::bitset<GD> i_str_bit_2(i_str_2);
    int index_2 = i_str_bit_2.to_ulong();

    file.seekp(index_1 * sizeof(Index_Page_H<RECORD, TK>), std::ios::beg);
    Index_Page_H<RECORD, TK> idx;
    idx.pointer = pointer1;
    file.write((char*)&idx, sizeof(Index_Page_H<RECORD, TK>));
    this->n_access++;
    file.flush();

    file.seekp(index_2 * sizeof(Index_Page_H<RECORD, TK>), std::ios::beg);
    idx.pointer = pointer2;
    file.write((char*)&idx, sizeof(Index_Page_H<RECORD, TK>));
    this->n_access++;
    file.flush();
  }
}

template <class RECORD, class TK>
void ExtendibleHashing<RECORD, TK>::re_linked_index_merge(
    std::fstream& file,
    Bucket<RECORD, TK>& bucket_1) {

  Index_Page_H<RECORD, TK> idx1;
  int index_to_merge = get_index_to_merge(bucket_1);
  file.seekg(index_to_merge * sizeof(Bucket<RECORD, TK>), std::ios::beg);
  file.read((char*)&idx1, sizeof(Index_Page_H<RECORD, TK>));
  this->n_access++;
  int pointer_be_persist = idx1.pointer;

  int sub_j = this->global_depth - bucket_1.local_depth + 1;
  int limit = pow(2, sub_j);

  std::string bits_bucket =
      bucket_1.bits.to_string().substr(sub_j, bucket_1.local_depth - 1);

  Index_Page_H<RECORD, TK> idx;
  for (int i = 0; i < limit; i++) {
    std::bitset<GD> i_bit(i);
    std::string i_str = i_bit.to_string().substr(i_bit.size() - sub_j, sub_j);

    std::string i_str_1 = i_str + bits_bucket;

    std::bitset<GD> i_str_bit_1(i_str_1);
    int index_1 = i_str_bit_1.to_ulong();

    file.seekp(index_1 * sizeof(Index_Page_H<RECORD, TK>), std::ios::beg);
    idx.pointer = pointer_be_persist;
    file.write((char*)&idx, sizeof(Index_Page_H<RECORD, TK>));
    this->n_access++;
    file.flush();
  }
}

template <class RECORD, class TK>
int ExtendibleHashing<RECORD, TK>::get_index_to_merge(
    Bucket<RECORD, TK>& bucket) {

  int sub_j = this->global_depth - bucket.local_depth;
  int limit = pow(2, sub_j);

  std::string bits_bucket =
      bucket.bits.to_string().substr(sub_j, bucket.local_depth);

  if (bits_bucket[0] == '0') {
    bits_bucket[0] = '1';

  } else {
    bits_bucket[0] = '0';
  }

  std::bitset<GD> bits_bucket_bit(bits_bucket);
  int index = bits_bucket_bit.to_ulong();

  return index;
}