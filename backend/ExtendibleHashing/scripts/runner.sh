#!/bin/bash

# delete only if the file exists
if [ -f "./build/main.out" ]; then
    sudo rm -r "./build/main.out";
fi

if [ -f "./data/file_data.dat" ]; then
    sudo rm -r "./data/file_data.dat";
fi

if [ -f "./data/file_buckets.dat" ]; then
    sudo rm -r "./data/file_buckets.dat";
fi

# compile the code and run the executable
g++ main.cpp -std=c++17 -o ./build/main.out;

if [ -f "./build/main.out" ]; then
    ./build/main.out;
fi
