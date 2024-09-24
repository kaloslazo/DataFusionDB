#ifndef EXTENDIBLEHRECORDA_HPP
#define EXTENDIBLEHRECORDA_HPP

#include <cstring>
#include <iomanip>
#include <iostream>

struct EHRecordA {
  char id[23];
  char name[521];
  char album[244];
  char album_id[23];
  char artists[1124];

  EHRecordA() = default;

  EHRecordA(const char id[23],
            const char name[521],
            const char album[244],
            const char album_id[23],
            const char artists[1124]) {
    strncpy(this->id, id, 22);
    this->id[22] = '\0';
    strncpy(this->name, name, 520);
    this->name[520] = '\0';
    strncpy(this->album, album, 243);
    this->album[243] = '\0';
    strncpy(this->album_id, album_id, 22);
    this->album_id[22] = '\0';
    strncpy(this->artists, artists, 1123);
    this->artists[1123] = '\0';
  }
  ~EHRecordA() = default;

  void Print() {
    std::string sep = " | ";
    std::cout << sep << std::setw(23) << id << sep << std::setw(30) << name << sep << std::setw(243) << album << sep << std::setw(22) << album_id << sep << std::endl;
  }

  std::string key() { return std::string(id, 23); }
};

#endif  // EXTENDIBLEHRECORDA_HPP
