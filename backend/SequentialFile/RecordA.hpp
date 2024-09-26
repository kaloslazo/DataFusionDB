#pragma once
#include <cstring>
#include <iostream>
#include "../Record.hpp"

using namespace std;

struct SeqRecordA {
    char id[23];
    char name[521];
    char album[244];
    char album_id[23];
    char artists[1124];

    void Print() {
        cout << "ID: " << id << endl;
        cout << "Name: " << name << endl;
        cout << "Album: " << album << endl;
        cout << "Album ID: " << album_id << endl;
        cout << "Artists: " << artists << endl;
    }

    string key() { return string(id, 23); }

    SeqRecordA to_record() {
        RecordA record;

        record.id = string(id, 23);
        record.name = string(name, 521);
        record.album = string(album, 244);
        record.album_id = string(album_id, 23);
        record.artists = string(artists, 1124);
        record.empty = false;

        return record;
    }

    SeqRecordA() {}
    SeqRecordA(const RecordA& record) {
        strncpy(id, record.id.c_str(), sizeof(id));
        id[sizeof(id) - 1] = '\0';
        strncpy(name, record.name.c_str(), sizeof(name));
        name[sizeof(name) - 1] = '\0';
        strncpy(album, record.album.c_str(), sizeof(album));
        album[sizeof(album) - 1] = '\0';
        strncpy(album_id, record.album_id.c_str(), sizeof(album_id));
        album_id[sizeof(album_id) - 1] = '\0';
        strncpy(artists, record.artists.c_str(), sizeof(artists));
        artists[sizeof(artists) - 1] = '\0';
    }
};