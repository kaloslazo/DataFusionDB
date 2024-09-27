#ifndef SQLPARSER_H
#define SQLPARSER_H

#include <optional>
#include <string>
#include <vector>
#include "AvlFile/AvlFileRA.hpp"
#include "AvlFile/AvlFileRB.hpp"
#include "ExtendibleHashingFile/ExtendibleHashing.hpp"
#include "ExtendibleHashingFile/ExtendibleRA.hpp"
#include "ExtendibleHashingFile/ExtendibleRB.hpp"
#include "Record.hpp"
#include "SequentialFile/SequentialFileRA.hpp"
#include "SequentialFile/SequentialFileRB.hpp"

using namespace std;

enum IndexType { INDEX_UNKNOWN, INDEX_AVL, INDEX_SEQUENTIAL, INDEX_HASH };

enum RecordType { TYPE_RECORD_A, TYPE_RECORD_B };

class SQLParser {
 private:
  vector<Record*> select_query(const string& query);
  void create_table(const string& query);
  void insert_query(const string& query);
  void update_query(const string& query);
  void delete_query(const string& query);
  void index_query(const string& query);
  void create_index(const string& query);
  Record* create_record(const vector<string>& values);
  void set_record_type(const string& header_csv);
  std::optional<Record*> search_id(const string& id);
  vector<Record*> search_range(const string& id1, const string& id2);
  void create_index_structure();

  vector<Record*> records;
  string filename;
  string table_name;
  bool table_created = false;
  RecordType record_type;
  IndexType index_type;

  // AVL structures
  AVLFileRA<string>* avl_a;
  AVLFileRB<string>* avl_b;

  // Sequential File structures
  SequentialFileA* seq_a;
  SequentialFileB* seq_b;

  // Extendible Hashing structures
  ExtendibleHashing<EHRecordA, std::string>* eh_a;
  ExtendibleHashing<EHRecordB, std::string>* eh_b;

 public:
  SQLParser();
  ~SQLParser();
  vector<Record*> execute_query(const string& query);
  bool compare_values(const string& record_value,
                      const string& op,
                      const string& query_value);
};

#endif  // SQLPARSER_H
