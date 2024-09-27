#ifndef EXTENDIBLERA_HPP
#define EXTENDIBLERA_HPP
#include <string>
#include <iostream>
#include <iomanip>
#include <string.h>
#include "../Record.hpp"

class EHRecordA : public Record {
public:
  char id[23];
  char name[521];
  char album[244];
  char album_id[23];
  char artists[1124];

  EHRecordA() = default;
  ~EHRecordA() = default;

  EHRecordA(const char* id, const char* name, const char* album, const char* album_id, const char* artists) {
    strncpy(this->id, id ? id : "", sizeof(this->id) - 1);
    this->id[sizeof(this->id) - 1] = '\0';
    strncpy(this->name, name ? name : "", sizeof(this->name) - 1);
    this->name[sizeof(this->name) - 1] = '\0';
    strncpy(this->album, album ? album : "", sizeof(this->album) - 1);
    this->album[sizeof(this->album) - 1] = '\0';
    strncpy(this->album_id, album_id ? album_id : "", sizeof(this->album_id) - 1);
    this->album_id[sizeof(this->album_id) - 1] = '\0';
    strncpy(this->artists, artists ? artists : "", sizeof(this->artists) - 1);
    this->artists[sizeof(this->artists) - 1] = '\0';
    empty = false;
  }

  EHRecordA(const RecordA& record) {
    strncpy(this->id, record.id.c_str(), sizeof(this->id) - 1);
    this->id[sizeof(this->id) - 1] = '\0';
    strncpy(this->name, record.name.c_str(), sizeof(this->name) - 1);
    this->name[sizeof(this->name) - 1] = '\0';
    strncpy(this->album, record.album.c_str(), sizeof(this->album) - 1);
    this->album[sizeof(this->album) - 1] = '\0';
    strncpy(this->album_id, record.album_id.c_str(), sizeof(this->album_id) - 1);
    this->album_id[sizeof(this->album_id) - 1] = '\0';
    strncpy(this->artists, record.artists.c_str(), sizeof(this->artists) - 1);
    this->artists[sizeof(this->artists) - 1] = '\0';
    empty = false;
  }

  RecordA to_record() const {
    RecordA record;
    record.id = std::string(id);
    record.name = std::string(name);
    record.album = std::string(album);
    record.album_id = std::string(album_id);
    record.artists = std::string(artists);
    return record;
  }

  string key() const {
    return std::string(id);
  }

  void Print() override {
    std::string sep = " | ";
    std::cout << sep << std::setw(23) << id << sep << std::setw(30) << name 
              << sep << std::setw(243) << album << sep << std::setw(22) << album_id << sep << std::endl;
  }
};

#endif // EXTENDIBLERA_HPP
