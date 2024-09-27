#include "SQLParser.hpp"
#include <regex>
#include <sstream>
#include <algorithm>
#include <cstring>
#include <sstream>
#include <algorithm>


// Modificar la función execute_query para incluir CREATE INDEX
vector<Record*> SQLParser::execute_query(const string& query) {
  vector<Record*> result;
  string lower_query = query;
  transform(lower_query.begin(), lower_query.end(), lower_query.begin(), ::tolower);

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
            if (record.id[0] != '\0') {  // Asumiendo que un id vacío significa que no se encontró
                return new RecordA(record);
            }
        } else if (record_type == TYPE_RECORD_B) {
            RecordB record = avl_b->find(id);
            if (record.vin[0] != '\0') {  // Asumiendo que un vin vacío significa que no se encontró
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
            auto record = eh_a->Search(id);
            if (record) {
                return new RecordA(record->to_record());
            }
        } else if (record_type == TYPE_RECORD_B) {
            auto record = eh_b->Search(id);
            if (record) {
                return new RecordB(record->to_record());
            }
        }
    }
    return std::nullopt;
}

// Función para SELECT
vector<Record*> SQLParser::select_query(const string &query) {
    vector<Record*> result;
    if (!table_created) {
        cerr << "Error: Table not created" << endl;
        return result;
    }

    regex select_regex(R"(select\s+\*\s+from\s+(\w+)(?:\s+where\s+(\w+)\s*=\s*('[^']*'|\d+))?)", regex::icase);
    smatch match;

    if (regex_match(query, match, select_regex)) {
        string table = match[1];
        string column = match[2];
        string value = match[3];

        // Remover comillas simples si existen
        if (value.front() == '\'' && value.back() == '\'') {
            value = value.substr(1, value.length() - 2);
        }

        if (column.empty()) {  // No WHERE clause
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
                if (column == "id" || column == "vin") {  // Assuming 'id' for RecordA and 'vin' for RecordB
                    auto record = search_id(value);
                    if (record) {
                        result.push_back(record.value());
                    }
                } else {
                    // Linear search for other columns
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
                            vector<SequentialRA> seq_records = seq_a->range_search("", "~");
                            for (const auto& rec : seq_records) {
                                all_records.push_back(new RecordA(rec.to_record()));
                            }
                        } else if (record_type == TYPE_RECORD_B) {
                            vector<SequentialRB> seq_records = seq_b->range_search("", "~");
                            for (const auto& rec : seq_records) {
                                all_records.push_back(new RecordB(rec.to_record()));
                            }
                        }
                    }

                    for (const auto& rec : all_records) {
                        if (record_type == TYPE_RECORD_A) {
                            RecordA* record_a = static_cast<RecordA*>(rec);
                            if ((column == "name" && record_a->name == value) ||
                                (column == "album" && record_a->album == value) ||
                                (column == "album_id" && record_a->album_id == value) ||
                                (column == "artists" && record_a->artists == value)) {
                                result.push_back(rec);
                            } else {
                                delete rec;
                            }
                        } else if (record_type == TYPE_RECORD_B) {
                            RecordB* record_b = static_cast<RecordB*>(rec);
                            if ((column == "year" && to_string(record_b->year) == value) ||
                                (column == "make" && record_b->make == value) ||
                                (column == "model" && record_b->model == value)) {
                                result.push_back(rec);
                            } else {
                                delete rec;
                            }
                        }
                    }
                }
            } else if (index_type == INDEX_HASH) {
                if (column == "id" || column == "vin") {  // Assuming 'id' for RecordA and 'vin' for RecordB
                    if (record_type == TYPE_RECORD_A) {
                        auto record = eh_a->Search(value);
                        if (record) {
                            result.push_back(new RecordA(record->to_record()));
                        }
                    } else if (record_type == TYPE_RECORD_B) {
                        auto record = eh_b->Search(value);
                        if (record) {
                            result.push_back(new RecordB(record->to_record()));
                        }
                    }
                } else {
                    cerr << "Error: Hash index only supports searching by primary key" << endl;
                }
            }
        }
    } else {
        cerr << "Error: Query syntax is not correct for SELECT" << endl;
    }
    return result;
}

// Función para CREATE TABLE
void SQLParser::create_table(const string &query) {
  regex create_regex(R"(create\s+table\s+(\w+)\s+from\s+file\s+"([^"]+)\")", regex::icase);
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
        seq_a = new SequentialFileA(filename, filename + ".aux", sizeof(SequentialRA));
      } else if (record_type == TYPE_RECORD_B) {
        seq_b = new SequentialFileB(filename, filename + ".aux", sizeof(SequentialRB));
      }
    } else if (index_type == INDEX_HASH) {
      if (record_type == TYPE_RECORD_A) {
        eh_a = new ExtendibleHashing<EHRecordA, string>(3, 4); // Ajusta estos valores según sea necesario
        eh_a->Create(filename + ".data", filename + ".buckets");
        eh_a->Load_csv(filename);
      } else if (record_type == TYPE_RECORD_B) {
        eh_b = new ExtendibleHashing<EHRecordB, string>(3, 4); // Ajusta estos valores según sea necesario
        eh_b->Create(filename + ".data", filename + ".buckets");
        eh_b->Load_csv(filename);
      }
    }

    table_created = true;
    cout << "OK. Table created" << endl;
  } else {
    cerr << "Error: Query syntax is not correct for CREATE TABLE" << endl;
  }
}

// Función para INSERT
void SQLParser::insert_query(const string &query) {
  if (!table_created) {
    cerr << "Error: Table not created" << endl;
    return;
  }

  regex insert_regex(R"(insert\s+into\s+(\w+)\s+values\s*\(([^)]+)\))", regex::icase);
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
        eh_a->Insert(*static_cast<RecordA*>(new_record));
      } else if (record_type == TYPE_RECORD_B) {
        eh_b->Insert(*static_cast<RecordB*>(new_record));
      }
    }

    delete new_record;
    cout << "OK. Record inserted" << endl;
  } else {
    cerr << "Error: Invalid INSERT query syntax" << endl;
  }
}

// Función para DELETE
void SQLParser::delete_query(const string &query) {
  if (!table_created) {
    cerr << "Error: Table not created" << endl;
    return;
  }

  regex delete_regex(R"(delete\s+from\s+(\w+)\s+where\s+(\w+)\s*=\s*(.+))", regex::icase);
  smatch match;

  if (regex_match(query, match, delete_regex)) {
    string where_value = match[3];
    where_value.erase(remove(where_value.begin(), where_value.end(), '\''), where_value.end());

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
      // La implementación actual de ExtendibleHashing no tiene un método remove
      cerr << "Error: DELETE not implemented for Hash index" << endl;
      return;
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

// Función para UPDATE (incompleta, dependerá de cómo manejas la actualización)
void SQLParser::update_query(const string &query) {
  cerr << "Error: UPDATE query not implemented" << endl;
}

// Función para crear un nuevo record
Record* SQLParser::create_record(const vector<string>& values) {
  if (record_type == TYPE_RECORD_A) {
    if (values.size() != 5) {
      cerr << "Error: Invalid number of values for RecordA" << endl;
      return nullptr;
    }
    return new RecordA(values[0], values[1], values[2], values[3], values[4]);
  } else if (record_type == TYPE_RECORD_B) {
    if (values.size() != 4) {
      cerr << "Error: Invalid number of values for RecordB" << endl;
      return nullptr;
    }
    return new RecordB(stoi(values[0]), values[1], values[2], values[3]);
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

void SQLParser::create_index(const string &query) {
  regex index_regex(R"(CREATE\s+INDEX\s+(\w+)\s+ON\s+(\w+)\s+USING\s+(\w+))", regex::icase);
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

    cout << "Index " << index_name << " created on " << table_name << " using " << index_type_str << endl;
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
      seq_a = new SequentialFileA(filename, filename + ".aux", sizeof(SequentialRA));
    } else if (record_type == TYPE_RECORD_B) {
      seq_b = new SequentialFileB(filename, filename + ".aux", sizeof(SequentialRB));
    }
  } else if (index_type == INDEX_HASH) {
    if (record_type == TYPE_RECORD_A) {
      eh_a = new ExtendibleHashing<EHRecordA, string>(3, 4);
      eh_a->Create(filename + ".data", filename + ".buckets");
      eh_a->Load_csv(filename);
    } else if (record_type == TYPE_RECORD_B) {
      eh_b = new ExtendibleHashing<EHRecordB, string>(3, 4);
      eh_b->Create(filename + ".data", filename + ".buckets");
      eh_b->Load_csv(filename);
    }
  }
}

// Constructor y destructor
SQLParser::SQLParser() : index_type(INDEX_UNKNOWN), record_type(TYPE_RECORD_A), 
  avl_a(nullptr), avl_b(nullptr), seq_a(nullptr), seq_b(nullptr), 
  eh_a(nullptr), eh_b(nullptr) {}

SQLParser::~SQLParser() {
  delete avl_a;
  delete avl_b;
  delete seq_a;
  delete seq_b;
  delete eh_a;
  delete eh_b;
}
