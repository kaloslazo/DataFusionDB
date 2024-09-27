#ifndef EXTENDIBLEHASHING_HPP
#define EXTENDIBLEHASHING_HPP
#include <bitset>
#include <cmath>
#include <fstream>
#include <vector>
#include "EHRecordA.hpp"
#include "EHRecordB.hpp"

#define GD 16  
#define PAGE_SIZE_H 4096

const int MD_H = 20;
const string DAT_EXT = "_data.dat";
const string IDX_EXT = "_idx.dat";

template <class RECORD, class TK>
struct Index_Page_H {
  int pointer;

  Index_Page_H() : pointer(0) {}
  Index_Page_H(int _pointer) : pointer(_pointer) {}
};

template <class RECORD, class TK>
struct Bucket {
  int size;
  int local_depth;
  int next;

  int free_list;

  std::bitset<GD> bits;

  RECORD registers[MD_H];
  Bucket() : size(0), local_depth(1), next(-1), bits(0), free_list(-1) {}
  Bucket(int _ld, std::bitset<GD> _bits)
      : size(0), local_depth(_ld), next(-1), bits(_bits), free_list(-1) {}
};

template <class RECORD, class TK>
class ExtendibleHashing {
  std::string idxName();
  std::string dataName();
  string filename;

  int pos_free_list = -1;
  int global_depth = GD;

  void init_file();

  void set_pointer(std::fstream& file, int index, int value);

  int get_size(std::fstream& file, int index);
  void set_size(std::fstream& file, int index, int value);
  int get_ld(std::fstream& file, int index);
  void set_ld(std::fstream& file, int index, int value);

  bool fileExists(const std::string& name);
  void add_bit(Bucket<RECORD, TK>& bucket, int value);
  int get_pointer(std::fstream& file, int index);

  Bucket<RECORD, TK> read_data_page(std::fstream& file, int index);
  void write_data_page(std::fstream& file,
                       Bucket<RECORD, TK> bucket,
                       int index);

  int write_new_page(std::fstream& file, Bucket<RECORD, TK>& bucket);

  void re_linked_index(std::fstream& file,
                       Bucket<RECORD, TK>& bucket_1,
                       Bucket<RECORD, TK>& bucket_2,
                       int pointer1,
                       int pointer2);

  void re_linked_index_merge(std::fstream& file, Bucket<RECORD, TK>& bucket_1);

  int get_index_to_merge(Bucket<RECORD, TK>& bucket);

 public:
  int n_access = 0;
  ExtendibleHashing(std::string _filename);
  bool add(RECORD reg);
  bool remove(TK key);
  std::vector<RECORD> search(TK key);
  std::vector<RECORD> range_search(TK begin_key, TK end_key);
  void loadCSV(const string& filename);
};

#endif  // EXTENDIBLEHASHING_HPP
