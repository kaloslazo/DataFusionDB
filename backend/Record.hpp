#ifndef RECORD_HPP
#define RECORD_HPP

#include <iostream>
using namespace std;

class Record {
   public:
    bool empty = true;
    virtual ~Record() = default;
    virtual string key() = 0;
    virtual void Print() = 0;
};

class RecordA : public Record {
   public:
    string id;
    string name;
    string album;
    string album_id;
    string artists;

    string key() override {
        return id;
    }

    void Print() override {
        cout << "ID: " << id << endl;
        cout << "Name: " << name << endl;
        cout << "Album: " << album << endl;
        cout << "Album ID: " << album_id << endl;
        cout << "Artists: " << artists << endl;
    }
};

class RecordB : public Record {
   public:
    int year;
    string make;
    string model;
    string vin;

    string key() override {
        return vin;
    }

    void Print() override {
        cout << "Year: " << year << endl;
        cout << "Make: " << make << endl;
        cout << "Model: " << model << endl;
        cout << "VIN: " << vin << endl;
    }
};

#endif  // RECORD_HPP
