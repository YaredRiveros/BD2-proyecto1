#include <iostream>
#include "extendibleHash.h"

using namespace std;

int main(){
    ExtendibleHash eh("index.bin", "records.bin");
    // cout << "---Inserción 1---" << endl;
    // eh.insertRecord(Record(1, "nombre1", "apellido1", 1, "carrera1", 1)); //1 en binario es 001 -> bucket 1

    // cout << "---Inserción 2---" << endl;
    // eh.insertRecord(Record(3, "nombre2", "apellido2", 2, "carrera2", 2)); //3 en binario es 011 -> bucket 2
    

    // cout << "---Inserción 4---" << endl;
    // eh.insertRecord(Record(7, "nombre4", "apellido3", 2, "carrera3", 2)); //2 en binario es 111 -> bucket 2

    // cout << "---Inserción 5---" << endl;
    // eh.insertRecord(Record(5, "nombre5", "apellido3", 2, "carrera3", 2)); //2 en binario es 101 -> bucket 1

    cout << "---Inserción fin---" << endl;
    eh.insertRecord(Record(9, "nombre9", "apellido9", 1, "carrera1", 1)); //1 en binario es 001 -> bucket 1



    cout << "Aun no acaba el main" << endl;

    return 0;
}