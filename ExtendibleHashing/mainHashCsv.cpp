#include <iostream>
#include <fstream>
#include <string.h>
#include <sstream>
#include <vector>
#include "extendibleHash.h"

using namespace std;

//Probando ExtendibleHash con un archivo csv

int main(){
    //Borro los datos de los archivos
    fstream indices("Indice.bin", ios::out | ios::binary);
    fstream datos("Datos.bin",ios::out | ios::binary);
    indices.close();
    datos.close();

    string csv_file_name = "data.csv";
    ExtendibleHash eHash("Indice.bin", "Datos.bin");
    cout << "Cargando datos desde " << csv_file_name << "..." << endl;
    eHash.LoadCsv(csv_file_name);
    cout << "Datos cargados exitosamente." << endl;

    // Record r(1, "Juan Perez", "Juan.Perez@utec.edu",4, "Mecatronica",202110320);

    // eHash.InsertRecordCSV(csv_file_name, r);

    eHash.muestraData();


    cout << "Lectura de datos" << endl;
    ifstream file2;
    file2.open("Datos.bin", ios::in | ios::binary);
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