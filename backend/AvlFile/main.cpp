#include <algorithm>
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <ctime>
#include "AvlFileA.hpp"
#include "AvlFileB.hpp"

using namespace std;

bool debug = false;

// ========================================
//              TEST FUNCTION
// ========================================

vector<RecordB> leerCSV(const string& filename) {
    ifstream file(filename);
    vector<RecordB> records;
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
        RecordB record;

        getline(ss, token, ',');
        record.year = stoi(token);

        getline(ss, token, ',');
        strncpy(record.make, token.c_str(), sizeof(record.make));
        record.make[sizeof(record.make) - 1] = '\0';

        getline(ss, token, ',');
        strncpy(record.model, token.c_str(), sizeof(record.model));
        record.model[sizeof(record.model) - 1] = '\0';

        getline(ss, token, '\n');
        strncpy(record.vin, token.c_str(), sizeof(record.vin));
        record.vin[sizeof(record.vin) - 1] = '\0';

        records.push_back(record);
    }

    file.close();
    return records;
}

void readFile() {
    int count = 0;
    AVLFileB<string> file;
    if (debug)
        file.debugAVL();
    cout << "------------------------------------------\n";
    vector<RecordB> records = leerCSV("../../data/car_prices_extended.csv");
    cout << "Cantidad de records: " << records.size() << "\n";

    cout << "Starting insertion...\n";

    // time
    clock_t start = clock();
    file.loadCSV("../../data/car_prices_extended.csv");
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
        RecordB r = file.find(record.key());
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

    // search 1 record
    file.n_access = 0;
    start = clock();
    RecordB r = file.find(records[0].key());
    end = clock();
    cout << "Time for search: " << (double)(end - start) / CLOCKS_PER_SEC << "\n";
    cout << "n_access: " << file.n_access << "\n";

    // delete 1 record
    file.n_access = 0;
    start = clock();
    file.remove(records[9855].key());
    end = clock();
    cout << "Time for delete: " << (double)(end - start) / CLOCKS_PER_SEC << "\n";
    cout << "n_access: " << file.n_access << "\n";

    // insert 1 record
    file.n_access = 0;
    start = clock();
    file.insert(records[9855]);
    end = clock();
    cout << "Time for insert: " << (double)(end - start) / CLOCKS_PER_SEC << "\n";
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
