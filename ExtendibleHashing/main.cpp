#include "ExtendibleHashing.hpp"
#include "EHRecordA.hpp"
#include <iostream>

int main() {
    ExtendibleHashing<EHRecordA, std::string> eh(1, 3);
    eh.Create("./data/file_data.dat", "./data/file_buckets.dat");
    eh.Load_csv("./data/spotify_data.csv");
    // eh.Print();

    std::string search_key = "6L0bHTV6hf9UL6uCezlJCC";
    std::cout << "Searching for key: " << search_key << std::endl;
    std::optional<EHRecordA> record = eh.Search(search_key);
    if (record.has_value()) {
        std::cout << "[!] Found record" << std::endl;
        record.value().Print();
        std::cout << std::endl;
    } else {
        std::cout << "[!] Record not found" << std::endl;
    }

    return 0;

}