#ifndef SQLPARSER_H
#define SQLPARSER_H

#include "Record.hpp"
#include <string>
#include <vector>
#include "AvlFile/AvlFileA.hpp"
#include "AvlFile/AvlFileB.hpp"

using namespace std;

enum IndexType {
    AVL_A,
    AVL_B,
    EXT_A,
    EXT_B,
    SEQ_A,
    SEQ_B,
};

class SQLParser {
private:
  vector<Record> select_query(const string &query);
  void create_table(const string &query);
  void insert_query(const string &query);
  void update_query(const string &query);
  void delete_query(const string &query);

  vector<Record> records;
  string filename;
  string table_name;
  bool table_created = false;

  IndexType index_type;

public:
  SQLParser();
  ~SQLParser();

  vector<Record> execute_query(const string& query);
};

#endif // SQLPARSER_H
