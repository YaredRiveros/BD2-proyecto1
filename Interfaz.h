#include <iostream>
#include <vector>
#include <fstream>
#include <filesystem>
#include "extendibleHash.h"

using namespace std;

struct RecordSequential{
    long int id;
    char nombre[20];
    char correo[20];
    int ciclo;
    char carrera[30];
    int codigo;

    public:
    bool deleted = false;
    RecordSequential(){
        strcpy(nombre, "");
        strcpy(correo, "");
        ciclo = 0;
        strcpy(carrera, "");
        codigo = 0;
        
    }

    RecordSequential(long int id, const char* nombre, const char* correo, int ciclo, const char* carrera, int codigo){
        this->id = id;
        strcpy(this->nombre, nombre);
        strcpy(this->correo, correo);
        this->ciclo = ciclo;
        strcpy(this->carrera, carrera);
        this->codigo = codigo;
    }
    void display() const {
    std::cout << "ID: " << id << std::endl;
    std::cout << "Nombre: " << nombre << std::endl;
    std::cout << "correo: " << correo << std::endl;
    std::cout << "Ciclo: " << ciclo << std::endl;
    std::cout << "Carrera: " << carrera << std::endl;
    std::cout << "Codigo: " << codigo << std::endl;
}

    ~RecordSequential(){}

};


bool operator<(const RecordSequential& p1, const RecordSequential& p2) {
    return p1.id < p2.id;
}

std::vector<RecordSequential> readCSV(const std::string &filename) {
    std::ifstream infile(filename);
    std::string line;
    std::vector<RecordSequential> records;

    while (std::getline(infile, line)) {
        std::stringstream ss(line);
        std::string token;
        std::vector<std::string> tokens;

        while (std::getline(ss, token, ',')) {
            tokens.push_back(token);
        }

        long id = std::stol(tokens[0]);
        int ciclo = std::stoi(tokens[3]);
        int codigo = std::stoi(tokens[5]);

        RecordSequential record(id, tokens[1].c_str(), tokens[2].c_str(), ciclo, tokens[4].c_str(), codigo);
        records.push_back(record);
    }

    return records;
}
void writeRecordSequential(std::ofstream &out, const RecordSequential &record) {
    out.write((char *)&record, sizeof(RecordSequential));
}

bool readRecordSequential(std::ifstream &in, RecordSequential &record) {
    in.read((char *)&record, sizeof(RecordSequential));
    return in.gcount() == sizeof(RecordSequential);
}
void mergeFilesSequential(const std::string &mainFilename, const std::string &auxFilename) {
    std::ifstream mainIn(mainFilename, std::ios::binary);
    std::ifstream auxIn(auxFilename, std::ios::binary);
    std::ofstream mergedOut("merged.dat", std::ios::binary);

    RecordSequential mainRecord, auxRecord;
    bool mainEnd = !readRecordSequential(mainIn, mainRecord);
    bool auxEnd = !readRecordSequential(auxIn, auxRecord);

    while (!mainEnd || !auxEnd) {
        if (mainEnd) {
            writeRecordSequential(mergedOut, auxRecord);
            auxEnd = !readRecordSequential(auxIn, auxRecord);
        } else if (auxEnd) {
            writeRecordSequential(mergedOut, mainRecord);
            mainEnd = !readRecordSequential(mainIn, mainRecord);
        } else {
            if (mainRecord < auxRecord) {
                writeRecordSequential(mergedOut, mainRecord);
                mainEnd = !readRecordSequential(mainIn, mainRecord);
            } else {
                writeRecordSequential(mergedOut, auxRecord);
                auxEnd = !readRecordSequential(auxIn, auxRecord);
            }
        }
    }

    mainIn.close();
    auxIn.close();
    mergedOut.close();

    std::remove(mainFilename.c_str());
    std::remove(auxFilename.c_str());
    std::rename("merged.dat", mainFilename.c_str());
}
long findMaxId(const std::string &mainFilename) {
    std::ifstream mainIn(mainFilename, std::ios::binary);
    RecordSequential currentRecord;
    long maxId = -1;

    while (readRecordSequential(mainIn, currentRecord)) {
        if (currentRecord.id > maxId) {
            maxId = currentRecord.id;
        }
    }

    mainIn.close();
    return maxId;
}

RecordSequential *searchRecordSequential(long id, const std::string &mainFilename, const std::string &auxFilename) {
    std::ifstream mainIn(mainFilename, std::ios::binary);
    std::ifstream auxIn(auxFilename, std::ios::binary);

    RecordSequential currentRecord;

    // Search in the auxiliary file first
    while (readRecordSequential(auxIn, currentRecord)) {
        if (currentRecord.id == id) {
            auxIn.close();
            mainIn.close();
            return new RecordSequential(currentRecord);
        }
    }

    // If not found in the auxiliary file, perform binary search in the main file
    long left = 0;
    long fileSize = mainIn.tellg();
    long right = fileSize / sizeof(RecordSequential) - 1;
    long mid;

    while (left <= right) {
        mid = left + (right - left) / 2;

        mainIn.seekg(mid * sizeof(RecordSequential), std::ios::beg);
        readRecordSequential(mainIn, currentRecord);

        if (currentRecord.id == id) {
            mainIn.close();
            auxIn.close();
            return new RecordSequential(currentRecord);
        }

        if (currentRecord.id < id) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }

    mainIn.close();
    auxIn.close();
    return nullptr;
}


vector<RecordSequential> searchSequential(long id, const std::string &mainFilename, const std::string &auxFilename) {
std::ifstream mainIn(mainFilename, std::ios::binary);
    std::ifstream auxIn(auxFilename, std::ios::binary);
    vector<RecordSequential> result;
    RecordSequential currentRecord;

    // Search in the auxiliary file
    while (readRecordSequential(auxIn, currentRecord)) {
        if (currentRecord.id == id  && !currentRecord.deleted) {
            result.push_back(currentRecord);
        }
    }

    // Binary search on the main file
    mainIn.seekg(0, std::ios::end);
    long fileSize = mainIn.tellg();
    long left = 0;
    long right = fileSize / sizeof(RecordSequential) - 1;

    while (left <= right) {
        long middle = left + (right - left) / 2;
        mainIn.seekg(middle * sizeof(RecordSequential), std::ios::beg);
        readRecordSequential(mainIn, currentRecord);

        if (currentRecord.id == id) {
            result.push_back(currentRecord);

            // Check for duplicates on the left side
            long leftDuplicate = middle - 1;
            while (leftDuplicate >= left) {
                mainIn.seekg(leftDuplicate * sizeof(RecordSequential), std::ios::beg);
                readRecordSequential(mainIn, currentRecord);
                if (currentRecord.id == id  && !currentRecord.deleted) {
                    result.push_back(currentRecord);
                } else {
                    break;
                }
                leftDuplicate--;
            }

            // Check for duplicates on the right side
            long rightDuplicate = middle + 1;
            while (rightDuplicate <= right) {
                mainIn.seekg(rightDuplicate * sizeof(RecordSequential), std::ios::beg);
                readRecordSequential(mainIn, currentRecord);
                if (currentRecord.id == id) {
                    result.push_back(currentRecord);
                } else {
                    break;
                }
                rightDuplicate++;
            }

            break;
        } else if (currentRecord.id < id) {
            left = middle + 1;
        } else {
            right = middle - 1;
        }
    }

    mainIn.close();
    auxIn.close();
    return result;



}

vector<RecordSequential> searchRangeSequential(long beginId, long endId, const std::string &mainFilename, const std::string &auxFilename) {
    vector<RecordSequential> results;
    RecordSequential currentRecord;

    // Search in the auxiliary file first
    std::ifstream auxIn(auxFilename, std::ios::binary);
    while (readRecordSequential(auxIn, currentRecord)) {
        if (currentRecord.id >= beginId && currentRecord.id <= endId && !currentRecord.deleted) {
            results.push_back(currentRecord);
        }
    }
    auxIn.close();

    // Search in the main file using binary search
    std::ifstream mainIn(mainFilename, std::ios::binary);
    long begin = 0;
    long mid;
    mainIn.seekg(0, std::ios::end);
    long end = mainIn.tellg() / sizeof(RecordSequential) - 1;

    while (begin <= end) {
        mid = begin + (end - begin) / 2;
        mainIn.seekg(mid * sizeof(RecordSequential));
        readRecordSequential(mainIn, currentRecord);

        if (currentRecord.id >= beginId && currentRecord.id <= endId && !currentRecord.deleted) {
            results.push_back(currentRecord);
            // Search for records with the same ID in the left half
            long left = mid - 1;
            while (left >= begin) {
                mainIn.seekg(left * sizeof(RecordSequential));
                readRecordSequential(mainIn, currentRecord);
                if (currentRecord.id >= beginId && currentRecord.id <= endId && !currentRecord.deleted) {
                    results.push_back(currentRecord);
                } else {
                    break;
                }
                left--;
            }

            // Search for records with the same ID in the right half
            long right = mid + 1;
            while (right <= end) {
                mainIn.seekg(right * sizeof(RecordSequential));
                readRecordSequential(mainIn, currentRecord);
                if (currentRecord.id >= beginId && currentRecord.id <= endId && !currentRecord.deleted) {
                    results.push_back(currentRecord);
                } else {
                    break;
                }
                right++;
            }
            break;
        }

        if (currentRecord.id < beginId) {
            begin = mid + 1;
        } else {
            end = mid - 1;
        }
    }

    mainIn.close();
    return results;
}


void insertRecordSequential(RecordSequential record, const std::string mainFilename, const std::string auxFilename, int K) {
    long maxId = findMaxId(mainFilename);
    bool recordExists = searchRecordSequential(record.id, mainFilename, auxFilename) != nullptr;

    if (recordExists && record.id < maxId) {
        std::ofstream auxOut(auxFilename, std::ios::binary | std::ios::app);
        writeRecordSequential(auxOut, record);
        auxOut.close();

        if (++K >= 2) {
            mergeFilesSequential(mainFilename, auxFilename);
            K = 0;
        }
    } else {
        std::ifstream mainIn(mainFilename, std::ios::binary);
        std::ofstream mainOut("temp.dat", std::ios::binary);

        RecordSequential currentRecord;
        bool inserted = false;

        while (readRecordSequential(mainIn, currentRecord)) {
            if (!inserted && record < currentRecord) {
                writeRecordSequential(mainOut, record);
                inserted = true;
            }
            writeRecordSequential(mainOut, currentRecord);
        }

        if (!inserted) {
            writeRecordSequential(mainOut, record);
        }

        mainIn.close();
        mainOut.close();

        std::remove(mainFilename.c_str());
        std::rename("temp.dat", mainFilename.c_str());
    }
}



void deleteRecordSequential(long id, const std::string &mainFilename, const std::string &auxFilename) {
    std::ifstream mainIn(mainFilename, std::ios::binary);
    std::ofstream mainOut("temp.dat", std::ios::binary);

    vector<RecordSequential> recordsToDelete = searchSequential(id, mainFilename, auxFilename);
    if (recordsToDelete.empty()) {
        std::cout << "RecordSequential not found." << std::endl;
        mainIn.close();
        mainOut.close();
        return;
    }

    RecordSequential currentRecord;
    bool found = false;

    while (readRecordSequential(mainIn, currentRecord)) {
        bool shouldWrite = true;
        for (const auto &recordToDelete : recordsToDelete) {
            if (currentRecord.id == recordToDelete.id) {
                shouldWrite = false;
                found = true;
                currentRecord.deleted = true;
                break;
            }
        }

        if (shouldWrite) {
            writeRecordSequential(mainOut, currentRecord);
        }
    }

    mainIn.close();
    mainOut.close();

    std::remove(mainFilename.c_str());
    std::rename("temp.dat", mainFilename.c_str());

    if (!found) {
        std::cout << "RecordSequential not found." << std::endl;
    } else {
        std::cout << "RecordSequential(s) deleted." << std::endl;
    }
}
void displayAllRecordsSequential(const std::string &mainFilename) {
    std::ifstream mainIn(mainFilename, std::ios::binary);
    RecordSequential currentRecord;

    while (readRecordSequential(mainIn, currentRecord)) {
        currentRecord.display();
        std::cout << "------------------------" << std::endl;
    }

    mainIn.close();
}

class Interfaz
{
private:
    string metodo;
    string archivoDatos;
    ExtendibleHash eHash;
public:
    void leerConsulta()
    {
        cout << "Ingrese consulta SQL:" << endl;
        cin.ignore();

        string cadenaEntera;
        getline(cin, cadenaEntera);
        string consulta;
        string delimitador = "-"; // nuestro delimitador de cada fila de la consulta será el punto y coma
        vector<string> palabras{};

        size_t pos = 0;
        while ((pos = cadenaEntera.find(delimitador)) != string::npos)
        {
            palabras.push_back(cadenaEntera.substr(0, pos));
            // Por cada iteracion se borra desde la pos 0 del string hasta
            // que se llegue a un delimitador
            cadenaEntera.erase(0, pos + delimitador.length());
        }
        // Se colocan en el vector de palabras solo las filas introducidas
        //(ya que el delimitador es \n)
        palabras.push_back(cadenaEntera);

        string fileName = palabras[0];

        // Busco el parentesis de inicio del nombre del archivo
        int posicion = fileName.find("(");

        fileName = fileName.substr(posicion);

        // Me quedo solo con el nombre del archivo en filename
        fileName = fileName.substr(2, fileName.size() - 4);
        this->archivoDatos = fileName;

        // Lee la parte de la consulta donde mencionamos el índice que debe utilizar
        if (palabras[1].substr(palabras[1].size() - 5, palabras[1].size() - 2) == "hash;")
        {
            metodo = "hash";
            ExtendibleHash eHash("Indice.bin", this->archivoDatos);
            cout << "Cargando datos desde " << this->archivoDatos << "..." << endl;
            eHash.LoadCsv(this->archivoDatos);
            cout << "Datos cargados exitosamente." << endl;
        }
        else
        {
            metodo = "sequential";

            // FALTA Insertar todos los datos del csv al sequential
            cout << "Cargando datos desde " << this->archivoDatos << "..." << endl;
            std::string mainFilename = "main.dat";
            std::string auxFilename = "aux.dat";
            int K = 0;
            long beginId, endId;
            if (!std::filesystem::exists(mainFilename)) {
                std::vector<RecordSequential> records = readCSV(this->archivoDatos);
                for (const auto &record : records) {
                    insertRecordSequential(record, mainFilename, auxFilename, K);
                }
            }
            cout << "Datos cargados exitosamente." << endl;
        }

        // Imprimir todos los records del csv
        cout << "Resultados de la consulta:" << endl;
        ifstream file2;
        file2.open(archivoDatos, ios::in | ios::binary);
        file2.seekg(0, ios::end);
        long int final = file2.tellg();
        file2.seekg(0, ios::beg);
        while (file2.tellg() < final)
        {
            Bucket b;
            file2.read((char *)&b, sizeof(Bucket));
            cout << "Bucket: " << b.binario << endl;
            for (int i = 0; i < b.nRecords; i++)
            {
                cout << b.records[i].id << " ";
            }
            cout << endl;
        }
        file2.close();
    }

    // IMPORTANTE: Todos estos métodos tienen que imprimir los records resultantes para evitar tener que hacer una consulta de
    // selección después de cada operación

    void search()
    {
        long int id;
        cout << "Ingrese el id:";
        cin >> id;
        if (metodo == "hash")
        {
            eHash.searchRecord(id);
        }
        else
        {
            searchSequential(id, "FALTA-DEFINIR-ARCHIVO-QUE-ESCRIBE", "FALTA-DEFINIR-ARCHIVO-AUXILIAR");
        }
    }

    void insert()
    {
        Record record;
        RecordSequential recordSequential;
        cout << "Ingrese el id:";
        cin >> record.id;
        cin >> recordSequential.id;
        cout << "Ingrese el nombre:";
        cin.ignore();
        getline(cin, record.nombre);
        getline(cin, recordSequential.nombre);
        cout << "Ingrese el correo:";
        getline(cin, record.correo);
        getline(cin, recordSequential.correo);
        cout << "Ingrese el ciclo:";
        cin >> record.ciclo;
        cin >> recordSequential.ciclo;
        cout << "Ingrese la carrera:";
        cin.ignore();
        getline(cin, record.carrera);
        getline(cin, recordSequential.carrera);
        cout << "Ingrese el codigo:";
        cin >> record.codigo;
        cin >> recordSequential.codigo;

        if (metodo == "hash")
        {
            eHash.insertRecord(record);
        }
        else
        {
            //Record record, const std::string &mainFilename, const std::string &auxFilename, int &K
            insertRecordSequential(recordSequential, "FALTA-DEFINIR-ARCHIVO-QUE-ESCRIBE", "FALTA-DEFINIR-ARCHIVO-AUXILIAR", 0);
        }
    }

    void rangeSearch()
    {
        cout << "Ingrese el id inicial:";
        long int beginId;
        cin >> beginId;
        cout << "Ingrese el id final:";
        long int endId;
        cin >> endId;

        //long beginId, long endId, const std::string &mainFilename, const std::string &auxFilename
        searchRangeSequential(beginId, endId, "FALTA-DEFINIR-ARCHIVO-QUE-ESCRIBE", "FALTA-DEFINIR-ARCHIVO-AUXILIAR");
    }

    void deleteRecord(long int id)
    {
        if (metodo == "hash")
        {
            eHash.deleteRecord(id);
        }
        else
        {
            deleteRecordSequential(id, "FALTA-DEFINIR-ARCHIVO-QUE-ESCRIBE", "FALTA-DEFINIR-ARCHIVO-AUXILIAR");
        }
    }
};