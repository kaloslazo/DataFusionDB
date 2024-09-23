#ifndef RECORD_HPP
#define RECORD_HPP

#include <string>
using namespace std;

class Record {
   public:
    bool empty = true;
    virtual ~Record() = default;
    virtual string key() = 0;
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
};

#endif  // RECORD_HPP
