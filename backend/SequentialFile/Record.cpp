#include "Record.hpp"

using namespace std;

Record::Record() {
  key = -1;
  name[0] = '\0';
  age = -1;
  term = -1;
}

Record::Record(int key, char name[12], int age, int term) {
  this->key = key;
  strncpy(this->name, name, sizeof(this->name) - 1);
  this->name[sizeof(this->name) - 1] = '\0';
  this->age = age;
  this->term = term;
}

Record::~Record() {}