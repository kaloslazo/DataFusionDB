#include "Record.hpp"
#include <cstring>
#include <iomanip>
#include <iostream>

using namespace std;

Record::Record() {
  key = -1;
  name[0] = '\0';
  age = -1;
  term = -1;
};

Record::Record(int _key, const char* _name, int _age, int _term) {
  key = _key;
  strncpy(name, _name, sizeof(name) - 1);
  name[sizeof(name) - 1] = '\0';
  age = _age;
  term = _term;
};

void Record::Print() {
  string sep = " | ";

  //one line with separator and setw
  cout << sep << setw(5) << key << sep << setw(10) << name << sep << setw(4) << age << sep << setw(4) << term << endl;
};
