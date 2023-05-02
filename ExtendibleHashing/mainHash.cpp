#include <iostream>
#include "extendibleHash.h"
#include <fstream>

using namespace std;

int main(){
    // Limpia index.bin y records.bin
    ofstream file;
    file.open("index.bin", ios::out | ios::binary);
    file.close();
    file.open("records.bin", ios::out | ios::binary);
    file.close();

    ExtendibleHash eh("index.bin", "records.bin");

    cout << "---Inserción 1---" << endl;
    eh.insertRecord(Record(1, "Pepe", "Vazques", 5, "CS", 202177777)); //1 en binario es 001 -> bucket 1

    cout << "---Inserción 2---" << endl;
    eh.insertRecord(Record(2, "nombre2", "apellido2", 2, "carrera2", 2)); //3 en binario es 011 -> bucket 2

    cout << "---Inserción 3---" << endl;
    eh.insertRecord(Record(5, "nombre5", "apellido3", 2, "carrera3", 2)); //2 en binario es 101 -> bucket 1

    cout << "---Inserción 4---" << endl;
    eh.insertRecord(Record(7, "nombre4", "apellido3", 2, "carrera3", 2)); //2 en binario es 111 -> bucket 2

    cout << "---Inserción 5---" << endl;
    eh.insertRecord(Record(9, "nombre9", "apellido9", 1, "carrera1", 1)); //1 en binario es 001 -> bucket 1
    eh.muestraData();

    cout << "---Inserción 6---" << endl;
    eh.insertRecord(Record(11, "nombre11", "apellido11", 2, "carrera2", 2)); //3 en binario es 011 -> bucket 2
    eh.muestraData();
    
    cout << "---Inserción 7---" << endl;
    eh.insertRecord(Record(13, "nombre13", "apellido13", 2, "carrera3", 2)); //2 en binario es 101 -> bucket 1

    cout << "---Inserción 8---" << endl;
    eh.insertRecord(Record(15, "nombre15", "apellido15", 2, "carrera3", 2)); //2 en binario es 111 -> bucket 2


    cout << "---Inserción 9---" << endl;
    eh.insertRecord(Record(17, "nombre17", "apellido17", 1, "carrera1", 1)); //1 en binario es 001 -> bucket 1

    cout << "---Inserción 10---" << endl;
    eh.insertRecord(Record(25, "nombre19", "apellido19", 2, "carrera2", 2)); //3 en binario es 011 -> bucket 2


    
    cout << "---Inserción 11---" << endl;
    eh.insertRecord(Record(33, "nombre21", "apellido21", 2, "carrera3", 2)); //2 en binario es 101 -> bucket 1

    cout << "---Inserción 12---" << endl;
    eh.insertRecord(Record(41, "nombre22", "apellido22", 2, "carrera4", 2)); //2 en binario es 101 -> bucket 1

    cout << "---Inserción 13---" << endl;
    eh.insertRecord(Record(49, "nombre23", "apellido23", 2, "carrera5", 2)); //2 en binario es 101 -> bucket 1

    cout << "---Inserción 14---" << endl;
    eh.insertRecord(Record(57, "nombre24", "apellido24", 2, "carrera6", 2));

    cout << "---Inserción 15---" << endl;
    eh.insertRecord(Record(65, "Encadenadito", "Cardenas", 1, "Industrial", 202110299));

    cout << "---Inserción 16---" << endl;
    eh.insertRecord(Record(73, "nombre26", "apellido26", 2, "carrera8", 2));

    cout << "---Inserción 17---" << endl;
    eh.insertRecord(Record(81, "nombre27", "apellido27", 2, "carrera9", 2));

    cout << "---Inserción 18---" << endl;
    eh.insertRecord(Record(89, "nombre28", "apellido28", 2, "carrera10", 2));

    cout << "---Inserción 19---" << endl;
    eh.insertRecord(Record(97, "nombre29", "apellido29", 2, "carrera11", 2));

    cout << "---Inserción 20---" << endl;
    eh.insertRecord(Record(105, "nombre30", "apellido30", 2, "carrera12", 2));

    cout << "---Inserción 21---" << endl;
    eh.insertRecord(Record(113, "nombre31", "apellido31", 2, "carrera13", 2));

    cout << "---Inserción 22---" << endl;
    eh.insertRecord(Record(121, "nombre32", "apellido32", 2, "carrera14", 2));

    cout << "---Inserción 23---" << endl;
    eh.insertRecord(Record(129, "nombre33", "apellido33", 2, "carrera15", 2));

    cout << "---Inserción 24---" << endl;
    eh.insertRecord(Record(137, "nombre34", "apellido34", 2, "carrera16", 2));

    cout << "---Inserción 25---" << endl;
    eh.insertRecord(Record(145, "nombre35", "apellido35", 2, "carrera17", 2));


    cout << "Lectura de datos" << endl;
    ifstream file2;
    file2.open("records.bin", ios::in | ios::binary);
    file2.seekg(0, ios::end);
    long int final = file2.tellg();
    file2.seekg(0, ios::beg);
    while(file2.tellg() < final){
        Bucket b;
        file2.read((char*)&b, sizeof(Bucket));
        cout << "Bucket: " << b.binario << endl;
        for(int i = 0; i < b.nRecords; i++){
            cout << b.records[i].id << " ";
        }
        cout << endl;
    }
    file2.close();

    cout << "\tLectura de record no encadenado" << endl;
    Record r = eh.searchRecord(1);
    r.display();


    cout << "\tLectura de record encadenado" << endl;
    Record r2 = eh.searchRecord(65);
    r2.display();


    cout << "\tLectura de un record que no se encuentra" << endl;
    Record r3 = eh.searchRecord(100);
    r3.display();   


    cout << "\tEliminación de un record que sí se encuentra" << endl;
    eh.deleteRecord(1);

    cout << "\tBusco el record eliminado" << endl;
    Record r4 = eh.searchRecord(1);
    r4.display();

    // int opcion;
    // while(opcion!=4){
    //     //Menu que permita al usuario insertar, buscar y eliminar
    //     cout << "Menu Hash" << endl;
    //     cout << "1. Buscar" << endl;
    //     cout << "2. Insertar" << endl;
    //     cout << "3. Eliminar" << endl;
    //     cout << "4. Salir" << endl;
        
    //     int id;
    //     cin>>opcion;
    //     if(opcion==1){
    //         cout << "Ingrese el id:"; cin>>id;
    //         Record recBusqueda = eh.searchRecord(id);
    //         recBusqueda.display(); 
    //     }
    //     else if(opcion==2){
    //         string nombre;
    //         string apellido;
    //         int ciclo;
    //         string carrera;
    //         int codigo;

    //         cout << "Ingrese el id:";cin>>id;
    //         cin.ignore();
    //         cout << "Ingrese el nombre:"; getline(cin,nombre);
    //         cout << "Ingrese el apellido:"; getline(cin, apellido);
    //         cout << "Ingrese el ciclo:";cin>>ciclo;
    //         cin.ignore();
    //         cout << "Ingrese la carrera:";getline(cin,carrera);
    //         cout << "Ingres el codigo [solo numeros]:";cin>>codigo;
    //         Record buscado(id,nombre,apellido,ciclo,carrera,codigo);
            
    //         eh.insertRecord(buscado);
    //         cout << "Agregado con éxito!" << endl;
    //     }
    //     else if(opcion==3){
    //         cout << "Ingrese el id:";cin>>id;
    //         eh.deleteRecord(id);
    //         cout << "Eliminado con éxito!"<<endl;
    //     }
    //     else{
    //         break;
    //     }

    // }

    return 0;
}