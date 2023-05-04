#include <iostream>
#include <chrono>
#include <fstream>
#include <string>
#include "extendibleHash.h"
using namespace std;


int main(){

    string csv_file_name = "DataBase.csv";
    ExtendibleHash eHash("Indice.bin", "Datos.bin");
    // cout << "Cargando datos desde " << csv_file_name << "..." << endl;
    // //Inserción
    // chrono::time_point<std::chrono::system_clock>inicio,fin;
    // inicio = chrono::high_resolution_clock::now();
    // eHash.LoadCsv(csv_file_name);
    // cout << "Datos cargados exitosamente." << endl;
    // fin = chrono::high_resolution_clock::now();

    // std::chrono::duration<double,std::milli>t=fin-inicio;
    // cout << "Tiempo Inserción: " << t.count() << " ms"<<endl;

    // //Búsqueda
    // inicio = chrono::high_resolution_clock::now();
    // eHash.searchRecord(1250);
    // eHash.searchRecord(2500);
    // eHash.searchRecord(5000);
    // fin = chrono::high_resolution_clock::now();

    // t=fin-inicio;
    // cout << "Tiempo Búsqueda: " << t.count() << " ms"<<endl;

    eHash.muestraData();
}