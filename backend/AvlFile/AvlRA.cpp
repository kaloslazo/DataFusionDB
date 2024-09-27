#include <cstring>
#include "AvlRA.hpp"

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
  strncpy(id, record.id.c_str(), sizeof(id) - 1);id[sizeof(id) - 1] = '\0';
  strncpy(name, record.name.c_str(), sizeof(name) - 1);name[sizeof(name) - 1] = '\0';
  strncpy(album, record.album.c_str(), sizeof(album) - 1);album[sizeof(album) - 1] = '\0';
  strncpy(album_id, record.album_id.c_str(), sizeof(album_id) - 1);album_id[sizeof(album_id) - 1] = '\0';
  strncpy(artists, record.artists.c_str(), sizeof(artists) - 1);artists[sizeof(artists) - 1] = '\0';
};

RecordA AvlRA::to_record() const {
  RecordA record;
  record.id = string(id, 23);
  record.name = string(name, 521);
  record.album = string(album, 244);
  record.album_id = string(album_id, 23);
  record.artists = string(artists, 1124);
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
