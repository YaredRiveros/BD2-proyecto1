#include <iostream>
#include <fstream>
#include <string.h>
#include <sstream>
#include <vector>
#include "extendibleHash.h"

using namespace std;

//Probando ExtendibleHash con un archivo csv

int main(){

    string csv_file_name = "data.csv";
    ExtendibleHash eHash("Indice.bin", "Datos.bin");
    cout << "Cargando datos desde " << csv_file_name << "..." << endl;
    eHash.LoadCsv(csv_file_name);
    cout << "Datos cargados exitosamente." << endl;

    Record r(1, "Juan Perez", "Juan.Perez@utec.edu",4, "Mecatronica",202110320);
    Record r2(2, "Natalia Tafur","Natalia.Tafur@utec.edu",3,"Ingenieria de la energía",202110321);
    eHash.InsertRecordCSV(csv_file_name, r);
    eHash.InsertRecordCSV(csv_file_name, r2);

    eHash.muestraData();

    return 0;    

}