#include <iostream>
#include <fstream>
#include <string.h>
#include <sstream>
#include <vector>
using namespace std;
class Record {
    long int id;
    char nombre[20];
    char apellido[20];
    int ciclo;
    char carrera[30];
    int codigo;
    bool deleted;

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
        this -> deleted = false;
    }

    ~Record(){}

    long int return_Id(){ return id; }
    void get_Data();
    void put_Data() const;
    bool is_Deleted(){ return deleted; }
    void set_Deleted(bool d){ deleted = d; }

};

void Record::get_Data()
{
    cout << "\nIngrese datos del estudiante";
    cout << "\nID: ";
    cin >> id;
    cin.ignore();
    cout << "\nNombre: ";
    cin.getline(nombre, sizeof(nombre));
    cout << "\nApellido: ";
    cin.getline(apellido, sizeof(apellido));
    cout << "\nCiclo: ";
    cin >> ciclo;
    cin.ignore();
    cout << "\nCarrera: ";
    cin.getline(carrera, sizeof(carrera));
    cout << "\nCodigo: ";
    cin >> codigo;
}

void Record::put_Data() const
{
    cout<<"\n"<<id<<"\t"<<nombre<<"\t"<<apellido<<"\t"<<ciclo<<"\t"<<carrera<<"\t"<<codigo;
}
