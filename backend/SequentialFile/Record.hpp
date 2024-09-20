#pragma once
#include <iostream>
#include <cstring>

using namespace std;

struct Record{
    int key;
    char name[12];
    int age;
    int term;

    Record();
    Record(int key, char name[12], int age, int term);
    Record(int key, const char* name, int age, int term);

    void print();
    ~Record();
};
