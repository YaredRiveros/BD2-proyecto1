#include <iostream>
#include <fstream>
#include <string.h>
#include <sstream>
#include <vector>


using namespace std;
struct Record{
    long int id;
    char nombre[20];
    char apellido[20];
    int ciclo;
    char carrera[30];
    int codigo;

    public:
    Record(){
        strcpy(nombre, "");
        strcpy(apellido, "");
        ciclo = 0;
        strcpy(carrera, "");
        codigo = 0;
    }

    Record(long int id, const char* nombre, const char* apellido, int ciclo, const char* carrera, int codigo){
        this->id = id;
        strcpy(this->nombre, nombre);
        strcpy(this->apellido, apellido);
        this->ciclo = ciclo;
        strcpy(this->carrera, carrera);
        this->codigo = codigo;
    }
    void display() const {
    std::cout << "ID: " << id << std::endl;
    std::cout << "Nombre: " << nombre << std::endl;
    std::cout << "Apellido: " << apellido << std::endl;
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
void mergeFiles(const std::string &mainFilename, const std::string &auxFilename) {
    std::ifstream mainIn(mainFilename, std::ios::binary);
    std::ifstream auxIn(auxFilename, std::ios::binary);
    std::ofstream mergedOut("merged.dat", std::ios::binary);

    Record mainRecord, auxRecord;
    bool mainEnd = !readRecord(mainIn, mainRecord);
    bool auxEnd = !readRecord(auxIn, auxRecord);

    while (!mainEnd || !auxEnd) {
        if (mainEnd) {
            writeRecord(mergedOut, auxRecord);
            auxEnd = !readRecord(auxIn, auxRecord);
        } else if (auxEnd) {
            writeRecord(mergedOut, mainRecord);
            mainEnd = !readRecord(mainIn, mainRecord);
        } else {
            if (mainRecord < auxRecord) {
                writeRecord(mergedOut, mainRecord);
                mainEnd = !readRecord(mainIn, mainRecord);
            } else {
                writeRecord(mergedOut, auxRecord);
                auxEnd = !readRecord(auxIn, auxRecord);
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

void insertRecord(const Record &record, const std::string &mainFilename, const std::string &auxFilename) {
    std::ifstream mainIn(mainFilename, std::ios::binary);
    std::ofstream mainOut("temp.dat", std::ios::binary);
    std::ofstream auxOut(auxFilename, std::ios::binary | std::ios::app);

    Record currentRecord;
    bool inserted = false;

    while (readRecord(mainIn, currentRecord)) {
        if (!inserted && record < currentRecord) {
            writeRecord(mainOut, record);
            inserted = true;
        }
        writeRecord(mainOut, currentRecord);
    }

    if (!inserted) {
        writeRecord(auxOut, record);
    }

    mainIn.close();
    mainOut.close();
    auxOut.close();

    std::remove(mainFilename.c_str());
    std::rename("temp.dat", mainFilename.c_str());
    mergeFiles(mainFilename, auxFilename);
}


Record *searchRecord(long id, const std::string &mainFilename, const std::string &auxFilename) {
    std::ifstream mainIn(mainFilename, std::ios::binary);
    std::ifstream auxIn(auxFilename, std::ios::binary);

    Record currentRecord;

    while (readRecord(mainIn, currentRecord)) {
        if (currentRecord.id == id) {
            mainIn.close();
            auxIn.close();
            return new Record(currentRecord);
        }
    }

    while (readRecord(auxIn, currentRecord)) {
        if (currentRecord.id == id) {
            mainIn.close();
            auxIn.close();
            return new Record(currentRecord);
        }
    }

    mainIn.close();
    auxIn.close();
    return nullptr;
}
void deleteRecord(long id, const std::string &mainFilename, const std::string &auxFilename) {
    std::ifstream mainIn(mainFilename, std::ios::binary);
    std::ofstream mainOut("temp.dat", std::ios::binary);

    Record currentRecord;
    bool found = false;

    while (readRecord(mainIn, currentRecord)) {
        if (currentRecord.id == id) {
            found = true;
        } else {
            writeRecord(mainOut, currentRecord);
        }
    }

    mainIn.close();
    mainOut.close();

    std::remove(mainFilename.c_str());
    std::rename("temp.dat", mainFilename.c_str());

    if (!found) {
        std::cout << "Record not found." << std::endl;
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

    std::vector<Record> records = readCSV("data.csv");
    for (const auto &record : records) {
        insertRecord(record, mainFilename, auxFilename);
    }

    while (true) {
        std::cout << "1. Search" << std::endl;
        std::cout << "2. Delete" << std::endl;
        std::cout << "3. Merge files" << std::endl;
        std::cout << "4. Insert record" << std::endl; // Add this line
        std::cout << "5. Display all records" << std::endl; // Add this line
        std::cout << "6. Exit" << std::endl; // Update this line

        int choice;
        std::cin >> choice;

        switch (choice) {
            case 1: {
                long id;
                std::cout << "Enter the ID to search: ";
                std::cin >> id;

                Record *record = searchRecord(id, mainFilename, auxFilename);
                if (record) {
                    // Display the record details
                    record->display();
                    delete record;
                } else {
                    std::cout << "Record not found." << std::endl;
                }
                break;
            }
            case 2: {
                long id;
                std::cout << "Enter the ID to delete: ";
                std::cin >> id;
                deleteRecord(id, mainFilename, auxFilename);
                break;
            }
            case 3: {
                mergeFiles(mainFilename, auxFilename);
                break;
            }
            case 4:{
                long id;
        char nombre[20];
        char apellido[20];
        int ciclo;
        char carrera[30];
        int codigo;

        std::cout << "Enter ID: ";
        std::cin >> id;
        std::cout << "Enter nombre: ";
        std::cin.ignore();
        std::cin.getline(nombre, 20);
        std::cout << "Enter apellido: ";
        std::cin.getline(apellido, 20);
        std::cout << "Enter ciclo: ";
        std::cin >> ciclo;
        std::cout << "Enter carrera: ";
        std::cin.ignore();
        std::cin.getline(carrera, 30);
        std::cout << "Enter codigo: ";
        std::cin >> codigo;

        Record record(id, nombre, apellido, ciclo, carrera, codigo);
        insertRecord(record, mainFilename, auxFilename);
        break;}
        case 5: {
        displayAllRecords(mainFilename);
        break;
    }
    case 6:
        return 0;
            default:
                std::cout << "Invalid choice." << std::endl;
        }
    }
}
