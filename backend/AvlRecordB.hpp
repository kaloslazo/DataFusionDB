#ifndef AVLRECORDB_HPP
#define AVLRECORDB_HPP

#include <cstring>
#include "Record.hpp"

struct AvlRecordB {
  int year;
  char make[30];
  char model[50];
  char vin[18];
  int left = -1;
  int right = -1;
  int nextdel = -1;

  RecordB to_record();
  void Print();
  std::string key();

  AvlRecordB();
  AvlRecordB(const RecordB& record);
};

#endif // AVLRECORDB_HPP
