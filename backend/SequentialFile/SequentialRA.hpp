#pragma once
#include <cstring>
#include <iostream>
#include "../Record.hpp"

struct SequentialRA {
  char id[23];
  char name[521];
  char album[244];
  char album_id[23];
  char artists[1124];

  SequentialRA() = default;

  inline SequentialRA(const RecordA& record) {
    strncpy(id, record.id.c_str(), sizeof(id) - 1);
    id[sizeof(id) - 1] = '\0';
    strncpy(name, record.name.c_str(), sizeof(name) - 1);
    name[sizeof(name) - 1] = '\0';
    strncpy(album, record.album.c_str(), sizeof(album) - 1);
    album[sizeof(album) - 1] = '\0';
    strncpy(album_id, record.album_id.c_str(), sizeof(album_id) - 1);
    album_id[sizeof(album_id) - 1] = '\0';
    strncpy(artists, record.artists.c_str(), sizeof(artists) - 1);
    artists[sizeof(artists) - 1] = '\0';
  }

  inline void Print() const {
    std::cout << "ID: " << id << std::endl;
    std::cout << "Name: " << name << std::endl;
    std::cout << "Album: " << album << std::endl;
    std::cout << "Album ID: " << album_id << std::endl;
    std::cout << "Artists: " << artists << std::endl;
  }

  inline std::string key() const { 
    return std::string(id, 23); 
  }

  inline RecordA to_record() const {
    RecordA record;
    record.id = std::string(id, 23);
    record.name = std::string(name, 521);
    record.album = std::string(album, 244);
    record.album_id = std::string(album_id, 23);
    record.artists = std::string(artists, 1124);
    record.empty = false;
    return record;
  }
};