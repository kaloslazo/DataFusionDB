#ifndef SEQUENTIALRA_HPP
#define SEQUENTIALRA_HPP
#include <cstring>
#include <iostream>
#include <algorithm>
#include "../Record.hpp"

struct SequentialRA {
  char id[24];       // Increased by 1 for null terminator
  char name[522];    // Increased by 1 for null terminator
  char album[245];   // Increased by 1 for null terminator
  char album_id[24]; // Increased by 1 for null terminator
  char artists[1125];// Increased by 1 for null terminator

  SequentialRA() {
    memset(this, 0, sizeof(SequentialRA));
  }

  inline SequentialRA(const RecordA& record) {
    setId(record.id);
    setName(record.name);
    setAlbum(record.album);
    setAlbumId(record.album_id);
    setArtists(record.artists);
  }

  void setId(const std::string& value) { copyAndTruncate(id, value, sizeof(id)); }
  void setName(const std::string& value) { copyAndTruncate(name, value, sizeof(name)); }
  void setAlbum(const std::string& value) { copyAndTruncate(album, value, sizeof(album)); }
  void setAlbumId(const std::string& value) { copyAndTruncate(album_id, value, sizeof(album_id)); }
  void setArtists(const std::string& value) { copyAndTruncate(artists, value, sizeof(artists)); }

  static void copyAndTruncate(char* dest, const std::string& src, size_t maxLen) {
    size_t len = std::min(src.length(), maxLen - 1);
    std::copy_n(src.begin(), len, dest);
    dest[len] = '\0';
  }

  inline void Print() const {
    std::cout << "ID: " << id << std::endl;
    std::cout << "Name: " << name << std::endl;
    std::cout << "Album: " << album << std::endl;
    std::cout << "Album ID: " << album_id << std::endl;
    std::cout << "Artists: " << artists << std::endl;
  }

  inline std::string key() const { 
    return std::string(id);
  }

  inline RecordA to_record() const {
    RecordA record;
    record.id = id;
    record.name = name;
    record.album = album;
    record.album_id = album_id;
    record.artists = artists;
    return record;
  }
};
#endif // SEQUENTIALRA_HPP
