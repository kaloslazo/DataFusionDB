#include "ExtendibleHashing.hpp"
#include "Bucket.cpp"
// Implementation

template <class RECORD, class TK>
ExtendibleHashing<RECORD, TK>::ExtendibleHashing(int global_depth, int bucket_size) {
    Global_depth = global_depth;
    Num_cells = pow(2, Global_depth);
    Bucket_size = bucket_size;
    Bucket_addresses.resize(Num_cells, -1);
}

template <class RECORD, class TK>
ExtendibleHashing<RECORD, TK>::~ExtendibleHashing() {
    Close();
}

template <class RECORD, class TK>
void ExtendibleHashing<RECORD, TK>::Close() {
    std::cout << "[!] Closing files" << std::endl;
    if (File_data.is_open()) {
        std::cout << "[-] Closing file data" << std::endl;
        File_data.close();
    }
    if (File_buckets.is_open()) {
        std::cout << "[-] Closing file buckets" << std::endl;
        File_buckets.close();
    }
}

template <class RECORD, class TK>
int ExtendibleHashing<RECORD, TK>::Hash(TK key) {
    std::hash<TK> hash_key;
    size_t key_hash = hash_key(key);
    int bucket_index = key_hash & ((1 << Global_depth) - 1);
    // std::cout << "Hashing key: " << key << ", Hash value: " << bucket_index << std::endl;
    return bucket_index;
}

template <class RECORD, class TK>
void ExtendibleHashing<RECORD, TK>::Create(std::string filename_data, std::string filename_buckets) {
    File_data.open(filename_data, std::ios::in | std::ios::out | std::ios::binary);
    File_buckets.open(filename_buckets, std::ios::in | std::ios::out | std::ios::binary);

    if (!File_data.is_open()) {
        std::cout << "[+] File not exists. Creating file " << filename_data << std::endl;
        File_data.open(filename_data, std::ios::out | std::ios::in | std::ios::binary | std::ios::trunc);
        File_data.close();
        File_data.open(filename_data, std::ios::in | std::ios::out | std::ios::binary);
    }

    if (!File_buckets.is_open()) {
        std::cout << "[+] File not exists. Creating file " << filename_buckets << std::endl;
        File_buckets.open(filename_buckets, std::ios::out | std::ios::in | std::ios::binary | std::ios::trunc);
        File_buckets.close();
        File_buckets.open(filename_buckets, std::ios::in | std::ios::out | std::ios::binary);
    }

    Bucket<RECORD, TK>* bucket_zero = new Bucket<RECORD, TK>(Bucket_size);
    Bucket<RECORD, TK>* bucket_one = new Bucket<RECORD, TK>(Bucket_size);

    Save_bucket(bucket_zero, 0);
    Save_bucket(bucket_one, 1);

    File_data.seekp(0, std::ios::beg);
    File_data.write(reinterpret_cast<char*>(&Bucket_addresses[0]), Num_cells * sizeof(int));

    delete bucket_zero;
    delete bucket_one;
}

template <class RECORD, class TK>
bool ExtendibleHashing<RECORD, TK>::Open(std::string filedata, std::string filebuckets) {
    File_data.open(filedata, std::ios::in | std::ios::out | std::ios::binary);
    File_buckets.open(filebuckets, std::ios::in | std::ios::out | std::ios::binary);

    if (!File_data.is_open()) {
        std::cout << "[!] Error opening file " << filedata << std::endl;
        return false;
    }

    if (!File_buckets.is_open()) {
        std::cout << "[!] Error opening file " << filebuckets << std::endl;
        return false;
    }

    File_data.seekg(0, std::ios::beg);
    File_data.read(reinterpret_cast<char*>(&Bucket_addresses[0]), Num_cells * sizeof(int));

    return true;
}

template <class RECORD, class TK>
void ExtendibleHashing<RECORD, TK>::Save_bucket(Bucket<RECORD, TK>* bucket, int bucket_directory_index) {
    File_buckets.seekp(0, std::ios::end);
    int bucket_offset = File_buckets.tellp();

    // Escribir los datos del bucket, no el puntero
    File_buckets.write(reinterpret_cast<char*>(&bucket->Local_depth), sizeof(int));
    File_buckets.write(reinterpret_cast<char*>(&bucket->Max_bucket_size), sizeof(int));
    File_buckets.write(reinterpret_cast<char*>(&bucket->Current_size), sizeof(int));
    for (int i = 0; i < bucket->Current_size; i++) {
        File_buckets.write(reinterpret_cast<char*>(&bucket->Records[i]), sizeof(RECORD));
    }
    File_buckets.flush();

    Bucket_addresses[bucket_directory_index] = bucket_offset;
}

template <class RECORD, class TK>
Bucket<RECORD, TK>* ExtendibleHashing<RECORD, TK>::Load_bucket(int bucket_directory_index) {
    int bucket_offset = Bucket_addresses[bucket_directory_index];

    File_buckets.seekg(bucket_offset, std::ios::beg);

    Bucket<RECORD, TK>* bucket = new Bucket<RECORD, TK>(Bucket_size);
    File_buckets.read(reinterpret_cast<char*>(&bucket->Local_depth), sizeof(int));
    File_buckets.read(reinterpret_cast<char*>(&bucket->Max_bucket_size), sizeof(int));
    File_buckets.read(reinterpret_cast<char*>(&bucket->Current_size), sizeof(int));
    for (int i = 0; i < bucket->Current_size; i++) {
        File_buckets.read(reinterpret_cast<char*>(&bucket->Records[i]), sizeof(RECORD));
    }

    return bucket;
}

template <class RECORD, class TK>
bool ExtendibleHashing<RECORD, TK>::Insert(RECORD record) {
    int bucket_directory_index = Hash(record.key());

    while (true) {
        Bucket<RECORD, TK>* bucket = Load_bucket(bucket_directory_index);

        if (bucket->Current_size < bucket->Max_bucket_size) {
            bucket->Insert(record);
            // std::cout << "[+] Inserted record and saved key " << record.key() << " in bucket in position " << bucket_directory_index << std::endl;
            Save_bucket(bucket, bucket_directory_index);
            delete bucket;  // Asegúrate de liberar la memoria aquí
            return true;
        }

        // std::cout << "[!] Bucket is full. Splitting bucket" << std::endl;
        Split_bucket(record.key(), bucket_directory_index);
        delete bucket;  // Asegúrate de liberar la memoria aquí también

        bucket_directory_index = Hash(record.key());
    }
}

template <class RECORD, class TK>
std::optional<RECORD> ExtendibleHashing<RECORD, TK>::Search(TK key) {
    int bucket_index = Hash(key);
    // std::cout << "Searching in bucket: " << bucket_index << std::endl;
    Bucket<RECORD, TK>* bucket = Load_bucket(bucket_index);
    // std::cout << "Loaded bucket at address: " << bucket << std::endl;
    std::optional<RECORD> result = bucket->Search(key);
    if (result) {
        std::cout << "Record found in bucket" << std::endl;
    } else {
        std::cout << "Record not found in bucket" << std::endl;
    }
    delete bucket;
    return result;
}

template <class RECORD, class TK>
void ExtendibleHashing<RECORD, TK>::Split_bucket(TK key, int bucket_directory_index) {
    Bucket<RECORD, TK>* old_bucket = Load_bucket(bucket_directory_index);

    if (old_bucket->Local_depth == Global_depth) {
        // std::cout << "[!] Expanding directory" << std::endl;
        Expand_directory();
    }

    old_bucket->Local_depth++;

    Bucket<RECORD, TK>* new_bucket = new Bucket<RECORD, TK>(old_bucket->Max_bucket_size);
    new_bucket->Local_depth = old_bucket->Local_depth;

    int new_bucket_index = bucket_directory_index | (1 << (old_bucket->Local_depth - 1));

    // std::cout << "[+] Redistributing records between buckets" << std::endl;

    for (int i = 0; i < old_bucket->Current_size; i++) {
        RECORD* record = &old_bucket->Records[i];
        int hash = Hash(record->key());

        if ((hash & ((1 << old_bucket->Local_depth) - 1)) == new_bucket_index) {
            new_bucket->Insert(*record);
            old_bucket->Remove(record->key());
            i--;
        }
    }

    for (int i = 0; i < Num_cells; i++) {
        if ((i & ((1 << old_bucket->Local_depth) - 1)) == new_bucket_index) {
            Bucket_addresses[i] = File_buckets.tellp();
        }
    }

    // std::cout << "[+] Redistributed records" << std::endl;
    Save_bucket(old_bucket, bucket_directory_index);
    Save_bucket(new_bucket, new_bucket_index);

    delete old_bucket;
    delete new_bucket;
}

template <class RECORD, class TK>
void ExtendibleHashing<RECORD, TK>::Expand_directory() {
    int new_num_cells = 2 * Num_cells;
    std::vector<int> new_bucket_addresses(new_num_cells);

    for (int i = 0; i < Num_cells; i++) {
        new_bucket_addresses[i] = Bucket_addresses[i];
        new_bucket_addresses[i + Num_cells] = Bucket_addresses[i];
    }

    Global_depth++;
    Num_cells = new_num_cells;
    Bucket_addresses = new_bucket_addresses;

    File_data.seekp(0, std::ios::beg);
    File_data.write(reinterpret_cast<char*>(&Bucket_addresses[0]), Num_cells * sizeof(int));
    File_data.flush();
}

template <class RECORD, class TK>
void ExtendibleHashing<RECORD, TK>::Print() {
    std::cout << std::endl << "[!] Print directory with " << Num_cells << " cells" << std::endl;

    for (int i = 0; i < Num_cells; i++) {
        std::cout << "[+] Cell " << i << " with bucket address " << Bucket_addresses[i] << std::endl;
        Bucket<RECORD, TK>* bucket = Load_bucket(i);
        bucket->Print();
        std::cout << std::endl;
        delete bucket;
    }
}

template <class RECORD, class TK>
void ExtendibleHashing<RECORD, TK>::Load_csv(std::string filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "[!] Error. Can't open file: " << filename << std::endl;
        throw std::runtime_error("Error. Can't open file");
    }

    std::string line;
    std::getline(file, line);  // skip header

    std::cout << "[!] Loading data from file " << filename << std::endl;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string token;

        char id[23] = {0}, name[521] = {0}, album[244] = {0}, album_id[23] = {0}, artists[1124] = {0};

        if (std::getline(ss, token, ',')) {
            std::strncpy(id, token.c_str(), sizeof(id) - 1);
        }
        if (std::getline(ss, token, ',')) {
            std::strncpy(name, token.c_str(), sizeof(name) - 1);
        }
        if (std::getline(ss, token, ',')) {
            std::strncpy(album, token.c_str(), sizeof(album) - 1);
        }
        if (std::getline(ss, token, ',')) {
            std::strncpy(album_id, token.c_str(), sizeof(album_id) - 1);
        }
        if (std::getline(ss, token, ',')) {
            std::strncpy(artists, token.c_str(), sizeof(artists) - 1);
        }

        if (id[0] == '\0') {  // Skip empty records
            std::cout << "Skipping empty record" << std::endl;
            continue;
        }

        // std::cout << "ID: " << id << std::endl;

        RECORD record(id, name, album, album_id, artists);
        //record.Print();

        Insert(record);
    }
}

// template <class RECORD, class TK>
// bool ExtendibleHashing<RECORD, TK>::Remove(TK key) {
//     int bucket_index = Hash(key);
//     Bucket<RECORD, TK>* bucket = Load_bucket(bucket_index);
    
//     bool removed = false;
//     for (int i = 0; i < bucket->Current_size; i++) {
//         if (bucket->Records[i].key() == key) {
//             // Shift all elements after the removed one
//             for (int j = i; j < bucket->Current_size - 1; j++) {
//                 bucket->Records[j] = bucket->Records[j + 1];
//             }
//             bucket->Current_size--;
//             removed = true;
//             break;
//         }
//     }

//     if (removed) {
//         Save_bucket(bucket, bucket_index);
//         std::cout << "Record with key " << key << " removed from bucket " << bucket_index << std::endl;
//     } else {
//         std::cout << "Record with key " << key << " not found in bucket " << bucket_index << std::endl;
//     }

//     delete bucket;
//     return removed;
// }

