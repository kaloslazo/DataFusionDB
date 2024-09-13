#include <iostream>
#include <regex>
#include <algorithm>
#include <string>
#include <vector>
#include "SQLParser.hpp"

using namespace std;


// START EXAMPLE ONLY - MUST REPLACE WITH REAL FUNCTIONS FOR DESIRED STRUCTURE
Record search_id(string id, vector<Record> records) {
  Record result;

  for (Record record : records) {
    if (stoi(record.id) == stoi(id)) result = record;
  };

  return result;
};
vector<Record> search_range(string id1, string id2, vector<Record> records) {
  vector<Record> result;

  for (Record record : records) {
    if (stoi(record.id) >= stoi(id1) && stoi(record.id) <= stoi(id2)) result.push_back(record);
  };

  return result;
};
// END EXAMPLE ONLY - MUST REPLACE WITH REAL FUNCTIONS FOR DESIRED STRUCTURE


// constructor and destructor
SQLParser::SQLParser() {
  table_created = false;
};

SQLParser::~SQLParser() {};

// handle the query, execution uses internal functions
vector<Record> SQLParser::execute_query(const string &query) {
  vector<Record> result;

  // query to minus case (easiest to handle)
  string query_lower = query;
  transform(query_lower.begin(), query_lower.end(), query_lower.begin(), ::tolower);

  // regex to find the desired query
  regex create_regex(R"(create\s+table\s+(\w+)\s+from\s+file\s+"([^"]+)\")", regex::icase);
  regex select_regex(R"(select\s+\*\s+from\s+(\w+)(?:\s+where\s+(.+))?)", regex::icase);
  regex insert_regex(R"(insert\s+into\s+(\w+)\s+values\s*\(([^)]+)\))", regex::icase);
  regex delete_regex(R"(delete\s+from\s+(\w+)\s+where\s+(.+))", regex::icase);
  regex update_regex(R"(update\s+(\w+)\s+set\s+(.+)\s+where\s+(.+))", regex::icase);
  smatch match;

  // match the query with the regex and execute the desired function
  if(regex_match(query_lower, match, create_regex)) create_table(query);
  else if(regex_match(query_lower, match, select_regex)) result = select_query(query);
  else if(regex_match(query_lower, match, insert_regex)) insert_query(query);
  else if(regex_match(query_lower, match, delete_regex)) delete_query(query);
  else if(regex_match(query_lower, match, update_regex)) update_query(query);
  else cerr << "Error. Query not recognized" << endl;

  return result;
};

// handle selection with considering ranges
vector<Record> SQLParser::select_query(const string &query) {
  vector<Record> result;

  if(!table_created) {
    cerr << "Error. Table not created" << endl;
    return result;
  };

  // regex to find the parameters
  regex select_regex(R"(select\s+\*\s+from\s+(\w+)(?:\s+where\s+(.+))?)", regex::icase);
  smatch match;

  if(regex_match(query, match, select_regex)) {
    string table_name = match[1];
    string condition_where = match[2];

    // if no where clause, return all records (SELECT * FROM TABLE)
    if(condition_where.empty()) {
      result = records;
    } else {
      // if where clause, return the records that match the condition, clean spaces
      condition_where = regex_replace(condition_where, regex("^ +| +$|( ) +"), "$1");

      // manage the case when the condition is an id or a range between two ids
      smatch match_condition;
      regex equal_regex(R"(id\s*=\s*(\d+))", regex::icase);
      regex between_regex(R"(id\s+between\s+(\d+)\s+and\s+(\d+))", regex::icase);

      // search the records that match the condition
      if(regex_match(condition_where, match_condition, equal_regex)) result.push_back(search_id(match_condition[1], records));
      else if(regex_match(condition_where, match_condition, between_regex)) result = search_range(match_condition[1], match_condition[2], records);
      else cerr << "Error. Query syntaxis is not correct for SELECT" << endl;
    };
  } else {
    cerr << "Error. Query syntaxis is not correct for SELECT" << endl;
  };

  return result;
};

// create the table means use the desired structure and initialize
void SQLParser::create_table(const string &query) {
  // regex to find the parameters
  regex create_regex(R"(create\s+table\s+(\w+)\s+from\s+file\s+"([^"]+)\")", regex::icase);
  smatch match;

  if (regex_match(query, match, create_regex)) {
    table_name = match[1];
    filename = match[2];

    // create the table with the desired structure
    records.clear();
    records.push_back(Record("Gatito Miau", "10"));
    records.push_back(Record("Hannah Montana", "20"));

    table_created = true;
    cout << "Table " << table_name << " created" << endl;
  } else {
    cerr << "Error. Query syntaxis is not correct for CREATE TABLE" << endl;
  };
};

// insert elements in the desired structure, replace with own !!
void SQLParser::insert_query(const string &query) {
  if (!table_created) {
    cerr << "Error. Table not created" << endl;
    return;
  };

  // regex to find the parameters
  regex insert_regex(R"(insert\s+into\s+(\w+)\s+values\s*\(([^)]+)\))", regex::icase);
  smatch match;

  if(regex_match(query, match, insert_regex)) {
    string table_name = match[1];
    string values_delimited = match[2];

    // insert the values in the desired structure
    vector<string> values_split;
    stringstream ss(values_delimited);
    string item_record;

    // replace the function with the desired structure
    while(getline(ss, item_record, ',')) {
      item_record = regex_replace(item_record, regex("^ +| +$|( ) +"), "$1");
      values_split.push_back(item_record);
    };

    if (values_split.size() != 2) {
      cerr << "Error. Incorrect number of values for INSERT" << endl;
      return;
    };
    
    string name = values_split[0];
    string id = string(values_split[1]);

    records.push_back(Record(name, id));
    cout << "OK. Record inserted" << endl;
  } else {
    cerr << "Error. Query syntaxis is not correct for INSERT" << endl;
  };
};

// update the information with desired structure
void SQLParser::update_query(const string &query) {

};

// delete the information with desired structure
void SQLParser::delete_query(const string &query) {

};
