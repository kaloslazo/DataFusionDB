#include "AvlRA.hpp"
#include <cstring>

AvlRA::AvlRA() {
  memset(id, 0, sizeof(id));
  memset(name, 0, sizeof(name));
  memset(album, 0, sizeof(album));
  memset(album_id, 0, sizeof(album_id));
  memset(artists, 0, sizeof(artists));
  left = -1;
  right = -1;
  nextdel = -1;
};

AvlRA::AvlRA(const RecordA& record) {
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
};

RecordA AvlRA::to_record() const {
  RecordA record;
  record.id = id;
  record.name = name;
  record.album = album;
  record.album_id = album_id;
  record.artists = artists;
  record.empty = false;
  return record;
};

void AvlRA::Print() {
  printf("id: %s\n", id);
  printf("name: %s\n", name);
  printf("album: %s\n", album);
  printf("album_id: %s\n", album_id);
  printf("artists: %s\n", artists);
  printf("left: %d\n", left);
  printf("right: %d\n", right);
  printf("nextdel: %d\n", nextdel);
};

std::string AvlRA::key() {
  return std::string(id);
};
