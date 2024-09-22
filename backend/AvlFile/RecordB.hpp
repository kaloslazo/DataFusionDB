#ifndef RECORDB_HPP
#define RECORDB_HPP

#include <iostream>

using namespace std;

struct Record {
    int left = -1;
    int right = -1;
    int nextdel = -1;

    Record() {}
    ~Record() {}

    void Print() {}

    string key() {
        return "";
    }
};

#endif  // RECORDB_HPP
