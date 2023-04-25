#include <iostream>
#include <string>
#include <map>
#include <cstring>
using namespace std;


//Componentes : 1. Bucket 2. HashFunction 3. HashIndex

struct Record{
    char name[40];
    char email[40];
    int ciclo;
    int code;
    char carrera[40];

    Record(char name[40], char email[40], int ciclo, int code, char carrera[40]){
        strcpy(this->name, name); // strcpy es para copiar strings
        strcpy(this->email, email);
        this->ciclo = ciclo;
        this->code = code;
        strcpy(this->carrera, carrera);
    }
};

class Bucket{
    int Ldepth; //profundidad local
    int size; //cantidad actual de registros
    Record *records; //array de records
    Bucket *next; //en caso de overflow
    int index; //indice del bucket
    string filename; //nombre del archivo

    // map<int, Record> records; Si utilizo map, faltaría el enlace al siguiente bucket
    public:
    Bucket(int Ldepth, int index, string filename);
    void insert(Record record){

    }
    bool isFull(int max){
        return size == max;
    }
    bool isEmpty(){
        return size == 0;
    }
};



class ExtendibleHash{
    int Gdepth; //profundidad global
    int max; //cantidad maxima de registros por bucket
    Bucket **buckets; //array de buckets para el hash
};
