#include "Bucket.hpp"

// Implementation

template <class RECORD, class TK>
Bucket<RECORD, TK>::Bucket(int max_keys) {
    Local_depth = 1;
    Max_bucket_size = max_keys;
    Current_size = 0;
    Records = new RECORD[Max_bucket_size];
}

template <class RECORD, class TK>
Bucket<RECORD, TK>::~Bucket() {
    delete[] Records;
}

template <class RECORD, class TK>
int Bucket<RECORD, TK>::Insert(RECORD& record) {
    if (Current_size == Max_bucket_size) {
        std::cout << "[!] Bucket is full" << std::endl;
        return -1;
    }
    Records[Current_size] = record;
    Current_size++;
    return 0;
}

template <class RECORD, class TK>
std::optional<RECORD> Bucket<RECORD, TK>::Search(TK key) {
    for (int i = 0; i < Current_size; i++) {
        if (Records[i].key() == key) {
            return std::optional<RECORD>(Records[i]);
        }
    }
    return std::nullopt;
}

template <class RECORD, class TK>
void Bucket<RECORD, TK>::Remove(TK key) {
    for (int i = 0; i < Current_size; i++) {
        if (Records[i].key() == key) {
            for (int j = i; j < Current_size - 1; j++) {
                Records[j] = Records[j + 1];
            }
            Current_size--;
            break;
        }
    }
}

template <class RECORD, class TK>
void Bucket<RECORD, TK>::Print() {
    std::cout << "Local depth: " << Local_depth << std::endl;
    std::cout << "Number of records: " << Current_size << std::endl;
    for (int i = 0; i < Current_size; i++) {
        Records[i].Print();
    }
}
