#ifndef SQLPARSER_H
#define SQLPARSER_H

#include <string>
#include <vector>

#include "Record.hpp"
#include "AVLFileA.hpp"
#include "AVLFileB.hpp"

using namespace std;

enum IndexType {
  INDEX_UNKNOWN,
  INDEX_AVL,
  INDEX_EXTENDIBLE,
  INDEX_SEQUENTIAL
};

enum RecordType {
  TYPE_RECORD_A,
  TYPE_RECORD_B
};

class SQLParser {
private:
  vector<Record*> select_query(const string &query);
  void create_table(const string &query);
  void insert_query(const string &query);
  void update_query(const string &query);
  void delete_query(const string &query);

  // handle the record type
  Record* create_record(const vector<string>& values);
  void set_record_type(const string& header_csv);

  Record* search_id(const string& id);
  vector<Record*> search_range(const string& id1, const string& id2);

  vector<Record*> records;
  string filename;
  string table_name;
  bool table_created = false;
  
  RecordType record_type;
  IndexType index_type;

  AVLFileA<char[23]>* avl_a;
  AVLFileB<char[18]>* avl_b;

public:
  SQLParser();
  ~SQLParser();

  vector<Record*> execute_query(const string& query);
};

#endif // SQLPARSER_H
