#include <iostream>
#include <fstream>
#include <math.h>
#include <cstring>
#include <string.h>
#include <sstream>
#include <map>
#include <vector>

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
    char correo[20];
    int ciclo;
    char carrera[30];
    int codigo;

    public:
    Record(){
        id = -1;
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

    Record(long int id, string nombre, string correo, int ciclo, string carrera, int codigo){
        this->id = id;
        for(int i=0;i<20;i++){
            this->nombre[i] = nombre[i];
            this->correo[i] = correo[i];
        }
        
        this->ciclo = ciclo;
        for(int i=0;i<30;i++)
            this->carrera[i] = carrera[i];
        this->codigo = codigo;
    }

    ~Record(){}

    void display(){
        cout << "ID: " << id << endl;
        cout << "Nombre: " << nombre << endl;
        cout << "Correo: " << correo << endl;
        cout << "Ciclo: " << ciclo << endl;
        cout << "Carrera: " << carrera << endl;
        cout << "Codigo: " << codigo << endl;
        cout << "------" << endl;
    }
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
    long int pos; //Posición del bucket en records.bin


    public:
    Bucket(){
        localDepth = 1;
        posNextBucket = -1;
        nRecords = 0;
        strcpy(binario, "");
        pos = 0;

    }

    Bucket(int localDepth, string binario){
        this->localDepth = localDepth;
        posNextBucket = -1;
        nRecords = 0;
        for(int i = 0; i < binario.length(); i++){
            this->binario[i] = binario[i];
        }
        pos = 0;

    }

    ~Bucket(){}

    void display(){
        cout << "---------------BUCKET--------------\n";
        cout << "Bucket: " << binario << endl;
        cout << "Pos: " << pos << endl;
        cout << "Local depth: " << localDepth << endl;
        cout << "Pos next bucket: " << posNextBucket << endl;
        cout << "nRecords: " << nRecords << endl;
        cout << "Records: " << endl;
        for(int i=0;i<nRecords;i++){
            cout << "\tID: " << records[i].id << endl;
            cout << "\tNombre: " << records[i].nombre << endl;
            cout << "\t------" << endl;
        }
        cout << "\n";
    }
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

    void display(){
        cout << "Pos: " << pos << endl;
        cout << "Binario: ";
        printCharArray(binario, globalDepth);
        cout << "------" << endl;
    }
};

class ExtendibleHash{
    string indexFileName;
    string recordsFileName;
    hash<float> hash_fn;

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
        ofstream recordsFile0(recordsFileName, ios::out| ios::binary | ios::app);
        if(recordsFile0.tellp() == 0){
            Bucket bucket1;
            bucket1.pos = 0;
            llenarArray(bucket1.binario, globalDepth, "0");
            recordsFile0.write((char*)&bucket1, sizeof(Bucket));
            Bucket bucket2;
            bucket2.pos = sizeof(Bucket);
            llenarArray(bucket2.binario, globalDepth, "1");
            recordsFile0.write((char*)&bucket2, sizeof(Bucket));
        }

        // recordsFile.seekg(0, ios::beg);
        // Bucket bucket;
        // recordsFile.read((char*)&bucket, sizeof(Bucket));
        // cout << "Bucket 1: " << bucket.binario << endl;
        // cout << "Estamos en la pos " << recordsFile.tellg() << " de records.bin" << endl;
        // recordsFile.read((char*)&bucket, sizeof(Bucket));
        // cout << "Bucket2: " << bucket.binario << endl;

        indexFile.close();
        recordsFile0.close();
    }

    ~ExtendibleHash(){}


    bool validarId(Record record){
        ifstream recordsFile(recordsFileName, ios::in | ios::binary | ios::app);
        if(!recordsFile){
            try {
                throw "Error al abrir el archivo de records";
            } catch (const char* msg) {
                cerr << msg << endl;
            }
        }
        //1. Busco la posición del bucket en el archivo de index
        ifstream indexFile;
        indexFile.open(indexFileName, ios::in | ios::binary | ios::app);
        if(!indexFile){
            try {
                throw "Error al abrir el archivo de index";
            } catch (const char* msg) {
                cerr << msg << endl;
            }
        }
        Indice index;
        int hash = hash_fn(record.id)%(int)pow(2, globalDepth);
        indexFile.seekg(hash*sizeof(Indice), ios::beg); //La posicion del índice está dada por el hash del id
        indexFile.read((char*)&index, sizeof(Indice));
        //index.display();
        //2. Busco el bucket en el archivo de records
        Bucket bucket;
        recordsFile.seekg(index.pos, ios::beg);
        recordsFile.read((char*)&bucket, sizeof(Bucket));
        //bucket.display();
        //3. Busco el record en el bucket
        for(int i = 0; i < bucket.nRecords; i++){
            if(bucket.records[i].id == record.id){
                return true;
            }
        }
        return false;
    }

    void insertRecord(Record record){
        //cout << "Tamño de un bucket: " << sizeof(Bucket) << endl;
        //Falta implementar un metodo hash, por ahora saco el modulo
        long int numIndices = pow(2,globalDepth);

        bool existe = validarId(record);

        if(existe == false){

            int hash = hash_fn(record.id) % numIndices; //no se necesita llevar a binario porque el decimal me da el número de índice   
            //cout << "Hash: " << hash << endl;
            //1. Busco la posición del bucket en el archivo de index
            ifstream indexFile;
            indexFile.open(indexFileName, ios::in | ios::binary | ios::app);
            if(!indexFile){
                try {
                    throw "Error al abrir el archivo de index";
                } catch (const char* msg) {
                    cerr << msg << endl;
                }
            }
            Indice index;
            indexFile.seekg(hash*sizeof(Indice), ios::beg); //La posicion del índice está dada por el #del índice * el tamaño del índice
            // 2. Leo el índice del bucket para obtener la pos
            indexFile.read((char*)&index, sizeof(Indice));

            indexFile.close();

            //cout << "Pos a la que apunta el indice leido: " << index.pos << endl;
            // 3. Busco el bucket en el archivo de records
            fstream recordsFile;

            recordsFile.open(recordsFileName, ios::in | ios::out | ios::binary);
            if(!recordsFile){
                try {
                    throw "Error al abrir el archivo de records";
                } catch (const char* msg) {
                    cerr << msg << endl;
                }
            }

            // int prueba = 99999;
            // recordsFile.write((char*)&prueba, sizeof(int));
            

            Bucket bucket;
            recordsFile.seekg(index.pos, ios::beg);
            // 4. Leo el bucket
            recordsFile.read((char*)&bucket, sizeof(Bucket));

            //Prueba
            bucket.pos = index.pos;
            // cout << endl;    

            // //leo los registros del bucket
            // for(int i = 0; i < bucket.nRecords; i++){
            //     cout << "ID " << i << ": " << bucket.records[i].id << endl;
            // }

            // recordsFile.write((char*)&bucket, sizeof(Bucket));
            // 5. Insertar elemento
            long int posBucket;
            bool encadenamiento = false;

            // if(bucket.posNextBucket!=-1){
            //     bucket.display();

            // }
            // cout << bucket.posNextBucket << endl;
            while(bucket.posNextBucket != -1){ //Mientras haya buckets encadenados
                encadenamiento = true;
                //cout << "bucleee" << endl;   //Error: cuando el maximo del array "records" es Maxrecords, entra a bucle infinito
                //Verificamos que no se encuentre el registro en el bucket
                for(int i = 0; i < bucket.nRecords; i++){
                    if(bucket.records[i].id == record.id){
                        try{
                            throw "El registro ya existe";
                        }catch(const char* msg){
                            cerr << msg << endl;
                        }
                    }
                }
                //Si no se encuentra el registro, se pasa al siguiente bucket
                recordsFile.seekg(bucket.posNextBucket, ios::beg);
                posBucket = recordsFile.tellg();
                recordsFile.read((char*)&bucket, sizeof(Bucket)); 
                break;
            } //si no encontro el registro al final del while, entonces nos quedamos en el ultimo bucket

            if(!encadenamiento)
                posBucket = index.pos;

                    
            // cout << "Tamaño del bucket luego del while: " << bucket.nRecords << endl;
            string indexBin(index.binario); // indexBin es el binario del indice
            string bucketBin = indexBin.substr(indexBin.length()-bucket.localDepth);  
            // cout << "BucketBin: " << bucketBin << endl;
            // llenarArray(bucket.binario, globalDepth, bucketBin); //uso la funcion para llenar el array con el binario del indice
            for(int i=0;i<bucketBin.length();i++){
                if(bucketBin[i]=='0')
                    bucket.binario[i] = '0';
                else if(bucketBin[i]=='1')
                    bucket.binario[i] = '1';
                // cout << "i: " << i << endl;
                // cout << "bucket.binario: " << bucket.binario << endl;
            }

            //Quito letras que no me interesan
            bucket.binario[bucket.localDepth] = '\0';

            // cout << "Bucket.binario: " << bucket.binario << endl;
            
            // for(int i = 0; i < bucket.nRecords; i++){
            //     cout << "ID " << i << ": " << bucket.records[i].id << endl;
            // }

            // //Cuando borro todos los registros de records.bin corre bien hasta acá, pero si tiene datos no funciona :(
                
            // //5.1 Si no está lleno el bucket, insertar el registro en la primera posición vacía

            // cout << "posBucket: " << posBucket << endl;
            
            if(bucket.nRecords<maxRecords){
                // cout << "Bucket no está lleno" << endl;
                // Se inserta el registro en la primera posición vacía
                bucket.records[bucket.nRecords] = record;
                bucket.nRecords++;
                //bucket.display();
                recordsFile.seekp(posBucket, ios::beg);

                recordsFile.write((char*)&bucket, sizeof(Bucket));

                // //verificamos
                // recordsFile.seekg(posBucket, ios::beg);
                // recordsFile.read((char*)&bucket, sizeof(Bucket));
                // cout << "Bucket luego de insertar: " << endl;
                // bucket.display();
                // muestraData();
            }
            //3. Si está lleno, revisar que el depth local sea menor al global
            else{
                // cout  << "Bucket lleno" << endl;
                // 3.1. Si localDepth es menor que globalDepth, duplicar el bucket
                if(bucket.localDepth<globalDepth){
                    //3.1.1. Duplicar el bucket
        
                    Bucket newBucket(bucket.localDepth+1,bucket.binario); //creo un nuevo bucket con el local depth + 1
                    bucket.localDepth++; //actualizo el local depth al actual

                    //definir el nuevo binario de cada bucket
                    //string Newbin(newBucket.binario);
                    string Oldbin(bucket.binario);
                    string OldbinC;

                    //cout << "Newbin: " << Newbin << endl;
                    for(int i=0;i<Oldbin.length();i++){
                        if(Oldbin[i]!=' '){
                            OldbinC+=Oldbin[i];
                        }
                    }

                    string newBinario = "1" + OldbinC;
                    string oldBinario = "0" + OldbinC;

                    // cout << "Newbin: " << newBinario << endl;
                    // cout << "Oldbin: " << oldBinario << endl;
                    // cout << "OldBinario: " << oldBinario << endl;

                    for(int i=0;i<oldBinario.length();i++){
                        bucket.binario[i] = oldBinario[i];
                    }
                    // cout << "Bucket binario: " << bucket.binario << endl;
                    for(int i=0;i<newBinario.length();i++){
                        newBucket.binario[i] = newBinario[i];
                    }
                    // llenarArray(newBucket.binario, globalDepth, newBinario); //uso la funcion para llenar el array con el binario del indice
                    // llenarArray(bucket.binario, globalDepth, oldBinario); //uso la funcion para llenar el array con el binario del indice

                    // 3.1.2. Reasignar los registros del bucket original y el nuevo bucket
                    // Emparejo todos los registros del bucket original a un binario
                    map<Record,string> allRecords;

                    int hashC2;

                    for(int i=0;i<bucket.nRecords;i++){
                        //Aqui si entra
                        hashC2 = hash_fn(bucket.records[i].id)%((int)pow(2,globalDepth));
                        allRecords[bucket.records[i]] = decimalToBinary(hashC2);
                    }

                    allRecords[record] = decimalToBinary(record.id); //agrego el nuevo registro al map

                    //3.1.3 Eliminamos todos los elementos del bucket original
                    bucket.nRecords = 0;    


                    // 3.1.4. Reasignar los registros a los buckets correspondientes
                    
                    for(auto it=allRecords.begin();it!=allRecords.end();it++){
                        if(it->second.substr(it->second.length()-bucket.localDepth) == oldBinario){ //si los ultimos digitos del binario del registro son iguales al binario del bucket original
                            // cout << "entro al if" << endl;
                            bucket.records[bucket.nRecords] = it->first;
                            bucket.nRecords++;

                        }
                        else if(it->second.substr(it->second.length()-bucket.localDepth) == newBinario){
                            // cout << "entro al else if" << endl;
                            newBucket.records[newBucket.nRecords] = it->first;
                            newBucket.nRecords++;
                        }
                    }
                    // cout << "--Luego del split--" << endl;
                    // cout << "Bucket:" << endl;
                    // bucket.display();
                    // cout << "NewBucket:" << endl;
                    // newBucket.display();

                    // 3.1 3. Actualizar el bucket original y el nuevo bucket en el archivo de records
                    recordsFile.seekp(index.pos, ios::beg);
                    recordsFile.write((char*)&bucket, sizeof(Bucket));
                    recordsFile.seekp(0, ios::end);
                    long int posNewBucket = recordsFile.tellp(); //guardo la pos del new bucket
                    newBucket.pos = posNewBucket;
                    recordsFile.write((char*)&newBucket, sizeof(Bucket)); //Se escribe el nuevo bucket al final del archivo de records

                    // //verifico que se hayan escrito bien los buckets

                    // //lectura del bucket original
                    // recordsFile.seekg(index.pos, ios::beg);
                    // cout << "Posicion del bucket original: " << index.pos << endl;
                    // recordsFile.read((char*)&bucket, sizeof(Bucket));
                    // cout  << "Bucket original: " << endl;
                    // bucket.display();

                    //Lectura del nuevo bucket
                    recordsFile.seekg(posNewBucket, ios::beg);
                    recordsFile.read((char*)&newBucket, sizeof(Bucket));
                    
                    // cout << "Nuevo bucket: " << endl;
                    // newBucket.display();

                    //3.1.4. Actualizar el archivo de índices, actualizar la direccion a donde apuntan ya que se creo un nuevo bucket

                    indexFile.open(indexFileName, ios::in | ios::binary);
                    vector<Indice> indices;
                    Indice indexAux;
                    // cout << "index.pos: " << index.pos << endl;
                    // cout << "IndexAux.pos: " << indexAux.pos << endl;
                    indexFile.seekg(0, ios::end);
                    long int posFinal = indexFile.tellg();
                    // cout << "Posicion final: " << posFinal << endl;
                    indexFile.seekg(0, ios::beg);

                    while(indexFile.tellg()<posFinal){
                        // cout << "pos del archivo: " << indexFile.tellg() << endl;
                        indexFile.read((char*)&indexAux, sizeof(Indice));
                        // cout << "IndexAux.pos: " ;
                        //printCharArray(indexAux.binario, globalDepth);
                        indices.push_back(indexAux);
                    }
                    indexFile.close();
                    // cout << "Oldbinario: " << oldBinario << endl;
                    // cout << "Newbinario: " << newBinario << endl;
                    // cout << "# de indices: " << indices.size() << endl;

                    // //Leemos todos los índices
                    // for(auto it=indices.begin();it!=indices.end();it++){
                    //     it->display();
                    // }

                    //Itero todos los índices
                for(auto it=indices.begin();it!=indices.end();it++){
                        // cout << "IndexAux.pos (inicio): " << it->pos << endl;
                        // cout << "IndexAux.binario1: " << it->binario << endl;
                        string indexBin2(it->binario);
                        if(indexBin2.substr(indexBin2.length()-bucket.localDepth) == oldBinario){ //si el binario del indice es igual al binario del bucket original
                            it->pos = index.pos;       //se le deja la misma posicion
                        }
                        else if(indexBin2.substr(indexBin2.length()-bucket.localDepth) == newBinario){
                            it->pos = posNewBucket;       //si no, se le asigna la posicion del nuevo bucket
                        }
                        // cout << "IndexAux.binario2: " << it->binario << endl;
                        // cout << "IndexAux.pos (fin): " << it->pos << endl;
                } 

                    //Escribo los índices modificados en el archivo de índices
                    ofstream indexFile2;
                    // indexFile2.open(indexFileName, ios::binary | ios::out);
                    indexFile2.open(indexFileName, ios::out | ios::binary ); //no me funciona si lo abro sin el modo app 
                    indexFile2.seekp(0, ios::beg);

                    for(auto it=indices.begin();it!=indices.end();it++){
                        Indice ind = *it;
                        indexFile2.write((char*)&ind, sizeof(Indice));
                    }

                    indexFile2.close();
                    //3.1.4.1 Recorrer todos los índices en index.bin: 
                    
                    // fstream indexFile2;
                    // indexFile2.open(indexFileName, ios::binary | ios::in | ios::out);
                    // indexFile2.seekg(0, ios::end);
                    // long int indexSize = indexFile2.tellg();
                    // indexFile2.seekg(0, ios::beg);
                    // Indice indexAux;
                    // while(indexFile2.tellg() < indexSize){ //itero todos los indices
                    //     long int posIndex = indexFile2.tellg();
                    //     indexFile2.read((char*)&indexAux, sizeof(Indice));
                    //     string indexBin2(indexAux.binario);
                    //     if(indexBin2.substr(indexBin2.length()-bucket.localDepth) == oldBinario){ //si el binario del indice es igual al binario del bucket original
                    //         indexAux.pos = index.pos;       //se le deja la misma posicion
                    //     }
                    //     else if(indexBin2.substr(indexBin2.length()-bucket.localDepth) == newBinario){
                    //         indexAux.pos = posNewBucket;       //si no, se le asigna la posicion del nuevo bucket
                    //     }
                    //     //Escribo los cambios de los índices
                    //     indexFile2.seekp(posIndex, ios::beg);
                    //     indexFile2.write((char*)&indexAux, sizeof(Indice));
                    // }

                    // indexFile2.close();
                }
                //3.2. Si local depth es mayor o igual, encadenar un nuevo bucket al bucket original

                else{       
                    //cout << "Entra encadenamiento:" <<endl;
                    
                    //3.2.1. Crear un nuevo bucket
                    Bucket newBucket(bucket.localDepth,bucket.binario); // copia del bucket original
                    //3.2.2. Agregar el registro en la primera posición vacía del nuevo bucket
                    newBucket.records[newBucket.nRecords] = record;
                    newBucket.nRecords++;
                    //3.2.3 El bucket original apunta al nuevo bucket
                    recordsFile.seekp(0, ios::end);
                    //cout << "Posicion del nuevo bucket: " << recordsFile.tellp() << endl;
                    bucket.posNextBucket = recordsFile.tellp();
                    //3.2.4. Actualizar el bucket original y el nuevo bucket en el archivo de records

                    recordsFile.seekp(posBucket, ios::beg); 
                    recordsFile.write((char*)&bucket, sizeof(Bucket));
                    recordsFile.seekp(0, ios::end);
                    recordsFile.write((char*)&newBucket, sizeof(Bucket)); //Se escribe el nuevo bucket al final del archivo de records
                    
                    recordsFile.close();

                    // //verificacion de encadenamiento

                    // //lectura del bucket original
                    // recordsFile.seekg(posBucket, ios::beg);
                    // cout << "posBucket: " << posBucket << endl;
                    // recordsFile.read((char*)&bucket, sizeof(Bucket));
                    // cout  << "Bucket original: " << endl;
                    // bucket.display();

                    // //Lectura del nuevo bucket
                    // recordsFile.seekg(bucket.posNextBucket, ios::beg);
                    // recordsFile.read((char*)&newBucket, sizeof(Bucket));
                    // cout << "Nuevo bucket: " << endl;
                    // newBucket.display();
                }
            }

            recordsFile.close();
            
        }else{
            try{
                throw "El id ya existe";
            }catch(const char* msg){
                cerr << msg << endl;
            }
        } 
    }

    void muestraData() {
        cout << "\n\n----------------DATA----------------\n";
        //muestra index
        
        cout << "MUESTRA LOS INDEX\n";
        ifstream indexFile;
        indexFile.open(indexFileName, ios::binary);
        indexFile.seekg(0, ios::end);
        long int indexSize = indexFile.tellg();
        indexFile.seekg(0, ios::beg);
        Indice index;
        while(indexFile.tellg() < indexSize){
            indexFile.read((char*)&index, sizeof(Indice));
            index.display();
        }
        indexFile.close();

        //muestra records
        cout << "MUESTRA LOS RECORDS\n";
        ifstream recordsFile;
        recordsFile.open(recordsFileName, ios::in | ios::binary);
        recordsFile.seekg(0, ios::end);
        long int recordsSize = recordsFile.tellg();
        recordsFile.seekg(0, ios::beg);
        Bucket bucket;
        while(recordsFile.tellg() < recordsSize){
            recordsFile.read((char*)&bucket, sizeof(Bucket));
            bucket.display();
        }
        recordsFile.close();
        cout << "FIN DATA\n\n";
    }

    void deleteRecord(int id){
        //1. Hallo el hash del id
        long int numIndices = pow(2,globalDepth);
        int hash = hash_fn(id) % numIndices;
        //2. Buscar el índice del registro en el archivo de índices
        fstream indexFile;
        indexFile.open(indexFileName, ios::binary | ios::in);
        indexFile.seekg(hash*sizeof(Indice), ios::beg);
        Indice index;
        indexFile.read((char*)&index, sizeof(Indice));
        indexFile.close();
        //3. Buscar el registro en el archivo de records
        fstream recordsFile;
        recordsFile.open(recordsFileName, ios::binary | ios::in | ios::out);
        recordsFile.seekg(index.pos, ios::beg);
        Bucket bucket;
        recordsFile.read((char*)&bucket, sizeof(Bucket));
        //4. Buscar el registro en el bucket
        for(int i=0; i<bucket.nRecords; i++){
            if(bucket.records[i].id == id){
                //5. Eliminar el registro del bucket
                cout << "Registro eliminado:" << endl;
                bucket.records[i].display();

                int posEliminado = i;
                int numArrastres = bucket.nRecords - posEliminado - 1;
                for(int j=i,k=0; k<numArrastres; j++,k++){
                    bucket.records[j] = bucket.records[j+1];
                }

                bucket.nRecords--;
                //6. Actualizar el bucket en el archivo de records

                recordsFile.seekp(index.pos, ios::beg);
                recordsFile.write((char*)&bucket, sizeof(Bucket));

                return;
            }
        }
        //7. Si no se encuentra, buscar en los encadenamientos del bucket
        while(bucket.posNextBucket != -1){
            recordsFile.seekg(bucket.posNextBucket, ios::beg);
            recordsFile.read((char*)&bucket, sizeof(Bucket));
            for(int i=0; i<bucket.nRecords; i++){
                if(bucket.records[i].id == id){
                    //8. Eliminar el registro del bucket
                    cout << "Registro eliminado:" << endl;
                    bucket.records[i].display();

                    int posEliminado = i;
                    int numArrastres = bucket.nRecords - posEliminado - 1;
                    for(int j=i,k=0; k<numArrastres; j++,k++){
                        bucket.records[j] = bucket.records[j+1];
                    }

                    bucket.nRecords--;
                    //9. Actualizar el bucket en el archivo de records

                    recordsFile.seekp(index.pos, ios::beg);
                    recordsFile.write((char*)&bucket, sizeof(Bucket));

                    return;
                }
            }
        }
        //10. Si no se encuentra, el registro no existe
        cout << "El registro no existe" << endl;
        recordsFile.close();
    }

    Record searchRecord(int id){
        try{
            //1. Hallo el hash del id
            long int numIndices = pow(2,globalDepth);
            int hash = hash_fn(id) % numIndices;
            //2. Buscar el índice del registro en el archivo de índices
            fstream indexFile;
            indexFile.open(indexFileName, ios::binary | ios::in);
            indexFile.seekg(hash*sizeof(Indice), ios::beg);
            Indice index;
            indexFile.read((char*)&index, sizeof(Indice));
            indexFile.close();
            //3. Buscar el registro en el archivo de records
            fstream recordsFile;
            recordsFile.open(recordsFileName, ios::binary | ios::in);
            recordsFile.seekg(index.pos, ios::beg);
            Bucket bucket;
            recordsFile.read((char*)&bucket, sizeof(Bucket));
            //4. Buscar el registro en el bucket
            for(int i=0; i<bucket.nRecords; i++){
                if(bucket.records[i].id == id){
                    return bucket.records[i];
                }
            }

            //5. Si no se encuentra, iterar por los encadenamientos
            while(bucket.posNextBucket != -1){
                recordsFile.seekg(bucket.posNextBucket, ios::beg);
                recordsFile.read((char*)&bucket, sizeof(Bucket));
                for(int i=0; i<bucket.nRecords; i++){
                    if(bucket.records[i].id == id){
                        return bucket.records[i];
                    }
                }
            }
            recordsFile.close();
            throw "No se encontró el registro, devolviendo registro vacío";
        }
        catch(const char* msg){
            cerr << msg << endl;
        }
        return Record();
    }


    void LoadCsv(const string& FileName){
        ifstream file(FileName);
        string line;

        while(getline(file,line)){
            stringstream ss(line);
            long int id;
            string nombre, correo, carrera;
            int ciclo, codigo;
            getline(ss, line, ',');
            id = stol(line);
            getline(ss, nombre, ',');
            getline(ss, correo, ',');
            ss >> ciclo;
            ss.ignore();
            getline(ss, carrera, ',');
            ss >> codigo;
            Record record(id, nombre.c_str(), correo.c_str(), ciclo, carrera.c_str(), codigo);

            insertRecord(record);

        }
        file.close();
    }

    void InsertRecordCSV(const string& FileName,Record record){

        //Tambien debo insertarlo en ExtendibleHash
        insertRecord(record);

        bool existe = validarId(record);

        if(existe == false){
            //Insertar en el archivo CSV
            ofstream file(FileName,ios::app);
            //salto de linea
            file << endl;
            file << record.id << "," << record.nombre << "," << record.correo << "," << record.ciclo << "," << record.carrera << "," << record.codigo ;
            file.close();
        }

    }
};