#ifndef RECORDB_HPP
#define RECORDB_HPP

#include <cstring>
#include <iostream>
#include "../Record.hpp"

using namespace std;

struct AvlRecordB {
    int year;
    char make[30];
    char model[50];
    char vin[18];

    int left = -1;
    int right = -1;
    int nextdel = -1;

    void Print() {
        cout << "Year: " << year << endl;
        cout << "Make: " << make << endl;
        cout << "Model: " << model << endl;
        cout << "VIN: " << vin << endl;
    }

    string key() { return string(vin, 18); }

    RecordB to_record() {
        RecordB record;
        record.year = year;
        record.make = string(make, 30);
        record.model = string(model, 50);
        record.vin = string(vin, 18);
        record.empty = false;

        return record;
    }
    AvlRecordB() {}
    AvlRecordB(const RecordB& record) {
        year = record.year;
        strncpy(make, record.make.c_str(), sizeof(make));
        make[sizeof(make) - 1] = '\0';
        strncpy(model, record.model.c_str(), sizeof(model));
        model[sizeof(model) - 1] = '\0';
        strncpy(vin, record.vin.c_str(), sizeof(vin));
        vin[sizeof(vin) - 1] = '\0';
    }
};

#endif  // RECORDB_HPP
