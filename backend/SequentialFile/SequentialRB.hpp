#pragma once
#include <cstring>
#include <iostream>
#include "../Record.hpp"

struct SequentialRB {
  int year;
  char make[30];
  char model[50];
  char vin[18];

  SequentialRB() = default;

  inline SequentialRB(const RecordB& record) {
    year = record.year;
    strncpy(make, record.make.c_str(), sizeof(make) - 1);
    make[sizeof(make) - 1] = '\0';
    strncpy(model, record.model.c_str(), sizeof(model) - 1);
    model[sizeof(model) - 1] = '\0';
    strncpy(vin, record.vin.c_str(), sizeof(vin) - 1);
    vin[sizeof(vin) - 1] = '\0';
  }

  inline void Print() const {
    std::cout << "Year: " << year << std::endl;
    std::cout << "Make: " << make << std::endl;
    std::cout << "Model: " << model << std::endl;
    std::cout << "VIN: " << vin << std::endl;
  }

  inline std::string key() const { 
    return std::string(vin, 18); 
  }

  inline RecordB to_record() const {
      RecordB record;
      record.year = year;
      record.make = std::string(make);
      record.model = std::string(model);
      record.vin = std::string(vin);
      record.empty = false;
      return record;
  }
};
