#include "SQLParser.hpp"
#include <algorithm>
#include <cstring>
#include <regex>
#include <sstream>
#include "ExtendibleHashingFile/ExtendibleRA.hpp"

// Modificar la función execute_query para incluir CREATE INDEX
vector<Record*> SQLParser::execute_query(const string& query) {
  vector<Record*> result;
  string lower_query = query;
  transform(lower_query.begin(), lower_query.end(), lower_query.begin(),
            ::tolower);

  if (lower_query.find("select") == 0) {
    result = select_query(query);
  } else if (lower_query.find("create table") == 0) {
    create_table(query);
  } else if (lower_query.find("create index") == 0) {
    create_index(query);
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

std::optional<Record*> SQLParser::search_id(const std::string& id) {
  if (index_type == INDEX_AVL) {
    if (record_type == TYPE_RECORD_A) {
      RecordA record = avl_a->find(id);
      if (record.id[0] !=
          '\0') {  // Asumiendo que un id vacío significa que no se encontró
        return new RecordA(record);
      }
    } else if (record_type == TYPE_RECORD_B) {
      RecordB record = avl_b->find(id);
      if (record.vin[0] !=
          '\0') {  // Asumiendo que un vin vacío significa que no se encontró
        return new RecordB(record);
      }
    }
  } else if (index_type == INDEX_SEQUENTIAL) {
    if (record_type == TYPE_RECORD_A) {
      SequentialRA record = seq_a->search(id);
      if (record.id[0] != '\0') {
        return new RecordA(record.to_record());
      }
    } else if (record_type == TYPE_RECORD_B) {
      SequentialRB record = seq_b->search(id);
      if (record.vin[0] != '\0') {
        return new RecordB(record.to_record());
      }
    }
  } else if (index_type == INDEX_HASH) {
    if (record_type == TYPE_RECORD_A) {
      vector<EHRecordA> records = eh_a->search(id);
      if (!records.empty()) {
        return new RecordA(records[0].to_record());
      }
    } else if (record_type == TYPE_RECORD_B) {
      vector<EHRecordB> records = eh_b->search(id);
      if (!records.empty()) {
        return new RecordB(records[0].to_record());
      }
    }
  }
  return std::nullopt;
}

// Helper function to compare values
bool SQLParser::compare_values(const string& record_value,
                               const string& op,
                               const string& query_value) {
  if (op == "=")
    return record_value == query_value;
  if (op == ">")
    return record_value > query_value;
  if (op == "<")
    return record_value < query_value;
  if (op == ">=")
    return record_value >= query_value;
  if (op == "<=")
    return record_value <= query_value;
  return false;
}

// Función para SELECT
vector<Record*> SQLParser::select_query(const string& query) {
  vector<Record*> result;
  if (!table_created) {
    cerr << "Error: Table not created" << endl;
    return result;
  }

  regex select_regex(
      R"(select\s+\*\s+from\s+(\w+)(?:\s+where\s+(\w+)\s*(=|>=|<=|>|<)\s*('[^']*'|\d+)(?:\s+(and)\s+(\w+)\s*(=|>=|<=|>|<)\s*('[^']*'|\d+))?)?)",
      regex::icase);
  smatch match;

  if (regex_match(query, match, select_regex)) {
    string table = match[1];
    string column1 = match[2];
    string op1 = match[3];
    string value1 = match[4];
    string logical_op = match[5];
    string column2 = match[6];
    string op2 = match[7];
    string value2 = match[8];

    // Remove single quotes if they exist
    auto remove_quotes = [](string& val) {
      if (val.front() == '\'' && val.back() == '\'') {
        val = val.substr(1, val.length() - 2);
      }
    };

    remove_quotes(value1);
    remove_quotes(value2);

    if (column1.empty()) {  // No WHERE clause
      if (index_type == INDEX_AVL) {
        if (record_type == TYPE_RECORD_A) {
          vector<AvlRA> all_records = avl_a->inorder();
          for (const auto& rec : all_records) {
            result.push_back(new RecordA(rec.to_record()));
          }
        } else if (record_type == TYPE_RECORD_B) {
          vector<AvlRB> all_records = avl_b->inorder();
          for (const auto& rec : all_records) {
            result.push_back(new RecordB(rec.to_record()));
          }
        }
      } else if (index_type == INDEX_SEQUENTIAL) {
        if (record_type == TYPE_RECORD_A) {
          vector<SequentialRA> all_records = seq_a->range_search("", "~");
          for (const auto& rec : all_records) {
            result.push_back(new RecordA(rec.to_record()));
          }
        } else if (record_type == TYPE_RECORD_B) {
          vector<SequentialRB> all_records = seq_b->range_search("", "~");
          for (const auto& rec : all_records) {
            result.push_back(new RecordB(rec.to_record()));
          }
        }
      } else if (index_type == INDEX_HASH) {
        cerr << "Error: Full table scan not supported for Hash indexes" << endl;
      }
    } else {  // WHERE clause
      if (index_type == INDEX_AVL || index_type == INDEX_SEQUENTIAL) {
        if (column1 == "id" ||
            column1 ==
                "vin") {  // Assuming 'id' for RecordA and 'vin' for RecordB
          vector<Record*> all_records;
          if (index_type == INDEX_AVL) {
            if (record_type == TYPE_RECORD_A) {
              vector<AvlRA> avl_records = avl_a->inorder();
              for (const auto& rec : avl_records) {
                all_records.push_back(new RecordA(rec.to_record()));
              }
            } else if (record_type == TYPE_RECORD_B) {
              vector<AvlRB> avl_records = avl_b->inorder();
              for (const auto& rec : avl_records) {
                all_records.push_back(new RecordB(rec.to_record()));
              }
            }
          } else if (index_type == INDEX_SEQUENTIAL) {
            if (record_type == TYPE_RECORD_A) {
              vector<SequentialRA> seq_records =
                  seq_a->range_search(value1, value2);
              for (const auto& rec : seq_records) {
                all_records.push_back(new RecordA(rec.to_record()));
              }
            } else if (record_type == TYPE_RECORD_B) {
              vector<SequentialRB> seq_records =
                  seq_b->range_search(value1, value2);
              for (const auto& rec : seq_records) {
                all_records.push_back(new RecordB(rec.to_record()));
              }
            }
          }

          // Apply the conditions
          for (const auto& rec : all_records) {
            bool condition_met = true;
            if (record_type == TYPE_RECORD_A) {
              RecordA* record_a = static_cast<RecordA*>(rec);
              condition_met = compare_values(record_a->id, op1, value1);
              if (logical_op == "and" && condition_met) {
                condition_met = compare_values(record_a->id, op2, value2);
              }
            } else if (record_type == TYPE_RECORD_B) {
              RecordB* record_b = static_cast<RecordB*>(rec);
              condition_met = compare_values(record_b->vin, op1, value1);
              if (logical_op == "and" && condition_met) {
                condition_met = compare_values(record_b->vin, op2, value2);
              }
            }
            if (condition_met) {
              result.push_back(rec);
            } else {
              delete rec;
            }
          }
        } else {
          // Linear search for other columns
          // ... (existing code for linear search)
        }
      } else if (index_type == INDEX_HASH) {
        if (column1 == "id" ||
            column1 ==
                "vin") {  // Assuming 'id' for RecordA and 'vin' for RecordB
          if (record_type == TYPE_RECORD_A) {
            vector<EHRecordA> records = eh_a->search(value1);
            if (!records.empty()) {
              result.push_back(new RecordA(records[0].to_record()));
            }
          } else if (record_type == TYPE_RECORD_B) {
            vector<EHRecordB> records = eh_b->search(value1);
            if (!records.empty()) {
              result.push_back(new RecordB(records[0].to_record()));
            }
          }
        } else {
          cerr << "Error: Hash index only supports searching by primary key"
               << endl;
        }
      }
    }
  } else {
    cerr << "Error: Query syntax is not correct for SELECT" << endl;
  }
  return result;
}

// Función para CREATE TABLE
void SQLParser::create_table(const string& query) {
  regex create_regex(R"(create\s+table\s+(\w+)\s+from\s+file\s+"([^"]+)\")",
                     regex::icase);
  smatch match;

  if (regex_match(query, match, create_regex)) {
    table_name = match[1];
    filename = match[2];

    ifstream file(filename);
    if (!file.is_open()) {
      cerr << "Error: File not found" << endl;
      return;
    }

    string header_csv;
    getline(file, header_csv);
    set_record_type(header_csv);

    // Inicializar la estructura de índice correspondiente
    if (index_type == INDEX_AVL) {
      if (record_type == TYPE_RECORD_A) {
        avl_a = new AVLFileRA<string>(filename);
      } else if (record_type == TYPE_RECORD_B) {
        avl_b = new AVLFileRB<string>(filename);
      }
    } else if (index_type == INDEX_SEQUENTIAL) {
      if (record_type == TYPE_RECORD_A) {
        seq_a = new SequentialFileA(filename, filename + ".aux",
                                    sizeof(SequentialRA));
      } else if (record_type == TYPE_RECORD_B) {
        seq_b = new SequentialFileB(filename, filename + ".aux",
                                    sizeof(SequentialRB));
      }
    } else if (index_type == INDEX_HASH) {
      if (record_type == TYPE_RECORD_A) {
        eh_a = new ExtendibleHashing<EHRecordA, string>(filename);
        eh_a->loadCSV(filename);
      } else if (record_type == TYPE_RECORD_B) {
        eh_b = new ExtendibleHashing<EHRecordB, string>(filename);
        eh_b->loadCSV(filename);
      }
    }

    table_created = true;
    cout << "OK. Table created" << endl;
  } else {
    cerr << "Error: Query syntax is not correct for CREATE TABLE" << endl;
  }
}

// Función para INSERT
void SQLParser::insert_query(const string& query) {
  if (!table_created) {
    cerr << "Error: Table not created" << endl;
    return;
  }

  regex insert_regex(R"(insert\s+into\s+(\w+)\s+values\s*\(([^)]+)\))",
                     regex::icase);
  smatch match;

  if (regex_match(query, match, insert_regex)) {
    string values = match[2];
    vector<string> value_list;
    stringstream ss(values);
    string item;

    while (getline(ss, item, ',')) {
      item.erase(remove(item.begin(), item.end(), '\''), item.end());
      item.erase(remove(item.begin(), item.end(), ' '), item.end());
      value_list.push_back(item);
    }

    Record* new_record = create_record(value_list);
    if (index_type == INDEX_AVL) {
      if (record_type == TYPE_RECORD_A) {
        avl_a->insert(*static_cast<RecordA*>(new_record));
      } else if (record_type == TYPE_RECORD_B) {
        avl_b->insert(*static_cast<RecordB*>(new_record));
      }
    } else if (index_type == INDEX_SEQUENTIAL) {
      if (record_type == TYPE_RECORD_A) {
        seq_a->insert(*static_cast<RecordA*>(new_record));
      } else if (record_type == TYPE_RECORD_B) {
        seq_b->insert(*static_cast<RecordB*>(new_record));
      }
    } else if (index_type == INDEX_HASH) {
      if (record_type == TYPE_RECORD_A) {
        eh_a->add(*static_cast<EHRecordA*>(new_record));
      } else if (record_type == TYPE_RECORD_B) {
        eh_b->add(*static_cast<EHRecordB*>(new_record));
      }
    }

    delete new_record;
    cout << "OK. Record inserted" << endl;
  } else {
    cerr << "Error: Invalid INSERT query syntax" << endl;
  }
}

// Función para DELETE
void SQLParser::delete_query(const string& query) {
  if (!table_created) {
    cerr << "Error: Table not created" << endl;
    return;
  }

  regex delete_regex(R"(delete\s+from\s+(\w+)\s+where\s+(\w+)\s*=\s*(.+))",
                     regex::icase);
  smatch match;

  if (regex_match(query, match, delete_regex)) {
    string where_value = match[3];
    where_value.erase(remove(where_value.begin(), where_value.end(), '\''),
                      where_value.end());

    bool success = false;
    if (index_type == INDEX_AVL) {
      if (record_type == TYPE_RECORD_A) {
        success = avl_a->remove(where_value);
      } else {
        success = avl_b->remove(where_value);
      }
    } else if (index_type == INDEX_SEQUENTIAL) {
      if (record_type == TYPE_RECORD_A) {
        seq_a->remove_record(where_value);
      } else {
        seq_b->remove_record(where_value);
      }
      success = true;
    } else if (index_type == INDEX_HASH) {
      if (record_type == TYPE_RECORD_A) {
        success = eh_a->remove(where_value);
      } else if (record_type == TYPE_RECORD_B) {
        success = eh_a->remove(where_value);
      }
    }

    if (success) {
      cout << "Record deleted successfully" << endl;
    } else {
      cerr << "Error: Failed to delete record or record not found" << endl;
    }
  } else {
    cerr << "Error: Invalid DELETE query syntax" << endl;
  }
}

void SQLParser::update_query(const string& query) {
  cerr << "Error: UPDATE query not implemented" << endl;
}

// Función para crear un nuevo record
Record* SQLParser::create_record(const vector<string>& values) {
  if (record_type == TYPE_RECORD_A) {
    if (values.size() != 5) {
      cerr << "Error: Invalid number of values for RecordA" << endl;
      return nullptr;
    }
    if (index_type == INDEX_HASH) {
      return new EHRecordA(values[0].c_str(), values[1].c_str(),
                           values[2].c_str(), values[3].c_str(),
                           values[4].c_str());
    } else {
      return new RecordA(values[0], values[1], values[2], values[3], values[4]);
    }
  } else if (record_type == TYPE_RECORD_B) {
    if (values.size() != 4) {
      cerr << "Error: Invalid number of values for RecordB" << endl;
      return nullptr;
    }
    if (index_type == INDEX_HASH) {
      return new EHRecordB(stoi(values[0]), values[1].c_str(),
                           values[2].c_str(), values[3].c_str());
    } else {
      return new RecordB(stoi(values[0]), values[1], values[2], values[3]);
    }
  }
  return nullptr;
}

// Función para setear el tipo de registro
void SQLParser::set_record_type(const string& header) {
  if (header.find("id,name,album,album_id,artists") != string::npos) {
    record_type = TYPE_RECORD_A;
    cout << "Record type set to A" << endl;
  } else if (header.find("year,make,model,vin") != string::npos) {
    record_type = TYPE_RECORD_B;
    cout << "Record type set to B" << endl;
  } else {
    throw runtime_error("Error: Record type not recognized");
  }
}

void SQLParser::create_index(const string& query) {
  regex index_regex(R"(CREATE\s+INDEX\s+(\w+)\s+ON\s+(\w+)\s+USING\s+(\w+))",
                    regex::icase);
  smatch match;

  if (regex_match(query, match, index_regex)) {
    string index_name = match[1];
    string table_name = match[2];
    string index_type_str = match[3];

    if (table_name != this->table_name) {
      cerr << "Error: Table " << table_name << " does not exist." << endl;
      return;
    }

    if (index_type_str == "AVL") {
      index_type = INDEX_AVL;
    } else if (index_type_str == "SEQUENTIAL") {
      index_type = INDEX_SEQUENTIAL;
    } else if (index_type_str == "HASH") {
      index_type = INDEX_HASH;
    } else {
      cerr << "Error: Unsupported index type " << index_type_str << endl;
      return;
    }

    // Crear el índice correspondiente
    create_index_structure();

    cout << "Index " << index_name << " created on " << table_name << " using "
         << index_type_str << endl;
  } else {
    cerr << "Error: Invalid CREATE INDEX syntax" << endl;
  }
}

// Añadir esta nueva función miembro a la clase SQLParser
void SQLParser::create_index_structure() {
  if (index_type == INDEX_AVL) {
    if (record_type == TYPE_RECORD_A) {
      avl_a = new AVLFileRA<string>(filename);
    } else if (record_type == TYPE_RECORD_B) {
      avl_b = new AVLFileRB<string>(filename);
    }
  } else if (index_type == INDEX_SEQUENTIAL) {
    if (record_type == TYPE_RECORD_A) {
      seq_a = new SequentialFileA(filename, filename + ".aux",
                                  sizeof(SequentialRA));
    } else if (record_type == TYPE_RECORD_B) {
      seq_b = new SequentialFileB(filename, filename + ".aux",
                                  sizeof(SequentialRB));
    }
  } else if (index_type == INDEX_HASH) {
    if (record_type == TYPE_RECORD_A) {
      eh_a = new ExtendibleHashing<EHRecordA, string>(filename);
      eh_a->loadCSV(filename);
    } else if (record_type == TYPE_RECORD_B) {
      eh_b = new ExtendibleHashing<EHRecordB, string>(filename);
      eh_b->loadCSV(filename);
    }
  }
}

// Constructor y destructor
SQLParser::SQLParser()
    : index_type(INDEX_UNKNOWN),
      record_type(TYPE_RECORD_A),
      avl_a(nullptr),
      avl_b(nullptr),
      seq_a(nullptr),
      seq_b(nullptr),
      eh_a(nullptr),
      eh_b(nullptr) {}

SQLParser::~SQLParser() {
  delete avl_a;
  delete avl_b;
  delete seq_a;
  delete seq_b;
  delete eh_a;
  delete eh_b;
}
