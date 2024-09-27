#ifndef EXTENDIBLERB_HPP
#define EXTENDIBLERB_HPP

#include <cstring>
#include <iostream>
#include "../Record.hpp"

struct EHRecordB {
    int year;
    char make[30];
    char model[50];
    char vin[18];

    EHRecordB() = default;
    ~EHRecordB() = default;

    EHRecordB(int year, const char* make, const char* model, const char* vin) {
        this->year = year;
        strncpy(this->make, make ? make : "", sizeof(this->make) - 1);
        this->make[sizeof(this->make) - 1] = '\0';
        strncpy(this->model, model ? model : "", sizeof(this->model) - 1);
        this->model[sizeof(this->model) - 1] = '\0';
        strncpy(this->vin, vin ? vin : "", sizeof(this->vin) - 1);
        this->vin[sizeof(this->vin) - 1] = '\0';
    }

    // Nuevo constructor que acepta RecordB
    EHRecordB(const RecordB& record) {
        this->year = record.year;
        strncpy(this->make, record.make.c_str(), sizeof(this->make) - 1);
        this->make[sizeof(this->make) - 1] = '\0';
        strncpy(this->model, record.model.c_str(), sizeof(this->model) - 1);
        this->model[sizeof(this->model) - 1] = '\0';
        strncpy(this->vin, record.vin.c_str(), sizeof(this->vin) - 1);
        this->vin[sizeof(this->vin) - 1] = '\0';
    }

    RecordB to_record() const {
        RecordB record;
        record.vin = std::string(vin, 18);  // Aquí también convertimos char[] a std::string
        record.make = std::string(make, 30);
        record.model = std::string(model, 50);
        record.year = year;
        return record;
    }

    void Print() const {
        std::string sep = " | ";
        std::cout << sep << year << sep << make << sep << model << sep << vin << std::endl;
    }

    std::string key() const {
        return std::string(vin, 18);
    }
};

#endif // EXTENDIBLERB_HPP