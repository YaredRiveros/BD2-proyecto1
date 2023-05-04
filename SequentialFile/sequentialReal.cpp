#include <iostream>
#include <fstream>
#include <string.h>
#include <sstream>
#include <vector>
#include <filesystem>
#include <algorithm>


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


void insertRecordSequential(const RecordSequential &record, const std::string &mainFilename, const std::string &auxFilename, int &K) {
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

int main() {
    std::string mainFilename = "main.dat";
    std::string auxFilename = "aux.dat";
    int K = 0;
    long beginId, endId;
    std::vector<RecordSequential> records = readCSV("data.csv");
        for (const auto &record : records) {
            insertRecordSequential(record, mainFilename, auxFilename, K);
        }

    while (true) {
        std::cout << "1. Buscar" << std::endl;
        std::cout << "2. Eliminar" << std::endl;
        std::cout << "3. Fusionar archivos" << std::endl;
        std::cout << "4. Insertar registro" << std::endl;
        std::cout << "5. Mostrar todos los registros" << std::endl;
        std::cout << "6. Buscar por rango" << std::endl;
        std::cout << "7. Salir" << std::endl;
        std::cout << "Ingrese una opción: ";
        int choice;
        std::cin >> choice;

        switch (choice) {
            case 1: {
                long id;
                std::cout << "Ingrese el ID: ";
                std::cin >> id;

                 vector<RecordSequential> records = searchSequential(id, mainFilename, auxFilename);
                  if (!records.empty()) {
                      for (const auto &record : records) {
                          record.display();
                          std::cout << std::endl; // Separate records with a blank line
                      }
                  } else {
                      std::cout << "Registro no encontrado." << std::endl;
                  }
                  break;
            }
            case 2: {
                long id;
                std::cout << "Ingrese el ID para eliminar: ";
                std::cin >> id;
                deleteRecordSequential(id, mainFilename, auxFilename);
                break;
            }
            case 3: {
                mergeFilesSequential(mainFilename, auxFilename);
                break;
            }
            case 4:{
                long id;
                char nombre[20];
                char correo[20];
                int ciclo;
                char carrera[30];
                int codigo;

                std::cout << "Ingrese el ID: ";
                std::cin >> id;
                std::cout <<  "Ingrese el nombre: ";
                std::cin.ignore();
                std::cin.getline(nombre, 20);
                std::cout << "Ingrese el correo: ";
                std::cin.getline(correo, 20);
                std::cout <<"Ingrese el ciclo: ";
                std::cin >> ciclo;
                std::cout << "Ingrese la carrera: ";
                std::cin.ignore();
                std::cin.getline(carrera, 30);
                std::cout << "Ingrese el codigo: ";
                std::cin >> codigo;

        RecordSequential record(id, nombre, correo, ciclo, carrera, codigo);
        insertRecordSequential(record, mainFilename, auxFilename,K);
        break;}
        case 5: {
        displayAllRecordsSequential(mainFilename);
        break;
    }
    case 6:{
       std::cout << "Ingrese el ID inicial: ";
                std::cin >> beginId;
                std::cout << "Ingrese el ID final: ";
                std::cin >> endId;
                std::vector<RecordSequential> records = searchRangeSequential(beginId, endId, mainFilename, auxFilename);
                // Sort the records by their ID
                std::sort(records.begin(), records.end(), [](const RecordSequential &a, const RecordSequential &b) {
                    return a.id < b.id;
                });
                for (const auto &record : records) {
                    record.display();
                    std::cout << "------------------------" << std::endl;
                }
                break;
    }
    
    case 7 :
        return 0;
            default:
                std::cout << "Opción inválida." << std::endl;
        }
    }
}
