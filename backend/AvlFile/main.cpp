#include <fstream>
#include <random>
#include <algorithm>
#include <iostream>
#include <sstream>
#include "AvlFileA.hpp"

using namespace std;

bool debug = false;

// ========================================
//              TEST FUNCTION
// ========================================

vector<Record> leerCSV(const string& filename) {
    ifstream file(filename);
    vector<Record> records;
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
        Record record;

        getline(ss, token, ',');
        record.codigo = stoi(token);

        getline(ss, token, ',');
        strncpy(record.nombre, token.c_str(), sizeof(record.nombre));
        record.nombre[sizeof(record.nombre) - 1] =
            '\0';  // Asegurar el null-termination

        getline(ss, token, ',');
        strncpy(record.apellido, token.c_str(), sizeof(record.apellido));
        record.apellido[sizeof(record.apellido) - 1] =
            '\0';  // Asegurar el null-termination

        getline(ss, token, ',');
        record.ciclo = stoi(token);

        records.push_back(record);
    }

    file.close();
    return records;
}

void readFile(string filename) {
    AVLFile<int> file(filename);
    if (debug)
        file.debugAVL();
    cout << "------------------------------------------\n";
    vector<Record> records = leerCSV("datos.csv");
    cout << "Starting insertion...\n";
    for (auto& record : records) {
        file.insert(record);
    }
    cout << "Finishing insertion...\n";
    if (debug)
        file.debugAVL();

    shuffle(records.begin(), records.end(), default_random_engine(0));
    cout << "Starting deletion...\n";
    for (auto& record : records) {
        // cout << "Deleting record with key: " << record << "\n";
        if (!file.remove(record.key())) {
            cout << "Error al eliminar record con codigo: " << record.key()
                 << "\n";
            break;
        }
    }
    cout << "Finishing deletion...\n";

    cout << "Starting insertion...\n";
    for (auto& record : records) {
        file.insert(record);
    }
    cout << "Finishing insertion...\n";

    if (debug)
        file.debugAVL();
    cout << "------------------------------------------\n";
    bool passed = true;
    for (auto& record : records) {
        Record r = file.find(record.codigo);
        if (!(r == record)) {
            passed = false;
            cout << "Error en el record con codigo: " << record.codigo << "\n";
            cout << "Se esperaba: \n";
            record.showData();
            cout << "\nSe obtuvo: \n";
            r.showData();
            cout << "\n";
            break;
        }
    }
    if (passed)
        cout << "Todos los records fueron leidos correctamente\n";


    int expected_size = sizeof(Record) * records.size() + sizeof(Header);

    ifstream fin(filename, ios::binary);
    fin.seekg(0, ios::end);
    if (fin.tellg() == expected_size) {
        cout << "No hay espacio desperdiciado en el binario.\n";
    } else if (fin.tellg() > expected_size) {
        cout << "Hay espacio desperdiciado en el binario: "
             << (int)fin.tellg() - expected_size << " bytes sin usar.\n";
    } else {
        cout << "Hay menos bytes de lo esperado. Expected: " << expected_size
             << ". Found: " << fin.tellg() << "\n";
    }
    fin.close();

    cout << "Test concluido\n";
}

int main(int argc, char** argv) {
    // si se quiere observar los detalles (como la rotacion o insercion), usar
    // el parametro 'debug' o el flag
    // '-d' al ejecutar
    if (argc > 1 &&
        (strcmp(argv[1], "debug") == 0 || strcmp(argv[1], "-d") == 0))
        debug = true;

    // detalles del test:
    // 1. leer 1000 records de datos.csv e insertar uno por uno en el AVL
    // 2. borrar los 1000 records en orden aleatorio (diferente a la entrada), uno por uno
    // 3. insertar los 1000 records otra vez. Aparecerá un warning si es que se intenta insertar duplicados
    // 4. revisar que los 1000 records se puedan leer correctamente
    // 5. revisar que no haya espacio desperdiciado (el free list funciona correctamente)
    //
    // nota: utilizar un archivo avl_data.dat vacio para evitar los warnings durante la primera insecion.
    readFile("avl_data.dat");
    return 0;
}
