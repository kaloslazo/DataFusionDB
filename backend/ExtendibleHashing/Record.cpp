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

Record::Record(int _key, char _name[10], int _age, int _term) {
  key = _key;
  strncpy(name, _name, 10);
  age = _age;
  term = _term;
};

void Record::show_record() {
  cout << "Key: " << setw(4) << key << endl;
  cout << "Name: " << setw(10) << name << endl;
  cout << "Age: " << setw(4) << age << endl;
  cout << "Term: " << setw(4) << term << endl;
};
