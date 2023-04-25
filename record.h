#ifndef RECORD_H
#define RECORD_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <unistd.h>
#include <cstring>
using namespace std;

struct Record
{
    char nombre[30]{' '};
    char apellido[30]{' '};
    char ciclo[5]{' '};
    char carrera[20]{' '};
    char codigo[10]{' '};

    int nextDel;
    char ref;

    Record() = default;
    Record(string nombre, string apellido, string ciclo, string carrera, string codigo) {
        strcpy(this->nombre, nombre.c_str());
        strcpy(this->apellido, apellido.c_str());
        strcpy(this->ciclo, ciclo.c_str());
        strcpy(this->carrera, carrera.c_str());
        strcpy(this->codigo, codigo.c_str());

        this->nextDel = 0;
        this->ref = 'd';
    }

    const char* get_key() const {
        return codigo;
    }

    bool equal_key(const char* key) {
        return strcmp(this->codigo, key) == 0;
    }

    bool less_than_key(const char* key) {
        return strcmp(this->codigo, key) < 0;
    }

    bool greater_than_key(const char* key) {
        return strcmp(this->codigo, key) > 0;
    }

    bool less_or_equal(const char* key) {
        return strcmp(this->codigo, key) <= 0;
    }

    bool greater_or_equal(const char* key) {
        return strcmp(this->codigo, key) >= 0;
    }

    bool operator < (Record& other) {
        return strcmp(this->codigo, other.codigo) < 0;
    }

    bool operator == (Record& other) {
        return strcmp(this->codigo, other.codigo) == 0;
    }
};

struct RecordHash {
    int operator()(const char* key) {
        string str(key);
        hash<string> myHash;
        return myHash(str);
    }

    std::string key;
    RecordHash(const std::string& k) : key(k) {}
};

#endif // RECORD_H
