#ifndef RECORD_HPP
#define RECORD_HPP

#include <iostream>

using namespace std;

struct Record {
  char track[100];
  char album_name[100];
  char artist[50];
  char release_date[10];
  char isrc[12]; 
  int all_time_rank;
  int track_score;
  int spotify_streams;
  int spotify_popularity;
  int youtube_views;
  int tiktok_views;
  int airplay_spins;
  int siriusmxm_spins;
  int pandora_streams;
  int soundcloud_streams;
  int shazam_counts;
  int explicit_tracks;
  int left;
  int right;
  int nextdel;

  Record();
  Record(char track[100], char album_name[100], char artist[50], char release_date[10], char isrc[12], int all_time_rank, int track_score, int spotify_streams, int spotify_popularity, int youtube_views, int tiktok_views, int airplay_spins, int siriusmxm_spins, int pandora_streams, int soundcloud_streams, int shazam_counts, int explicit_tracks);
  ~Record();

  void Print();

  string key();
};

#endif // RECORD_HPP

