#include <iostream>
#include <chrono>
#include <fstream>
#include <string>
#include "EHC.h"
using namespace std;


int main(){

    string csv_file_name = "DataBase.csv";
    ExtendibleHash eHash("Indice.bin", "Datos.bin");
    cout << "------------------Inserción----------------------" << endl;
    cout << "Cargando datos desde " << csv_file_name << "..." << endl;
    //Inserción
    chrono::time_point<std::chrono::system_clock>inicio,fin;
    inicio = chrono::high_resolution_clock::now();
    // eHash.LoadCsv(csv_file_name);
    cout << "Datos cargados exitosamente." << endl;
    fin = chrono::high_resolution_clock::now();

    std::chrono::duration<double,std::milli>t=fin-inicio;
    cout << "Tiempo Inserción: " << t.count() << " ms"<<endl;

    //Búsqueda
    cout << "------------------Búsqueda----------------------" << endl;
    int count = 0;
    int total = 0;
    inicio = chrono::high_resolution_clock::now();

    Record r = eHash.searchRecord(1, count);
    r.display();
    total += count;
    r = eHash.searchRecord(9, count);
    r.display();
    total += count;
    r = eHash.searchRecord(19, count);
    r.display();
    total += count;
    r = eHash.searchRecord(39, count);
    r.display();
    total += count;
    r = eHash.searchRecord(78, count);
    r.display();
    total += count;
    r = eHash.searchRecord(156, count);
    r.display();
    total += count;
    r = eHash.searchRecord(312, count);
    r.display();
    total += count;
    r = eHash.searchRecord(625, count);
    r.display();
    total += count;
    r = eHash.searchRecord(1250, count);
    r.display();
    total += count;
    r = eHash.searchRecord(2500, count);
    r.display();
    total += count;
    r = eHash.searchRecord(5000, count);
    r.display();
    total += count;

    cout << "Total de accesos: " << count << endl;
    fin = chrono::high_resolution_clock::now();

    t=fin-inicio;
    cout << "Tiempo Búsqueda: " << t.count() << " ms"<<endl;

//    eHash.muestraData();
}