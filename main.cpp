#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "record.h"
#include "sequential.h"
using namespace std;

vector<Record> read_csv(const string& file_path) {
    ifstream file(file_path);
    string line;
    vector<Record> records;

    if (file.is_open()) {
        getline(file, line); // Leer la primera línea y descartarla (encabezados)

        while (getline(file, line)) {
            stringstream ss(line);
            string token;

            vector<string> row;
            while (getline(ss, token, ',')) {
                row.push_back(token);
            }

            Record record(row[0], row[1], row[2], row[3], row[4]);
            records.push_back(record);
        }

        file.close();
    } else {
        cerr << "No se pudo abrir el archivo." << endl;
    }

    return records;
}

int main() {
    string csv_file_path = "data_chiqui.csv"; // Reemplaza "data-proyecto1.csv" con el nombre de tu archivo .csv
    vector<Record> records = read_csv(csv_file_path);

    // Crear e inicializar un objeto de la clase SequentialFile
    SequentialFile<Record, const char*> mySequentialFile("mySequentialFile2.bin", "dummie2.bin");

    // Insertar los registros en la clase SequentialFile
    for (const auto &record : records) {
        string key_str = record.get_key();
        mySequentialFile.add_record(record);
    }

    // Buscar un registro en el SequentialFile
    string codigo = "202208710"; // Reemplaza esto con un código para buscar
    vector<Record> found_records = mySequentialFile.search_record(codigo.c_str());

    if (!found_records.empty()) {
        for (const auto& found_record : found_records) {
            cout << "Registro encontrado: " << found_record.nombre << ", " << found_record.apellido << endl;
        }
    } else {
        cout << "Registro no encontrado." << endl;
    }

    return 0;
}
