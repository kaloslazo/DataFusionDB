#ifndef EXTENDIBLEHRECORDB_HPP
#define EXTENDIBLEHRECORDB_HPP

#include <cstring>
#include <iomanip>
#include <iostream>

struct EHRecordB {
  int year;
  char make[30];
  char model[50];
  char vin[18];

  EHRecordB() = default;
  ~EHRecordB() = default;
  EHRecordB(int year, const char* make, const char* model, const char* vin){
    year = year;
    strncpy(this->make, make ? make : "", sizeof(this->make) - 1);
    this->make[sizeof(this->make) - 1] = '\0';
    strncpy(this->model, model ? model : "", sizeof(this->model) - 1);
    this->model[sizeof(this->model) - 1] = '\0';
    strncpy(this->vin, vin ? vin : "", sizeof(this->vin) - 1);    
    this->vin[sizeof(this->vin) - 1] = '\0';  

  }

  void Print() {
    std::string sep = " | ";
    std::cout << sep << year << sep << make << sep << model << sep << vin
              << std::endl;
  }

  std::string key() { return std::string(vin, 18); }
};

#endif  // EXTENDIBLEHRECORDB_HPP