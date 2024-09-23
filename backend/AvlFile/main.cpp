#include <algorithm>
#include <ctime>
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include "AvlFileA.hpp"
#include "AvlFileB.hpp"

using namespace std;

bool debug = false;

// ========================================
//              TEST FUNCTION
// ========================================

vector<RecordA> leerCSV(const string& filename) {
    ifstream file(filename);
    vector<RecordA> records;
    string line;

    if (!file.is_open()) {
        cerr << "No se pudo abrir el archivo.\n";
        return records;
    }

    // skip header line
    getline(file, line);

    while (getline(file, line)) {
        stringstream ss(line);
        string token;
        AvlRecordA record;

        /*getline(ss, token, ',');*/
        /*record.year = stoi(token);*/
        /**/
        /*getline(ss, token, ',');*/
        /*strncpy(record.make, token.c_str(), sizeof(record.make));*/
        /*record.make[sizeof(record.make) - 1] = '\0';*/
        /**/
        /*getline(ss, token, ',');*/
        /*strncpy(record.model, token.c_str(), sizeof(record.model));*/
        /*record.model[sizeof(record.model) - 1] = '\0';*/
        /**/
        /*getline(ss, token, '\n');*/
        /*strncpy(record.vin, token.c_str(), sizeof(record.vin));*/
        /*record.vin[sizeof(record.vin) - 1] = '\0';*/

        getline(ss, token, ',');
        strncpy(record.id, token.c_str(), sizeof(record.id));
        record.id[sizeof(record.id) - 1] = '\0';

        getline(ss, token, ',');
        strncpy(record.name, token.c_str(), sizeof(record.name));
        record.name[sizeof(record.name) - 1] = '\0';

        getline(ss, token, ',');
        strncpy(record.album, token.c_str(), sizeof(record.album));
        record.album[sizeof(record.album) - 1] = '\0';

        getline(ss, token, ',');
        strncpy(record.album_id, token.c_str(), sizeof(record.album_id));
        record.album_id[sizeof(record.album_id) - 1] = '\0';

        getline(ss, token, '\n');
        strncpy(record.artists, token.c_str(), sizeof(record.artists));
        record.artists[sizeof(record.artists) - 1] = '\0';

        records.push_back(record.to_record());
    }

    file.close();
    return records;
}

string dataset = "../../data/spotify_data_clean.csv";

void readFile() {
    int count = 0;
    AVLFileA<string> file;
    if (debug)
        file.debugAVL();
    cout << "------------------------------------------\n";
    vector<RecordA> records = leerCSV(dataset);
    cout << "Cantidad de records: " << records.size() << "\n";

    cout << "Starting insertion...\n";

    // time
    clock_t start = clock();
    file.loadCSV(dataset);
    clock_t end = clock();
    cout << "Time: " << (double)(end - start) / CLOCKS_PER_SEC << "\n";

    /*for (auto& record : records) {*/
    /*    if (count % 500 == 0) {*/
    /*        cout << "inserted " << count << " records\n";*/
    /*        cout << "n_access: " << file.n_access << "\n";*/
    /*    }*/
    /*    file.insert(record);*/
    /*    count++;*/
    /*}*/
    cout << "n_access: " << file.n_access << "\n";
    cout << "Finishing insertion...\n";
    if (debug)
        file.debugAVL();

    shuffle(records.begin(), records.end(), default_random_engine(0));
    /*cout << "Starting deletion...\n";*/
    /*for (auto& record : records) {*/
    /*    // cout << "Deleting record with key: " << record << "\n";*/
    /*    if (!file.remove(record.key())) {*/
    /*        cout << "Error al eliminar record con codigo: " << record.key()*/
    /*             << "\n";*/
    /*        break;*/
    /*    }*/
    /*}*/
    /*cout << "Finishing deletion...\n";*/
    /**/
    /*cout << "Starting insertion...\n";*/
    /*for (auto& record : records) {*/
    /*    file.insert(record);*/
    /*}*/
    /*cout << "Finishing insertion...\n";*/

    int found = 0;
    if (debug)
        file.debugAVL();
    cout << "------------------------------------------\n";
    bool passed = true;
    for (auto& record : records) {
        RecordA r = file.find(record.key());
        if (!(r.key() == record.key())) {
            passed = false;
            cout << "Error en el record con codigo: " << record.key() << "\n";
            cout << "Se esperaba: \n";
            record.Print();
            cout << "\nSe obtuvo: \n";
            r.Print();
            cout << "\n";

            cout << "Found: " << found << "\n";
            break;
        } else {
            found++;
        }
    }
    if (passed)
        cout << "Todos los records fueron leidos correctamente\n";
    cout << "Test concluido\n";

    // test range search
    string begin_vin = "1f";
    string end_vin = "1g";
    cout << "Range search from " << begin_vin << " to " << end_vin << "\n";
    vector<RecordA> range = file.range_search(begin_vin, end_vin);
    sort(range.begin(), range.end(),
         [](RecordA& a, RecordA& b) { return a.key() < b.key(); });
    cout << "Records found: " << range.size() << "\n";
    cout << "First record: " << range[0].key() << "\n";
    cout << "Last record: " << range[range.size() - 1].key() << "\n";

    // search 1 record
    file.n_access = 0;
    start = clock();
    RecordA r = file.find(records[0].key());
    end = clock();
    cout << "Time for search: " << (double)(end - start) / CLOCKS_PER_SEC
         << "\n";
    cout << "n_access: " << file.n_access << "\n";

    // delete 1 record
    file.n_access = 0;
    start = clock();
    file.remove(records[9855].key());
    end = clock();
    cout << "Time for delete: " << (double)(end - start) / CLOCKS_PER_SEC
         << "\n";
    cout << "n_access: " << file.n_access << "\n";

    // insert 1 record
    file.n_access = 0;
    start = clock();
    file.insert(records[9855]);
    end = clock();
    cout << "Time for insert: " << (double)(end - start) / CLOCKS_PER_SEC
         << "\n";
    cout << "n_access: " << file.n_access << "\n";
}

int main(int argc, char** argv) {
    // si se quiere observar los detalles (como la rotacion o insercion), usar
    // el parametro 'debug' o el flag
    // '-d' al ejecutar
    if (argc > 1 &&
        (strcmp(argv[1], "debug") == 0 || strcmp(argv[1], "-d") == 0))
        debug = true;

    readFile();
    return 0;
}
