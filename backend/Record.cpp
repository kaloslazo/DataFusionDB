#include "Record.hpp"
#include "AvlRecordA.hpp"
#include "AvlRecordB.hpp"

RecordA::RecordA(const AvlRecordA& avl_record) {
  id = string(avl_record.id);
  name = string(avl_record.name);
  album = string(avl_record.album);
  album_id = string(avl_record.album_id);
  artists = string(avl_record.artists);
  empty = false;
}

RecordB::RecordB(const AvlRecordB& avl_record) {
    year = avl_record.year;
    make = avl_record.make;
    model = avl_record.model;
    vin = avl_record.vin;
    empty = false;
}
