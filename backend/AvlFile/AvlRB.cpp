#include "AvlRB.hpp"
#include <cstring>

AvlRB::AvlRB() {
  left = -1;
  right = -1;
  nextdel = -1;
  year = 0;
  memset(make, 0, sizeof(make));
  memset(model, 0, sizeof(model));
  memset(vin, 0, sizeof(vin));
};

AvlRB::AvlRB(const RecordB& record) {
  year = record.year;
  strncpy(this->make, record.make.c_str(), sizeof(make) - 1);
  make[sizeof(make) - 1] = '\0';
  strncpy(this->model, record.model.c_str(), sizeof(model) - 1);
  model[sizeof(model) - 1] = '\0';
  strncpy(this->vin, record.vin.c_str(), sizeof(vin) - 1);
  vin[sizeof(vin) - 1] = '\0';
};

RecordB AvlRB::to_record() const {
  RecordB record;
  record.year = year;
  record.make = make;
  record.model = model;
  record.vin = vin;
  record.empty = false;
  return record;
};

void AvlRB::Print() {
  printf("year: %d\n", year);
  printf("make: %s\n", make);
  printf("model: %s\n", model);
  printf("vin: %s\n", vin);
  printf("left: %d\n", left);
  printf("right: %d\n", right);
  printf("nextdel: %d\n", nextdel);
};

std::string AvlRB::key() {
  return std::string(vin);
};
