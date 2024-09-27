#ifndef EXTENDIBLEHRECORDB_HPP
#define EXTENDIBLEHRECORDB_HPP

#include <cstring>
#include <iomanip>
#include <iostream>

using namespace std;

struct EHRecordB {
  int year;
  char make[30];
  char model[50];
  char vin[18];

  EHRecordB() = default;
  ~EHRecordB() = default;
  EHRecordB(int year, char make[30], char model[50], char vin[18]);

  void Print() {
    std::string sep = " | ";
    std::cout << sep << year << sep << make << sep << model << sep << vin
              << std::endl;
  }

  std::string key() { return std::string(vin, 18); }
};

#endif  // EXTENDIBLEHRECORDB_HPP