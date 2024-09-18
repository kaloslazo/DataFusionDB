#include "Record.hpp"
#include <cstring>
#include <iomanip>
#include <iostream>

using namespace std;

Record::Record() {
  track[0] = '\0';
  album_name[0] = '\0';
  artist[0] = '\0';
  release_date[0] = '\0';
  isrc[0] = '\0';
  all_time_rank = -1;
  track_score = -1;
  spotify_streams = -1;
  spotify_popularity = -1;
  youtube_views = -1;
  tiktok_views = -1;
  airplay_spins = -1;
  siriusmxm_spins = -1;
  pandora_streams = -1;
  soundcloud_streams = -1;
  shazam_counts = -1;
  explicit_tracks = -1;
}

Record::Record(char track[100], char album_name[100], char artist[50], char release_date[10], char isrc[12], int all_time_rank, int track_score, int spotify_streams, int spotify_popularity, int youtube_views, int tiktok_views, int airplay_spins, int siriusmxm_spins, int pandora_streams, int soundcloud_streams, int shazam_counts, int explicit_tracks) {
  strncpy(this->track, track, sizeof(this->track) - 1);
  this->track[sizeof(this->track) - 1] = '\0';
  strncpy(this->album_name, album_name, sizeof(this->album_name) - 1);
  this->album_name[sizeof(this->album_name) - 1] = '\0';
  strncpy(this->artist, artist, sizeof(this->artist) - 1);
  this->artist[sizeof(this->artist) - 1] = '\0';
  strncpy(this->release_date, release_date, sizeof(this->release_date) - 1);
  this->release_date[sizeof(this->release_date) - 1] = '\0';
  strncpy(this->isrc, isrc, sizeof(this->isrc) - 1);
  this->isrc[sizeof(this->isrc) - 1] = '\0';

  this->all_time_rank = all_time_rank;
  this->track_score = track_score;
  this->spotify_streams = spotify_streams;
  this->spotify_popularity = spotify_popularity;
  this->youtube_views = youtube_views;
  this->tiktok_views = tiktok_views;
  this->airplay_spins = airplay_spins;
  this->siriusmxm_spins = siriusmxm_spins;
  this->pandora_streams = pandora_streams;
  this->soundcloud_streams = soundcloud_streams;
  this->shazam_counts = shazam_counts;
  this->explicit_tracks = explicit_tracks;
}

Record::~Record() {}

// Método para imprimir los datos
void Record::Print() {
  string sep = " | ";

  // Imprimir una línea con separadores y setw para formatear columnas
  cout << sep << setw(30) << track
    << sep << setw(30) << album_name
    << sep << setw(20) << artist
    << sep << setw(12) << release_date
    << sep << setw(12) << isrc
    << sep << setw(5) << all_time_rank
    << sep << setw(5) << track_score
    << sep << setw(10) << spotify_streams
    << sep << setw(5) << spotify_popularity
    << sep << setw(10) << youtube_views
    << sep << setw(10) << tiktok_views
    << sep << setw(10) << airplay_spins
    << sep << setw(10) << siriusmxm_spins
    << sep << setw(10) << pandora_streams
    << sep << setw(10) << soundcloud_streams
    << sep << setw(10) << shazam_counts
    << sep << setw(5) << explicit_tracks
    << endl;
}

