#include <iostream>
#include <fstream>
#include <math.h>
#include <cstring>
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

//funcion para imprimir array de char
void printCharArray(char* arr, int size){
    for(int i = 0; i < size; i++){
        cout << arr[i];
    }
    cout << endl;
}

void llenarArray(char conteiner[], int sizeCont, string data){
	for(int i=0; i<sizeCont; i++){
		if(i < data.size())
			conteiner[i] = data[i];
		else
			conteiner[i] = ' ';
	}
	conteiner[sizeCont - 1] = '\0';
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

    Record(long int id, const char* nombre, const char* apellido, int ciclo, const char* carrera, int codigo){
        this->id = id;
        strcpy(this->nombre, nombre);
        strcpy(this->apellido, apellido);
        this->ciclo = ciclo;
        strcpy(this->carrera, carrera);
        this->codigo = codigo;
    }

    ~Record(){}

};

// Definimos la sobrecarga globalmente para que el std::map lo pueda utilizar
bool operator<(const Record& p1, const Record& p2) {
    return p1.id < p2.id;
}


struct Bucket{
    //No necesita binario porque solo se necesita modificarlo en index.bin
    Record records[maxRecords]; //Cambiar 100 por maxRecords y funciona
    int nRecords;
    int localDepth;
    long int posNextBucket; //Bucket encadenados a este bucket
    char binario[globalDepth];

    public:
    Bucket(){
        localDepth = 1;
        posNextBucket = -1;
        nRecords = 0;
        strcpy(binario, "");
    }

    Bucket(int localDepth, string binario){
        this->localDepth = localDepth;
        posNextBucket = -1;
        nRecords = 0;
        for(int i = 0; i < binario.length(); i++){
            this->binario[i] = binario[i];
        }
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

    ~Indice(){}
};

class ExtendibleHash{
    string indexFileName;
    string recordsFileName;

    public:
    ExtendibleHash(string indexFileName, string recordsFileName){
        this->indexFileName = indexFileName;
        this->recordsFileName = recordsFileName;

        //1. Genero el archivo de index
        ofstream indexFile(indexFileName, ios::out | ios::binary | ios::app);
        if(!indexFile){
            cout << "Error al crear el archivo de index" << endl;
            return;
        }
        //2. Escribo los índices: al inicio solo hay 2 buckets, uno en la pos 0 y otro en la pos sizeof(long int)
        //Si el index.bin está vacío, se crean los 2 buckets
        if(indexFile.tellp() == 0){
            for(int i = 0; i < pow(2, globalDepth); i++){
                Indice index;
                if(i%2==0){
                    index.pos = 0;
                }else{
                    index.pos = sizeof(Bucket);
                }
                strcpy(index.binario, decimalToBinary(i).c_str()); //"0" -> "000"
                indexFile.write((char*)&index, sizeof(Indice));
            }
        }

        //3. Genero el archivo de records con solo 2 buckets iniciales
        fstream recordsFile(recordsFileName, ios::out | ios::in | ios::binary | ios::app);
        if(recordsFile.tellp() == 0){
            Bucket bucket1;
            llenarArray(bucket1.binario, globalDepth, "0");
            recordsFile.write((char*)&bucket1, sizeof(Bucket));
            Bucket bucket2;
            llenarArray(bucket2.binario, globalDepth, "1");
            recordsFile.write((char*)&bucket2, sizeof(Bucket));
        }

        // recordsFile.seekg(0, ios::beg);
        // Bucket bucket;
        // recordsFile.read((char*)&bucket, sizeof(Bucket));
        // cout << "Bucket 1: " << bucket.binario << endl;
        // cout << "Estamos en la pos " << recordsFile.tellg() << " de records.bin" << endl;
        // recordsFile.read((char*)&bucket, sizeof(Bucket));
        // cout << "Bucket2: " << bucket.binario << endl;

        indexFile.close();
        recordsFile.close();
    }

    ~ExtendibleHash(){}

    void insertRecord(Record record){
        //Falta implementar un metodo hash, por ahora saco el modulo
        long int numIndices = pow(2,globalDepth);
        int hash = record.id % numIndices;     //no se necesita llevar a binario porque el decimal me da el número de índice
        cout << "Hash: " << hash << endl;
        //1. Busco la posición del bucket en el archivo de index
        fstream indexFile(indexFileName, ios::in | ios::out | ios::binary | ios::app);
        if(!indexFile){
            cout << "Error al abrir el archivo de index" << endl;
            return;
        }
        Indice index;
        indexFile.seekg(hash*sizeof(Indice), ios::beg); //La posicion del índice está dada por el #del índice * el tamaño del índice
        // 2. Leo el índice del bucket para obtener la pos
        indexFile.read((char*)&index, sizeof(Indice));
        cout << "Cada index pesa: " << sizeof(Indice) << endl;
        cout << "Estamos en la pos " << indexFile.tellg() << " de index.bin" << endl;
        cout << "Posicion a la que apunta el indice " << index.pos << endl;
        // 3. Busco el bucket en el archivo de records
        fstream recordsFile(recordsFileName, ios::in | ios::out | ios::binary | ios::app);
        if(!recordsFile){
            cout << "Error al abrir el archivo de records" << endl;
            return;
        }

        // int prueba = 99999;
        // recordsFile.write((char*)&prueba, sizeof(int));
        

        Bucket bucket;
        cout << "Primer binario:" << bucket.binario << endl;
        // recordsFile.seekg(500, ios::beg);
        // recordsFile.write((char*)&bucket, sizeof(Bucket));

        recordsFile.seekg(index.pos, ios::beg);
        cout << "Tamaño de un bucket: " << sizeof(Bucket) << endl;
        cout << "Posicion del bucket: " << index.pos << endl;
        // 4. Leo el bucket
        recordsFile.read((char*)&bucket, sizeof(Bucket));
        cout << "Tamaño bucket: " << bucket.nRecords << endl;

        cout << "Segundo binario:" << endl;
        printCharArray(bucket.binario, globalDepth);
        cout << endl;    

        //leo los registros del bucket
        for(int i = 0; i < bucket.nRecords; i++){
            cout << "ID " << i << ": " << bucket.records[i].id << endl;
        }

        // recordsFile.write((char*)&bucket, sizeof(Bucket));
        // 5. Insertar elemento
        long int posBucket;
        cout << "aca" << endl;
        while(bucket.posNextBucket != -1){ //Mientras haya buckets encadenados
            cout << "bucleee" << endl;   //Error: cuando el maximo del array "records" es Maxrecords, entra a bucle infinito
            cout << "Tamaño del bucketWhile: " << bucket.nRecords << endl;
            //Verificamos que no se encuentre el registro en el bucket
            for(int i = 0; i < bucket.nRecords; i++){
                if(bucket.records[i].id == record.id){
                    cout << "El registro ya existe" << endl;
                    return;
                }
            }
            //Si no se encuentra el registro, se pasa al siguiente bucket
            recordsFile.seekg(bucket.posNextBucket, ios::beg);
            posBucket = recordsFile.tellg();
            recordsFile.read((char*)&bucket, sizeof(Bucket)); 
        } //si no encontro el registro al final del while, entonces nos quedamos en el ultimo bucket
        // cout << "Tamaño del bucket luego del while: " << bucket.nRecords << endl;
        string indexBin(index.binario); // indexBin es el binario del indice
        cout << "Binario: " << indexBin << endl;

        string bucketBin = indexBin.substr(indexBin.length()-bucket.localDepth);  
        cout << "Binario definitivo: " << bucketBin << endl;
        // bucket.binario = indexBin.substr(indexBin.length()-bucket.localDepth);  //101 -> 3 - 1 = 2 -> desde la pos 2 hasta el final
        llenarArray(bucket.binario, globalDepth, bucketBin); //uso la funcion para llenar el array con el binario del indice
        cout << "Binario del bucket: ";
        printCharArray(bucket.binario, globalDepth);

        // cout << "Binario definitivo: " << bucket.binario << endl;
        // //Cuando borro todos los registros de records.bin corre bien hasta acá, pero si tiene datos no funciona :(
            
        // //5.1 Si no está lleno el bucket, insertar el registro en la primera posición vacía
        if(bucket.nRecords<maxRecords){
            cout << "no esta lleno" << endl;
            bucket.records[bucket.nRecords] = record;
            bucket.nRecords++;

            recordsFile.seekp(index.pos, ios::beg);
            // recordsFile.seekp(0, ios::beg);
            cout << "sizeof(Bucket): " << sizeof(Bucket) << endl;
            cout << "IndexPos: " << index.pos << endl;
            cout << "Binario del bucket:" << bucket.binario << endl;
            cout << "Tamaño del bucket luego de la inserción: " << bucket.nRecords << endl;
            recordsFile.write((char*)&bucket, sizeof(Bucket));
            cout << "ya escribió " << endl;
        }
        // //3. Si está lleno, revisar que el depth local sea menor al global
        // else{
        //     cout  << "else" << endl;
        //     //3.1. Si localDepth es menor que globalDepth, duplicar el bucket
        //     if(bucket.localDepth<globalDepth){
        //         //3.1.1. Duplicar el bucket
    
        //         Bucket newBucket(bucket.localDepth+1,bucket.binario); //creo un nuevo bucket con el local depth + 1
        //         bucket.localDepth++; //actualizo el local depth al actual

        //         //definir el nuevo binario de cada bucket
        //         newBucket.binario = "1" + newBucket.binario;
        //         bucket.binario = "0" + bucket.binario;

        //         //3.1.2. Reasignar los registros del bucket original y el nuevo bucket
        //         // Emparejo todos los registros del bucket original a un binario
        //         map<Record,string> allRecords;
        //         for(int i=0;i<bucket.nRecords;i++){
        //             allRecords[bucket.records[i]] = decimalToBinary(bucket.records[i].id);
        //         }

        //         allRecords[record] = decimalToBinary(record.id); //agrego el nuevo registro al map

        //         //3.1.3 Eliminamos todos los elementos del bucket original
        //         bucket.nRecords = 0;                

        //         //3.1.4. Reasignar los registros a los buckets correspondientes
        //         for(auto it=allRecords.begin();it!=allRecords.end();it++){
        //             if(it->second.substr(it->second.length()-bucket.localDepth) == bucket.binario){ //si los ultimos digitos del binario del registro son iguales al binario del bucket original
        //                 bucket.records[bucket.nRecords] = it->first;
        //                 bucket.nRecords++;
        //             }
        //             else if(it->second.substr(it->second.length()-bucket.localDepth) == newBucket.binario){
        //                 newBucket.records[newBucket.nRecords] = it->first;
        //                 newBucket.nRecords++;
        //             }
        //         }

        //         //3.1 3. Actualizar el bucket original y el nuevo bucket en el archivo de records
        //         recordsFile.seekp(index.pos, ios::beg);
        //         recordsFile.write((char*)&bucket, sizeof(Bucket));
        //         recordsFile.seekp(0, ios::end);
        //         long int posNewBucket = recordsFile.tellp(); //guardo la pos del new bucket
        //         recordsFile.write((char*)&newBucket, sizeof(Bucket)); //Se escribe el nuevo bucket al final del archivo de records

        //         //3.1.4. Actualizar el índice
        //         //3.1.4.1 Recorrer todos los índices en index.bin
        //         indexFile.seekg(0, ios::end);
        //         long int indexSize = indexFile.tellg();
        //         indexFile.seekg(0, ios::beg);
        //         Indice indexAux;
        //         while(indexFile.tellg() < indexSize){ //itero todos los indices
        //             indexFile.read((char*)&indexAux, sizeof(Indice));
        //             string indexBin2(indexAux.binario);
        //             if(indexBin2.substr(indexBin2.length()-bucket.localDepth) == bucket.binario){ //si el binario del indice es igual al binario del bucket original
        //                 indexAux.pos = index.pos;       //se le deja la misma posicion
        //             }
        //             else{
        //                 indexAux.pos = posNewBucket;       //si no, se le asigna la posicion del nuevo bucket
        //             }
        //         }

        //     }
        // //3.2. Si local depth es mayor o igual, encadenar un nuevo bucket al bucket original

        //     else{              
        //         if(bucket.nRecords<maxRecords){
        //             bucket.records[bucket.nRecords] = record;
        //             bucket.nRecords++;
        //             recordsFile.seekp(posBucket, ios::beg);	//los encadenados tienen el mismo índice que el original
        //             recordsFile.write((char*)&bucket, sizeof(Bucket));
        //         }else{
        //             //3.2.1. Crear un nuevo bucket
        //             Bucket newBucket(bucket.localDepth,bucket.binario);
        //             //3.2.2. Agregar el registro en la primera posición vacía del nuevo bucket
        //             newBucket.records[newBucket.nRecords] = record;
        //             newBucket.nRecords++;
        //             //3.2.3 El bucket original apunta al nuevo bucket
        //             recordsFile.seekp(0, ios::end);
        //             bucket.posNextBucket = recordsFile.tellp();
        //             //3.2.4. Actualizar el bucket original y el nuevo bucket en el archivo de records

        //             recordsFile.seekp(posBucket, ios::beg); 
        //             recordsFile.write((char*)&bucket, sizeof(Bucket));
        //             recordsFile.seekp(0, ios::end);
        //             recordsFile.write((char*)&newBucket, sizeof(Bucket)); //Se escribe el nuevo bucket al final del archivo de records
        //         }  
        //     }
        // }
        // //6. Cierro los archivos
        // indexFile.close();
        // recordsFile.close();
        // cout << "archivos cerrados" << endl;
    }

    void deleteRecord(int codigo){
        //TODO
    }

    void searchRecord(int codigo){
        //TODO
    }
};