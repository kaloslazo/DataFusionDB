#include "AvlRecordB.hpp"

AvlRecordB::AvlRecordB() {
  left = -1;
  right = -1;
  nextdel = -1;
  year = 0;
  memset(make, 0, sizeof(make));
  memset(model, 0, sizeof(model));
  memset(vin, 0, sizeof(vin));
}

AvlRecordB::AvlRecordB(const RecordB& record) {
  year = record.year;
  strncpy(make, record.make.c_str(), sizeof(make) - 1);
  make[sizeof(make) - 1] = '\0';
  strncpy(model, record.model.c_str(), sizeof(model) - 1);
  model[sizeof(model) - 1] = '\0';
  strncpy(vin, record.vin.c_str(), sizeof(vin) - 1);
  vin[sizeof(vin) - 1] = '\0';
}

RecordB AvlRecordB::to_record() {
  RecordB record;
  record.year = year;
  record.make = string(make, 30);
  record.model = string(model, 50);
  record.vin = string(vin, 18);
  record.empty = false;
  return record;
}

void AvlRecordB::Print() {
  // Implementa la función Print
}

std::string AvlRecordB::key() {
  // Implementa la función key
  return std::string(vin);
}
