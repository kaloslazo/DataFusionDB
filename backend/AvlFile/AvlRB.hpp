#ifndef AVLRB_HPP
#define AVLRB_HPP

#include "../Record.hpp"
using namespace std;

struct AvlRB {
  int year;
  char make[30];
  char model[50];
  char vin[18];
  int left = -1;
  int right = -1;
  int nextdel = -1;

  RecordB to_record() const;
  void Print();
  std::string key();

  AvlRB();
  AvlRB(const RecordB& record);
};

#endif // AVLRB_HPP
