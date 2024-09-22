#ifndef RECORDB_HPP
#define RECORDB_HPP

#include <iostream>

using namespace std;

struct Record {
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

    string key() {
        return string(vin, 18);
    }
};

#endif  // RECORDB_HPP
