#ifndef AVLRA_HPP
#define AVLRA_HPP

#include "../Record.hpp"
using namespace std;

struct AvlRA {
  char id[23];
  char name[521];
  char album[244];
  char album_id[23];
  char artists[1124];
  int left = -1;
  int right = -1;
  int nextdel = -1;

  RecordA to_record() const;
  void Print();
  std::string key();

  AvlRA();
  AvlRA(const RecordA& record);
};

#endif // AVLRA_HPP
