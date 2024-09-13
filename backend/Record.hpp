#ifndef RECORD_H
#define RECORD_H

#include <string>
using namespace std;

struct Record {
  string name;
  string id;
  
  Record();
  Record(string name, string id);
  ~Record();
};

#endif // RECORD_H
