#include <iostream>
#include <fstream>
#include <math.h>
#include <map>

using namespace std;

const int globalDepth = 3;
const int maxRecords = 4; //Cantidad máxima de registros por bucket

//No necesito clase Index porque solo necesito guardar la posición del bucket en records.bin


//funcion que convierte decimal a binario (string)
string decimalToBinary(long int n, int bits=globalDepth){
    string r;
    while(n!=0) {r=(n%2==0 ?"0":"1")+r; n/=2;}
    while(r.length()<bits) r = "0" + r;
    if (n==0){
        while(r.length()<bits) r = "0" + r;
    }
    return r;
}

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

    Record(long int id, char* nombre, char* apellido, int ciclo, char* carrera, int codigo){
        this->id = id;
        strcpy(this->nombre, nombre);
        strcpy(this->apellido, apellido);
        this->ciclo = ciclo;
        strcpy(this->carrera, carrera);
        this->codigo = codigo;
    }

    ~Record(){}

};

struct Bucket{
    //No necesita binario porque solo se necesita modificarlo en index.bin
    Record records[maxRecords];
    int nRecords;
    int localDepth;
    long int posNextBucket; //Bucket encadenados a este bucket
    string binario;

    public:
    Bucket(){
        localDepth = 1;
        posNextBucket = -1;
        nRecords = 0;
    }

    Bucket(int localDepth,string binario){
        this->localDepth = localDepth;
        posNextBucket = -1;
        nRecords = 0;
        this->binario = binario;
    }

    ~Bucket(){}

};

struct Indice{
    int pos;
    char binario[globalDepth];
    
    Indice(){
        pos = -1;
        strcpy(binario, "");
    }
    
    Indice(int pos, char* binario){
        this->pos = pos;
        strcpy(this->binario, binario);
    }
};

class ExtendibleHash{
    string indexFileName;
    string recordsFileName;

    public:
    ExtendibleHash(string indexFileName, string recordsFileName){
        this->indexFileName = indexFileName;
        this->recordsFileName = recordsFileName;

        //1. Genero el archivo de index
        ofstream indexFile(indexFileName, ios::out | ios::binary);
        if(!indexFile){
            cout << "Error al crear el archivo de index" << endl;
            return;
        }
        //2. Escribo los índices: al inicio solo hay 2 buckets, uno en la pos 0 y otro en la pos sizeof(long int)
        //Si el index.bin está vacío, se crean los 2 buckets
        if(indexFile.tellp() == 0){
            for(int i = 0; i < pow(2, globalDepth); i++){
                Indice index;
                index.pos = i*sizeof(Bucket);
                strcpy(index.binario, decimalToBinary(i).c_str()); //"0" -> "000"
                indexFile.write((char*)&index, sizeof(Indice));
            }
        }

        indexFile.close();
    }

    ~ExtendibleHash(){}

    void insertRecord(Record record){
        //Falta implementar un metodo hash, por ahora saco el modulo
        int hash = record.id % pow(2, globalDepth);
        //1. Busco la posición del bucket en el archivo de index
        fstream indexFile(indexFileName, ios::in | ios::out | ios::binary);
        if(!indexFile){
            cout << "Error al abrir el archivo de index" << endl;
            return;
        }
        Indice index;
        indexFile.seekg(hash*sizeof(Indice), ios::beg);
        // 2. Leo la posición del bucket
        indexFile.read((char*)&index, sizeof(long int));
        // 3. Busco el bucket en el archivo de records
        fstream recordsFile(recordsFileName, ios::in | ios::out | ios::binary);
        if(!recordsFile){
            cout << "Error al abrir el archivo de records" << endl;
            return;
        }
        Bucket bucket;
        recordsFile.seekg(index.pos, ios::beg);
        // 4. Leo el bucket
        recordsFile.read((char*)&bucket, sizeof(Bucket));
        // 5. Insertar elemento
        string indexBin(index.binario);
        
        bucket.binario = indexBin.substr(indexBin.length()-bucket.localDepth);  //101 -> 3 - 1 = 2 -> desde la pos 2 hasta el final
        //5.1 Si no está lleno el bucket, insertar el registro en la primera posición vacía
        if(bucket.nRecords<maxRecords){
            bucket.records[bucket.nRecords] = record;
            bucket.nRecords++;

            recordsFile.seekp(index.pos, ios::beg);
            recordsFile.write((char*)&bucket, sizeof(Bucket));

        }
        //3. Si está lleno, revisar que el depth local sea menor al global
        else{
            //3.1. Si localDepth es menor que globalDepth, duplicar el bucket
            if(bucket.localDepth<globalDepth){
                //3.1.1. Duplicar el bucket
    
                Bucket newBucket(bucket.localDepth+1,indexBin); //creo un nuevo bucket con el local depth + 1
                bucket.localDepth++; //actualizo el local depth al actual

                //definir el nuevo binario de cada bucket
                newBucket.binario = "1" + indexBin;
                bucket.binario = "0" + indexBin;

                //3.1.2. Reasignar los registros del bucket original y el nuevo bucket
                // Emparejo todos los registros del bucket original a un binario
                map<Record,string> allRecords;
                for(int i=0;i<bucket.nRecords;i++){
                    allRecords[bucket.records[i]] = decimalToBinary(bucket.records[i].id);
                }

                allRecords[record] = decimalToBinary(record.id); //agrego el nuevo registro al map

                //3.1.3 Eliminamos todos los elementos del bucket original
                bucket.nRecords = 0;                

                //3.1.4. Reasignar los registros a los buckets correspondientes
                for(auto it=allRecords.begin();it!=allRecords.end();it++){
                    if(it->second.substr(it->second.length()-bucket.localDepth) == bucket.binario){ //si los ultimos digitos del binario del registro son iguales al binario del bucket original
                        bucket.records[bucket.nRecords] = it->first;
                        bucket.nRecords++;
                    }
                    else if(it->second.substr(it->second.length()-bucket.localDepth) == newBucket.binario){
                        newBucket.records[newBucket.nRecords] = it->first;
                        newBucket.nRecords++;
                    }
                }

                //3.1 3. Actualizar el bucket original y el nuevo bucket en el archivo de records
                recordsFile.seekp(index.pos, ios::beg);
                recordsFile.write((char*)&bucket, sizeof(Bucket));
                recordsFile.seekp(0, ios::end);
                recordsFile.write((char*)&newBucket, sizeof(Bucket)); //Se escribe el nuevo bucket al final del archivo de records
            }
        //3.2. Si local depth es mayor o igual, encadenar un nuevo bucket al bucket original

        ////Error: Estamos sobreescribiendo los encadenados en la misma posición del archivo records.bin, No hay que utilizar
        // index.pos para los registros encadenados, sino que hay que buscar la última posición del archivo y escribir ahí

            else{
                Bucket aux;
                
                while(bucket.posNextBucket != -1){
                    recordsFile.seekg(bucket.posNextBucket, ios::beg);
                    recordsFile.read((char*)&aux, sizeof(Bucket)); 
                }
                if(aux.nRecords<maxRecords){
                    aux.records[aux.nRecords] = record;
                    aux.nRecords++;
                    recordsFile.seekp(index.pos, ios::beg);	//los encadenados tienen el mismo índice que el original
                    recordsFile.write((char*)&aux, sizeof(Bucket));
                }else{
                    //3.2.1. Crear un nuevo bucket
                    Bucket newBucket(aux.localDepth,aux.binario);
                    //3.2.2. Agregar el registro en la primera posición vacía del nuevo bucket
                    newBucket.records[newBucket.nRecords] = record;
                    newBucket.nRecords++;
                    //3.2.3 El bucket original apunta al nuevo bucket
                    recordsFile.seekp(0, ios::end);
                    aux.posNextBucket = recordsFile.tellp();
                    //3.2.4. Actualizar el bucket original y el nuevo bucket en el archivo de records
                    recordsFile.seekp(index.pos, ios::beg);
                    recordsFile.write((char*)&aux, sizeof(Bucket));
                    recordsFile.seekp(0, ios::end);
                    recordsFile.write((char*)&newBucket, sizeof(Bucket)); //Se escribe el nuevo bucket al final del archivo de records
                }  
            }
        }
    }

    void deleteRecord(int codigo){
        //TODO
    }

    void searchRecord(int codigo){
        //TODO
    }
};