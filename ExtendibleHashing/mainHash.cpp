#include <iostream>
#include "extendibleHash.h"

using namespace std;

int main(){
    ExtendibleHash eh("index.bin", "records.bin");
    // cout << "---Inserción 1---" << endl;
    // eh.insertRecord(Record(1, "nombre1", "apellido1", 1, "carrera1", 1)); //1 en binario es 001
    cout << "---Inserción 2---" << endl;
    eh.insertRecord(Record(3, "nombre2", "apellido2", 2, "carrera2", 2)); //3 en binario es 011
    // cout << "---Inserción 3---" << endl;
    // eh.insertRecord(Record(4, "nombre3", "apellido3", 3, "carrera3", 3)); //4 en binario es 100

    cout << "Aun no acaba el main" << endl;

    return 0;
}