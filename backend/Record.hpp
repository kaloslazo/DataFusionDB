#ifndef RECORD_HPP
#define RECORD_HPP

#include <cstring>
#include <iostream>
#include <string>

using namespace std;

struct AvlRA;
struct AvlRB;

class Record {
 public:
  bool empty = true;
  virtual ~Record() = default;
  virtual string key() const = 0;
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
  RecordA(const AvlRA& avl_record);
  RecordA(const string& id,
          const string& name,
          const string& album,
          const string& album_id,
          const string& artists)
      : id(id), name(name), album(album), album_id(album_id), artists(artists) {
    empty = false;
  };

  RecordA& operator=(const RecordA& other) {
    if (this != &other) {
      id = other.id;
      name = other.name;
      album = other.album;
      album_id = other.album_id;
      artists = other.artists;
    }
    return *this;
  }

  string key() const override { return id; };

  void Print() override {
    cout << "ID: " << id << endl;
    cout << "Name: " << name << endl;
    cout << "Album: " << album << endl;
    cout << "Album ID: " << album_id << endl;
    cout << "Artists: " << artists << endl;
  };
};

class RecordB : public Record {
 public:
  int year;
  string make;
  string model;
  string vin;

  RecordB() = default;
  RecordB(const AvlRB& avl_record);
  RecordB(int year, const string& make, const string& model, const string& vin)
      : year(year), make(make), model(model), vin(vin) {
    empty = false;
  };

  RecordB& operator=(const RecordB& other) {
    if (this != &other) {
      year = other.year;
      make = other.make;
      model = other.model;
      vin = other.vin;
    }

    return *this;
  }

  string key() const override { return vin; };

  void Print() override {
    cout << "Year: " << year << endl;
    cout << "Make: " << make << endl;
    cout << "Model: " << model << endl;
    cout << "VIN: " << vin << endl;
  };
};

#endif  // RECORD_HPP
