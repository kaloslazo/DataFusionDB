#pragma once
#include <iostream>
#include <cstring>

using namespace std;

typedef long pos_t;
#define VOID -1
#define USED -2

struct Record {
    // data
    int codigo = 0;
    char nombre[12] = "";
    char apellido[12] = "";
    int ciclo = 0;
    // pointers
    pos_t left = VOID;
    pos_t right = VOID;
    pos_t nextdel = USED;

    void showData() {
        cout << "\nCodigo   : " << codigo;
        cout << "\nNombre   : " << nombre;
        cout << "\nApellido : " << apellido;
        cout << "\nCiclo    : " << ciclo;
        cout << "\n";
    }

    bool operator==(const Record& other) const {
        return codigo == other.codigo && strcmp(nombre, other.nombre) == 0 &&
               strcmp(apellido, other.apellido) == 0 && ciclo == other.ciclo;
    }

    int key() { return codigo; }
};
