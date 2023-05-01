#include <iostream>
#include "extendibleHash.h"
#include <fstream>

using namespace std;

int main(){
    // Limpia index.bin y records.bin
    ofstream file;
    file.open("index.bin", ios::out | ios::binary);
    file.close();
    file.open("records.bin", ios::out | ios::binary);
    file.close();

    ExtendibleHash eh("index.bin", "records.bin");

    cout << "---Inserción 1---" << endl;
    eh.insertRecord(Record(1, "nombre1", "apellido1", 1, "carrera1", 1)); //1 en binario es 001 -> bucket 1

    cout << "---Inserción 2---" << endl;
    eh.insertRecord(Record(2, "nombre2", "apellido2", 2, "carrera2", 2)); //3 en binario es 011 -> bucket 2

    cout << "---Inserción 3---" << endl;
    eh.insertRecord(Record(5, "nombre5", "apellido3", 2, "carrera3", 2)); //2 en binario es 101 -> bucket 1

    cout << "---Inserción 4---" << endl;
    eh.insertRecord(Record(7, "nombre4", "apellido3", 2, "carrera3", 2)); //2 en binario es 111 -> bucket 2

    cout << "---Inserción 5---" << endl;
    eh.insertRecord(Record(9, "nombre9", "apellido9", 1, "carrera1", 1)); //1 en binario es 001 -> bucket 1
    eh.muestraData();

    cout << "---Inserción 6---" << endl;
    eh.insertRecord(Record(11, "nombre11", "apellido11", 2, "carrera2", 2)); //3 en binario es 011 -> bucket 2
    eh.muestraData();
    
    cout << "---Inserción 7---" << endl;
    eh.insertRecord(Record(13, "nombre13", "apellido13", 2, "carrera3", 2)); //2 en binario es 101 -> bucket 1

    cout << "---Inserción 8---" << endl;
    eh.insertRecord(Record(15, "nombre15", "apellido15", 2, "carrera3", 2)); //2 en binario es 111 -> bucket 2


    cout << "---Inserción 9---" << endl;
    eh.insertRecord(Record(17, "nombre17", "apellido17", 1, "carrera1", 1)); //1 en binario es 001 -> bucket 1

    cout << "---Inserción 10---" << endl;
    eh.insertRecord(Record(25, "nombre19", "apellido19", 2, "carrera2", 2)); //3 en binario es 011 -> bucket 2


    
    cout << "---Inserción 11---" << endl;
    eh.insertRecord(Record(33, "nombre21", "apellido21", 2, "carrera3", 2)); //2 en binario es 101 -> bucket 1

    cout << "---Inserción 12---" << endl;
    eh.insertRecord(Record(41, "nombre22", "apellido22", 2, "carrera4", 2)); //2 en binario es 101 -> bucket 1

    cout << "---Inserción 13---" << endl;
    eh.insertRecord(Record(49, "nombre23", "apellido23", 2, "carrera5", 2)); //2 en binario es 101 -> bucket 1

    cout << "---Inserción 14---" << endl;
    eh.insertRecord(Record(57, "nombre24", "apellido24", 2, "carrera6", 2));

    cout << "---Inserción 15---" << endl;
    eh.insertRecord(Record(65, "nombre25", "apellido25", 2, "carrera7", 2));


    cout << "Lectura de datos" << endl;
    ifstream file2;
    file2.open("records.bin", ios::in | ios::binary);
    file2.seekg(0, ios::end);
    long int final = file2.tellg();
    file2.seekg(0, ios::beg);
    while(file2.tellg() < final){
        Bucket b;
        file2.read((char*)&b, sizeof(Bucket));
        cout << "Bucket: " << b.binario << endl;
        for(int i = 0; i < b.nRecords; i++){
            cout << b.records[i].id << " ";
        }
        cout << endl;
    }
    file2.close();

    return 0;
}