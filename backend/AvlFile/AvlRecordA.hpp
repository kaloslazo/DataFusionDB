#ifndef RECORDA_HPP
#define RECORDA_HPP

#include <iostream>

using namespace std;

struct AvlRecordA {
    int left = -1;
    int right = -1;
    int nextdel = -1;

    void Print() {}

    string key() {
        return "";
    }
};

#endif  // RECORDA_HPP
