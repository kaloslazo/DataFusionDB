#ifndef RECORDA_HPP
#define RECORDA_HPP

#include <iostream>

using namespace std;

struct RecordA {
    int left = -1;
    int right = -1;
    int nextdel = -1;

    RecordA() {}
    ~RecordA() {}

    void Print() {}

    string key() {
        return "";
    }
};

#endif  // RECORDA_HPP
