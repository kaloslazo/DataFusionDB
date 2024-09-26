#include "SequentialFileA.hpp"
#include "SequentialFileB.hpp"
#include <chrono>  
#include <iostream>


using namespace std;


string path = "../../data/";

int main(){
    auto dataA = read_from_csvA(path + "spotify_data_clean.csv");
    auto dataB = read_from_csvB(path + "car_prices_extended.csv");

    //Create the Sequential Files
    SequentialFileA fileA("spotify_data.dat", "spotify_data_aux.dat", sizeof(SeqRecordA));
    SequentialFileB fileB("car_prices.dat", "car_prices_aux.dat", sizeof(SeqRecordB));

    //Insert the records into the Sequential Files
    // for (auto record : dataA) {
    //     fileA.insert(record);
    // }

    // for (auto record : dataB) {
    //     fileB.insert(record);
    // }

    //Show the description of the Sequential Files
    fileA.description();
    fileB.description();


    //Take a look of the times taken to find a record

    cout<<"Searching Times"<<endl;
    auto id = dataA[0].id;
    auto start = chrono::high_resolution_clock::now();
    auto record = fileA.search(id);
    auto end = chrono::high_resolution_clock::now();
    cout << "Time taken to find a record in file A: " << chrono::duration_cast<chrono::nanoseconds>(end - start).count() << "ns" << endl;


    auto vin = dataB[0].vin;
    start = chrono::high_resolution_clock::now();
    auto record2 = fileB.search(vin);
    end = chrono::high_resolution_clock::now();
    cout << "Time taken to find a record in file B: " << chrono::duration_cast<chrono::nanoseconds>(end - start).count() << "ns" << endl;


    cout<<"Inserting Times"<<endl;
    auto recordA = dataA[0];
    start = chrono::high_resolution_clock::now();
    fileA.insert(recordA);
    end = chrono::high_resolution_clock::now();

    cout << "Time taken to insert a record in file A: " << chrono::duration_cast<chrono::nanoseconds>(end - start).count() << "ns" << endl;

    auto recordB = dataB[0];
    start = chrono::high_resolution_clock::now();
    fileB.insert(recordB);
    end = chrono::high_resolution_clock::now();

    cout << "Time taken to insert a record in file B: " << chrono::duration_cast<chrono::nanoseconds>(end - start).count() << "ns" << endl;

    cout<<"Removing Times"<<endl;
    start = chrono::high_resolution_clock::now();
    fileA.remove_record(id);
    end = chrono::high_resolution_clock::now();

    cout << "Time taken to remove a record in file A: " << chrono::duration_cast<chrono::nanoseconds>(end - start).count() << "ns" << endl;

    start = chrono::high_resolution_clock::now();
    fileB.remove_record(vin);
    end = chrono::high_resolution_clock::now();

    cout << "Time taken to remove a record in file B: " << chrono::duration_cast<chrono::nanoseconds>(end - start).count() << "ns" << endl;
}