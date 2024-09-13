#ifndef SQLPARSER_H
#define SQLPARSER_H

#include "Record.hpp"
#include <string>
#include <vector>

using namespace std;

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
public:
  SQLParser();
  ~SQLParser();

  vector<Record> execute_query(const string& query);
};

#endif // SQLPARSER_H
