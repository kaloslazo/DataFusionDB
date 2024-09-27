#ifndef RECORD_HPP
#define RECORD_HPP

#include <iostream>
#include <string>

using namespace std;

// Forward declarations
struct AvlRecordA;
struct AvlRecordB;

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

  RecordA() = default;
  RecordA(const AvlRecordA& avl_record);
  RecordA(const string& id, const string& name, const string& album, const string& album_id, const string& artists)
  : id(id), name(name), album(album), album_id(album_id), artists(artists) {
    empty = false;
  }

  string key() override { return id; }
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

  RecordB() = default;
  RecordB(const AvlRecordB& avl_record);
  RecordB(int year, const string& make, const string& model, const string& vin)
  : year(year), make(make), model(model), vin(vin) {
    empty = false;
  }

  string key() override { return vin; }
  void Print() override {
    cout << "Year: " << year << endl;
    cout << "Make: " << make << endl;
    cout << "Model: " << model << endl;
    cout << "VIN: " << vin << endl;
  }
};

#endif // RECORD_HPP