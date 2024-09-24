<<<<<<< HEAD
#include "SQLParser.hpp"
=======
>>>>>>> 4eaea94 (feat: gui with autodetect record)
#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <vector>
<<<<<<< HEAD
=======
#include "SQLParser.hpp"
>>>>>>> 4eaea94 (feat: gui with autodetect record)
#include "AvlRecordA.hpp"

using namespace std;

vector<Record*> SQLParser::search_range(const string& id1, const string& id2) {
  vector<Record*> result;
  if (record_type == TYPE_RECORD_A) {
    char id1_arr[23], id2_arr[23];
    strncpy(id1_arr, id1.c_str(), sizeof(id1_arr));
    strncpy(id2_arr, id2.c_str(), sizeof(id2_arr));
    vector<RecordA> found = avl_a->range_search(id1_arr, id2_arr);
    for (const auto& rec : found) {
      result.push_back(new RecordA(rec));
    }
  } else if (record_type == TYPE_RECORD_B) {
    char id1_arr[18], id2_arr[18];
    strncpy(id1_arr, id1.c_str(), sizeof(id1_arr));
    strncpy(id2_arr, id2.c_str(), sizeof(id2_arr));
    vector<RecordB> found = avl_b->range_search(id1_arr, id2_arr);
    for (const auto& rec : found) {
      result.push_back(new RecordB(rec));
    }
  }
  return result;
}

Record* SQLParser::search_id(const string& id) {
  if (record_type == TYPE_RECORD_A) {
    char id_arr[23];
    strncpy(id_arr, id.c_str(), sizeof(id_arr));
<<<<<<< HEAD
    id_arr[sizeof(id_arr) - 1] = '\0';
    // cout << "id_arr: " << id_arr << endl;
    AvlRecordA found = avl_a->find(id_arr);
=======
    RecordA found = avl_a->find(id_arr);
>>>>>>> 4eaea94 (feat: gui with autodetect record)
    return new RecordA(found);
  } else if (record_type == TYPE_RECORD_B) {
    char id_arr[18];
    strncpy(id_arr, id.c_str(), sizeof(id_arr));
<<<<<<< HEAD
    AvlRecordB found = avl_b->find(id_arr);
=======
    RecordB found = avl_b->find(id_arr);
>>>>>>> 4eaea94 (feat: gui with autodetect record)
    return new RecordB(found);
  }
  return nullptr;
}

// constructor and destructor
SQLParser::SQLParser() {
  table_created = false;
};

SQLParser::~SQLParser() {
  delete avl_a;
  delete avl_b;
<<<<<<< HEAD
  for (auto record : records)
    delete record;
};

// handle the query, execution uses internal functions
// handle selection with considering ranges

vector<Record*> SQLParser::execute_query(const string& query) {
  vector<Record*> result;
  string lower_query = query;
  transform(lower_query.begin(), lower_query.end(), lower_query.begin(),
            ::tolower);

  if (lower_query.find("select") == 0) {
    result = select_query(query);
  } else if (lower_query.find("create table") == 0) {
    create_table(query);
  } else if (lower_query.find("insert into") == 0) {
    insert_query(query);
  } else if (lower_query.find("update") == 0) {
    update_query(query);
  } else if (lower_query.find("delete from") == 0) {
    delete_query(query);
  } else {
    cerr << "Error: Unsupported query type" << endl;
  }

  return result;
}

vector<Record*> SQLParser::select_query(const string& query) {
  vector<Record*> result;
  if (!table_created) {
    cerr << "Error: Table not created" << endl;
    return result;
  }
  regex select_regex(R"(select\s+\*\s+from\s+(\w+)(?:\s+where\s+(.+))?)",
                     regex::icase);
  smatch match;
  if (regex_match(query, match, select_regex)) {
    string table_name = match[1];
    string condition_where = match[2];
    if (condition_where.empty()) {
=======
  for (auto record : records) delete record;
};

// handle the query, execution uses internal functions
// handle selection with considering ranges

vector<Record*> SQLParser::execute_query(const string& query) {
  vector<Record*> result;
  string lower_query = query;
  transform(lower_query.begin(), lower_query.end(), lower_query.begin(), ::tolower);

  if (lower_query.find("select") == 0) {
    result = select_query(query);
  } else if (lower_query.find("create table") == 0) {
    create_table(query);
  } else if (lower_query.find("insert into") == 0) {
    insert_query(query);
  } else if (lower_query.find("update") == 0) {
    update_query(query);
  } else if (lower_query.find("delete from") == 0) {
    delete_query(query);
  } else {
    cerr << "Error: Unsupported query type" << endl;
  }

  return result;
}

vector<Record*> SQLParser::select_query(const string &query) {
  vector<Record*> result;
  if(!table_created) {
    cerr << "Error: Table not created" << endl;
    return result;
  }
  regex select_regex(R"(select\s+\*\s+from\s+(\w+)(?:\s+where\s+(.+))?)", regex::icase);
  smatch match;
  if(regex_match(query, match, select_regex)) {
    string table_name = match[1];
    string condition_where = match[2];
    if(condition_where.empty()) {
>>>>>>> 4eaea94 (feat: gui with autodetect record)
      if (record_type == TYPE_RECORD_A) {
        vector<AvlRecordA> all_records = avl_a->inorder();
        for (const auto& rec : all_records) {
          result.push_back(new RecordA(rec));  // Cambiado aquí
        }
      } else if (record_type == TYPE_RECORD_B) {
        vector<AvlRecordB> all_records = avl_b->inorder();
        for (const auto& rec : all_records) {
          result.push_back(new RecordB(rec));  // Cambiado aquí
        }
      }
    } else {
<<<<<<< HEAD
      if (record_type == TYPE_RECORD_A) {
        // usar solo lo que esta entre comillas
        size_t posIgual = condition_where.find("=");
        string valor = condition_where.substr(posIgual + 1);
        valor.erase(0, valor.find_first_not_of(' '));
        valor.erase(0, 1);
        valor.erase(valor.size() - 1);
        // cout << "valor: " << valor << endl;
        Record* record = search_id(valor);
        result.push_back(record);
        // record->Print();
      } else if (record_type == TYPE_RECORD_B) {
        // usar solo lo que esta entre comillas
        size_t posIgual = condition_where.find("=");
        string valor = condition_where.substr(posIgual + 1);
        valor.erase(0, valor.find_first_not_of(' '));
        valor.erase(0, 1);
        valor.erase(valor.size() - 1);
        // cout << "valor: " << valor << endl;
        Record* record = search_id(valor);
        result.push_back(record);
        // record->Print();
      }
=======
      // ... (resto del código sin cambios)
>>>>>>> 4eaea94 (feat: gui with autodetect record)
    }
  } else {
    cerr << "Error: Query syntax is not correct for SELECT" << endl;
  }
  return result;
}

// create the table means use the desired structure and initialize
void SQLParser::create_table(const string& query) {
  // regex to find the parameters
  regex create_regex(R"(create\s+table\s+(\w+)\s+from\s+file\s+"([^"]+)\")",
                     regex::icase);
  smatch match;

  if (regex_match(query, match, create_regex)) {
    table_name = match[1];
    filename = match[2];

    // read csv for handle the header and set the record type
    ifstream file(filename);
    if (!file.is_open()) {
      cerr << "Error. File not found" << endl;
      return;
    };

    string header_csv;
    getline(file, header_csv);
<<<<<<< HEAD
    file.close();
=======
>>>>>>> 4eaea94 (feat: gui with autodetect record)
    set_record_type(header_csv);

    if (record_type == TYPE_RECORD_A) {
      avl_a = new AVLFileA<char[23]>(filename);

    } else if (record_type == TYPE_RECORD_B) {
      avl_b = new AVLFileB<char[18]>(filename);
<<<<<<< HEAD
    };
=======
    }; 
>>>>>>> 4eaea94 (feat: gui with autodetect record)

    table_created = true;
    cout << "OK. Table created" << endl;

  } else {
    cerr << "Error. Query syntaxis is not correct for CREATE TABLE" << endl;
  };
};

// insert elements in the desired structure, replace with own !!
void SQLParser::insert_query(const string& query) {
  if (!table_created) {
    cerr << "Error. Table not created" << endl;
    return;
  };

  // regex to find the parameters
  regex insert_regex(R"(insert\s+into\s+(\w+)\s+values\s*\(([^)]+)\))",
                     regex::icase);
  smatch match;

  if (regex_match(query, match, insert_regex)) {
    string table_name = match[1];
    string values_delimited = match[2];

    // insert the values in the desired structure
    vector<string> values_split;
    stringstream ss(values_delimited);
    string item_record;

    // replace the function with the desired structure
    while (getline(ss, item_record, ',')) {
      item_record = regex_replace(item_record, regex("^ +| +$|( ) +"), "$1");
      values_split.push_back(item_record);
    };

    if ((record_type == TYPE_RECORD_A && values_split.size() != 5) ||
<<<<<<< HEAD
        (record_type == TYPE_RECORD_B && values_split.size() != 4)) {
      cerr << "Error. Incorrect number of values for INSERT" << endl;
      return;
    }

=======
      (record_type == TYPE_RECORD_B && values_split.size() != 4)) {
      cerr << "Error. Incorrect number of values for INSERT" << endl;
      return;
    }
    
>>>>>>> 4eaea94 (feat: gui with autodetect record)
    Record* record = create_record(values_split);
    if (record_type == TYPE_RECORD_A) {
      avl_a->insert(*static_cast<RecordA*>(record));

    } else if (record_type == TYPE_RECORD_B) {
      avl_b->insert(*static_cast<RecordB*>(record));
    };

    delete record;

    cout << "OK. Record inserted" << endl;
  } else {
    cerr << "Error. Query syntaxis is not correct for INSERT" << endl;
  };
};

// update the information with desired structure
<<<<<<< HEAD
void SQLParser::update_query(const string& query) {
=======
void SQLParser::update_query(const string &query) {
>>>>>>> 4eaea94 (feat: gui with autodetect record)
  cout << "falta conectar esto :(" << endl;
};

// delete the information with desired structure
<<<<<<< HEAD
void SQLParser::delete_query(const string& query) {
=======
void SQLParser::delete_query(const string &query) {
>>>>>>> 4eaea94 (feat: gui with autodetect record)
  cout << "falta conectar esto :(" << endl;
};

// create the record with the desired structure
Record* SQLParser::create_record(const vector<string>& values) {
  if (record_type == TYPE_RECORD_A) {
    if (values.size() != 5) {
      throw runtime_error("Invalid number of values for RecordA");
    }
    return new RecordA(values[0], values[1], values[2], values[3], values[4]);
  } else if (record_type == TYPE_RECORD_B) {
    if (values.size() != 4) {
      throw runtime_error("Invalid number of values for RecordB");
    }
    return new RecordB(stoi(values[0]), values[1], values[2], values[3]);
  } else {
    throw runtime_error("Unknown record type");
  }
}

<<<<<<< HEAD
=======

>>>>>>> 4eaea94 (feat: gui with autodetect record)
// set the record type with the header of the csv
void SQLParser::set_record_type(const string& header) {
  if (header.find("id,name,album,album_id,artists") != string::npos) {
    record_type = TYPE_RECORD_A;
    cout << "Record type set to A" << endl;

  } else if (header.find("year,make,model,vin") != string::npos) {
    record_type = TYPE_RECORD_B;
    cout << "Record type set to B" << endl;

  } else {
    throw runtime_error("Error. Record type not recognized");
  };
};
