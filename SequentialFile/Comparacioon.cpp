#include <iostream>
#include <fstream>
#include <string.h>
#include <sstream>
#include <vector>
#include <filesystem>
#include <chrono>


using namespace std;
struct Record{
    long int id;
    char nombre[20];
    char correo[20];
    int ciclo;
    char carrera[30];
    int codigo;

    public:
    bool deleted = false;
    Record(){
        strcpy(nombre, "");
        strcpy(correo, "");
        ciclo = 0;
        strcpy(carrera, "");
        codigo = 0;
        
    }

    Record(long int id, const char* nombre, const char* correo, int ciclo, const char* carrera, int codigo){
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

    ~Record(){}

};


bool operator<(const Record& p1, const Record& p2) {
    return p1.id < p2.id;
}

std::vector<Record> readCSV(const std::string &filename) {
    std::ifstream infile(filename);
    std::string line;
    std::vector<Record> records;

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

        Record record(id, tokens[1].c_str(), tokens[2].c_str(), ciclo, tokens[4].c_str(), codigo);
        records.push_back(record);
    }

    return records;
}
void writeRecord(std::ofstream &out, const Record &record) {
    out.write((char *)&record, sizeof(Record));
}

bool readRecord(std::ifstream &in, Record &record) {
    in.read((char *)&record, sizeof(Record));
    return in.gcount() == sizeof(Record);
}
void mergeFiles(const std::string &mainFilename, const std::string &auxFilename,int &cont) {
    std::ifstream mainIn(mainFilename, std::ios::binary);
    std::ifstream auxIn(auxFilename, std::ios::binary);
    std::ofstream mergedOut("merged.dat", std::ios::binary);

    Record mainRecord, auxRecord;
    bool mainEnd = !readRecord(mainIn, mainRecord);
    bool auxEnd = !readRecord(auxIn, auxRecord);
    cont += 2;

    while (!mainEnd || !auxEnd) {
        if (mainEnd) {
            writeRecord(mergedOut, auxRecord);
            auxEnd = !readRecord(auxIn, auxRecord);
            cont+=2;
        } else if (auxEnd) {
            writeRecord(mergedOut, mainRecord);
            mainEnd = !readRecord(mainIn, mainRecord);
            cont+=2;
        } else {
            if (mainRecord < auxRecord) {
                writeRecord(mergedOut, mainRecord);
                mainEnd = !readRecord(mainIn, mainRecord);
                cont+=2;
            } else {
                writeRecord(mergedOut, auxRecord);
                auxEnd = !readRecord(auxIn, auxRecord);
                cont+=2;
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
    Record currentRecord;
    long maxId = -1;

    while (readRecord(mainIn, currentRecord)) {
        if (currentRecord.id > maxId) {
            maxId = currentRecord.id;
        }
    }

    mainIn.close();
    return maxId;
}

Record *searchRecord(long id, const std::string &mainFilename, const std::string &auxFilename, int &count) {
    std::ifstream mainIn(mainFilename, std::ios::binary);
    std::ifstream auxIn(auxFilename, std::ios::binary);

    Record currentRecord;

    // Search in the auxiliary file first
    while (readRecord(auxIn, currentRecord)) {
        count++;
        if (currentRecord.id == id) {
            auxIn.close();
            mainIn.close();
            return new Record(currentRecord);
        }
    }

    // If not found in the auxiliary file, perform binary search in the main file
    long left = 0;
    long fileSize = mainIn.tellg();
    long right = fileSize / sizeof(Record) - 1;
    long mid;

    while (left <= right) {
        mid = left + (right - left) / 2;

        mainIn.seekg(mid * sizeof(Record), std::ios::beg);
        count++;
        readRecord(mainIn, currentRecord);

        if (currentRecord.id == id) {
            mainIn.close();
            auxIn.close();
            return new Record(currentRecord);
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


vector<Record> search(long id, const std::string &mainFilename, const std::string &auxFilename,int &count) {
std::ifstream mainIn(mainFilename, std::ios::binary);
    std::ifstream auxIn(auxFilename, std::ios::binary);
    vector<Record> result;
    Record currentRecord;

    // Search in the auxiliary file
    while (readRecord(auxIn, currentRecord)) {
        count++;
        if (currentRecord.id == id  && !currentRecord.deleted) {
            result.push_back(currentRecord);
        }
    }

    // Binary search on the main file
    mainIn.seekg(0, std::ios::end);
    long fileSize = mainIn.tellg();
    long left = 0;
    long right = fileSize / sizeof(Record) - 1;

    while (left <= right) {
        long middle = left + (right - left) / 2;
        mainIn.seekg(middle * sizeof(Record), std::ios::beg);
        count++;
        readRecord(mainIn, currentRecord);

        if (currentRecord.id == id) {
            result.push_back(currentRecord);

            // Check for duplicates on the left side
            long leftDuplicate = middle - 1;
            while (leftDuplicate >= left) {
                mainIn.seekg(leftDuplicate * sizeof(Record), std::ios::beg);
                count++;
                readRecord(mainIn, currentRecord);
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
                mainIn.seekg(rightDuplicate * sizeof(Record), std::ios::beg);
                count++;
                readRecord(mainIn, currentRecord);
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

vector<Record> searchRange(long beginId, long endId, const std::string &mainFilename, const std::string &auxFilename) {
    vector<Record> results;
    Record currentRecord;

    // Search in the auxiliary file first
    std::ifstream auxIn(auxFilename, std::ios::binary);
    while (readRecord(auxIn, currentRecord)) {
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
    long end = mainIn.tellg() / sizeof(Record) - 1;

    while (begin <= end) {
        mid = begin + (end - begin) / 2;
        mainIn.seekg(mid * sizeof(Record));
        readRecord(mainIn, currentRecord);

        if (currentRecord.id >= beginId && currentRecord.id <= endId && !currentRecord.deleted) {
            results.push_back(currentRecord);
            // Search for records with the same ID in the left half
            long left = mid - 1;
            while (left >= begin) {
                mainIn.seekg(left * sizeof(Record));
                readRecord(mainIn, currentRecord);
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
                mainIn.seekg(right * sizeof(Record));
                readRecord(mainIn, currentRecord);
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


void insertRecord(const Record &record, const std::string &mainFilename, const std::string &auxFilename, int &K,int &cont) {
    long maxId = findMaxId(mainFilename);
    bool recordExists = searchRecord(record.id, mainFilename, auxFilename,cont) != nullptr;

    if (recordExists && record.id < maxId) {
        std::ofstream auxOut(auxFilename, std::ios::binary | std::ios::app);
        cont++;
        writeRecord(auxOut, record);
        auxOut.close();

        if (++K >= 2) {
            mergeFiles(mainFilename, auxFilename,cont);
            K = 0;
        }
    } else {
        std::ifstream mainIn(mainFilename, std::ios::binary);
        std::ofstream mainOut("temp.dat", std::ios::binary);

        Record currentRecord;
        bool inserted = false;

        while (readRecord(mainIn, currentRecord)) {
            if (!inserted && record < currentRecord) {
                cont++;
                writeRecord(mainOut, record);
                inserted = true;
            }
            cont++;
            writeRecord(mainOut, currentRecord);
        }

        if (!inserted) {
            cont++;
            writeRecord(mainOut, record);
        }

        mainIn.close();
        mainOut.close();

        std::remove(mainFilename.c_str());
        std::rename("temp.dat", mainFilename.c_str());
    }
}



void deleteRecord(long id, const std::string &mainFilename, const std::string &auxFilename) {
    std::ifstream mainIn(mainFilename, std::ios::binary);
    std::ofstream mainOut("temp.dat", std::ios::binary);
    int cont=0;
    vector<Record> recordsToDelete = search(id, mainFilename, auxFilename,cont);
    if (recordsToDelete.empty()) {
        std::cout << "Record not found." << std::endl;
        mainIn.close();
        mainOut.close();
        return;
    }

    Record currentRecord;
    bool found = false;

    while (readRecord(mainIn, currentRecord)) {
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
            writeRecord(mainOut, currentRecord);
        }
    }

    mainIn.close();
    mainOut.close();

    std::remove(mainFilename.c_str());
    std::rename("temp.dat", mainFilename.c_str());

    if (!found) {
        std::cout << "Record not found." << std::endl;
    } else {
        std::cout << "Record(s) deleted." << std::endl;
    }
}
void displayAllRecords(const std::string &mainFilename) {
    std::ifstream mainIn(mainFilename, std::ios::binary);
    Record currentRecord;

    while (readRecord(mainIn, currentRecord)) {
        currentRecord.display();
        std::cout << "------------------------" << std::endl;
    }

    mainIn.close();
}

int main() {
    std::string mainFilename = "main.dat";
    std::string auxFilename = "aux.dat";
    int K = 0;
    long beginId, endId;

    cout << "--------------Inserción--------------" << endl;

    //Inserción
    chrono::time_point<std::chrono::system_clock>inicio,fin;
    inicio = chrono::high_resolution_clock::now();
    // cout << "Iniciando inserción..." << endl;
    // std::vector<Record> records = readCSV("DataBase.csv");
    // int cont = 0;
    // for (const auto &record : records) {
    //     insertRecord(record, mainFilename, auxFilename, K,cont);
    // }
    // cout << "Inserción finalizada" << endl;
    fin = chrono::high_resolution_clock::now();

    // cout << "Número de accesos a memoria secundaria: " << cont << endl;

    std::chrono::duration<double,std::milli>t=fin-inicio;
    cout << "Tiempo Inserción: " << t.count() << " ms"<<endl;

    cout << "--------------Búsqueda--------------" << endl;

    //Búsqueda
    int count = 0;
    int total = 0;
    inicio = chrono::high_resolution_clock::now();
    cout << "Iniciando búsqueda..." << endl;

    std::vector<Record> recordsB = search(1, mainFilename, auxFilename,count);
    for (const auto &record : recordsB) {
        record.display();
        std::cout << "------------------------" << std::endl;
    }

    total += count;
    count = 0;

    recordsB = search(9, mainFilename, auxFilename,count);
    for (const auto &record : recordsB) {
        record.display();
        std::cout << "------------------------" << std::endl;
    }

    total += count;
    count = 0;

    recordsB = search(19, mainFilename, auxFilename,count);
    for (const auto &record : recordsB) {
        record.display();
        std::cout << "------------------------" << std::endl;
    }

    total += count;
    count = 0;

    recordsB = search(39, mainFilename, auxFilename,count);
    for (const auto &record : recordsB) {
        record.display();
        std::cout << "------------------------" << std::endl;
    }

    total += count;
    count = 0;

    recordsB = search(78, mainFilename, auxFilename,count);
    for (const auto &record : recordsB) {
        record.display();
        std::cout << "------------------------" << std::endl;
    }

    total += count;
    count = 0;

    recordsB = search(156, mainFilename, auxFilename,count);
    for (const auto &record : recordsB) {
        record.display();
        std::cout << "------------------------" << std::endl;
    }

    total += count;
    count = 0;

    recordsB = search(312, mainFilename, auxFilename,count);
    for (const auto &record : recordsB) {
        record.display();
        std::cout << "------------------------" << std::endl;
    }

    total += count;
    count = 0;

    recordsB = search(625, mainFilename, auxFilename,count);
    for (const auto &record : recordsB) {
        record.display();
        std::cout << "------------------------" << std::endl;
    }

    total += count;
    count = 0;

    recordsB = search(1250, mainFilename, auxFilename,count);
    for (const auto &record : recordsB) {
        record.display();
        std::cout << "------------------------" << std::endl;
    }

    total += count;
    count = 0;

    recordsB = search(2500, mainFilename, auxFilename,count);
    for (const auto &record : recordsB) {
        record.display();
        std::cout << "------------------------" << std::endl;
    }

    total += count;
    count = 0;

    recordsB = search(5000, mainFilename, auxFilename,count);
    for (const auto &record : recordsB) {
        record.display();
        std::cout << "------------------------" << std::endl;
    }

    total += count;

    fin = chrono::high_resolution_clock::now();
    cout << "Número de accesos a memoria secundaria: " << total << endl;
    t=fin-inicio;
    cout << "Tiempo Búsqueda: " << t.count() << " ms"<<endl;

}
