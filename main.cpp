#include <iostream>
#include "extendibleHash.h"
#include "Interfaz.h"
#include <regex>

using namespace std;




//FUNCIONES SQL

long parseDeleteSql(const std::string &sql) {
    std::regex deleteRegex(R"(DELETE\s+FROM\s+SeqFile\s+WHERE\s+id\s*=\s*(\d+))", std::regex_constants::icase);
    std::smatch matches;

    if (std::regex_search(sql, matches, deleteRegex)) { //Estoy buscando en sql, matches es donde se guarda el resultado, deleteRegex es la expresion regular
        cout << "match hallado: " << matches[1].str() << endl;
        return std::stol(matches[1].str());
    } else {
        throw std::runtime_error("Invalid SQL syntax for DELETE.");
    }
}

pair<int,int> parseSelectWhere(const std::string &sql) {
    std::regex selectRegex(R"(SELECT\s+id\s+FROM\s+SeqFile\s+WHERE\s+id\s+BETWEEN\s+(\d+)\s+AND\s+(\d+))", std::regex_constants::icase);
    std::smatch matches;

    if (std::regex_search(sql, matches, selectRegex)) {
        return {std::stoi(matches[1].str()), std::stoi(matches[2].str())};
    } else {
        throw std::runtime_error("Invalid SQL syntax for SELECT.");
    }
}

class Database {
public:
    std::string mainFilename;
    std::string auxFilename;
    ExtendibleHash hash;

public:
    Database(const std::string &mainFilename, const std::string &auxFilename)
        : mainFilename(mainFilename), auxFilename(auxFilename) {
            //Leer toda la información del archivo mainFilename y cargarla en el hash.

            //Borro los datos de los archivos
            fstream indices("Indice.bin", ios::out | ios::binary);
            fstream datos("Datos.bin",ios::out | ios::binary);
            indices.close();
            datos.close();

            string csv_file_name = "data.csv";
            ExtendibleHash eHash("Indice.bin", "Datos.bin");
            cout << "Cargando datos desde " << csv_file_name << "..." << endl;
            eHash.LoadCsv(csv_file_name);
            cout << "Datos cargados exitosamente." << endl;
            //Leer toda la información para el sequential

            //Borro los datos de los archivos
            fstream indices2("main2.dat", ios::out | ios::binary);
            fstream datos2("aux2.dat",ios::out | ios::binary);
            indices2.close();
            datos2.close();
            
            fstream mainIn(mainFilename, ios::out | ios::binary); 
            mainIn.close();
            fstream auxFile(auxFilename, ios::out | ios::binary);
            auxFile.close();
            int K = 0;
            long beginId, endId;
            std::vector<RecordSequential> records = readCSV("data.csv");
            for (const auto &record : records) {
                insertRecordSequential(record, mainFilename, auxFilename, K);
            }
        
        }


    void deleteRecordS(const std::string &sql) {
        long id = parseDeleteSql(sql);
        ::deleteRecordSequential(id, mainFilename, auxFilename);
    }

    void deleteRecordE(const std::string &sql) {
        long id = parseDeleteSql(sql);
        hash.deleteRecord(id);
    }


    void selectAllRecordsS(const std::string &sql) {
        displayAllRecordsSequential(mainFilename);
    }

    void selectAllRecordsE(const std::string &sql) {
        hash.displayAll();
    }

    void selectWhere(const std::string &sql) {
        pair<int,int> ids = parseSelectWhere(sql);
        vector<RecordSequential> records = searchRangeSequential(ids.first, ids.second, mainFilename, auxFilename);
        for (const auto &record : records) {
            record.display();
            std::cout << "------------------------" << std::endl;
        }
    }
};

using namespace std;

int main(){
    int opcion = 0;
    Database db("main2.dat", "aux2.dat"); //nombres de los archivos que usa el sequential, el hash no los usa
    while(opcion!=3){

        cout << "-----------MENU-----------" << endl;
        cout << "Seleccione con que técnica desea trabajar: " << endl;
        cout << "1. Sequential File" << endl;
        cout << "2. Extendible Hash" << endl;
        cout << "3. Salir" << endl;
        cout << "Ingrese opcion: ";
        cin >> opcion;

        if(opcion==1){
            cout << "1. Ingresar consulta SQL(DELETE O SELECT ALL O BUSQUEDA POR RANGO)" << endl; //Solo soporta delete y select all
            cout << "2. Buscar" << endl;
            cout << "3. Insertar" << endl;
            cout << "4. Salir" << endl;
            cout << "Ingrese opcion: ";
            int opcion2; cin>>opcion2;
            if(opcion2==1){
                string sql;
                cout << "Ingrese consulta SQL: "; getline(cin,sql);
                try
                {
                    if(sql.find("DELETE")!=string::npos){
                        db.deleteRecordS(sql);
                        std::cout << "Registro eliminado correctamente." << std::endl;
                    }
                    else if(sql.find("SELECT ALL")!=string::npos){
                        db.selectAllRecordsS(sql);
                    }
                    else if(sql.find("BETWEEN")!=string::npos){
                        db.selectWhere(sql);
                    }
                    else{
                        cout << "Consulta invalida" << endl;
                    }
                }
                catch (const std::runtime_error &e)
                {
                    std::cerr << "Error: " << e.what() << std::endl;
                }
            }
            else if(opcion==2){
                //busqueda por igualdad
                long id;
                cout << "Ingrese id a buscar: "; cin>>id;
                RecordSequential *record = searchRecordSequential(id, "main2.dat", "aux2.dat");
                if (record != nullptr) {
                    cout << "Se insertó correctamente" << endl;
                } else {
                    std::cout << "No se encontró el registro." << std::endl;
                }
            }
            else if(opcion==3){
                RecordSequential record;
                cout << "Ingrese id: "; long int id; cin>>id;
                cin.ignore();
                cout << "Ingrese nombre: "; string nombre; getline(cin,nombre);
                for(int i=0;i<20-nombre.size();i++){
                    record.nombre[i] = nombre[i];
                }
                cout << "Ingrese apellido: "; string correo; getline(cin,correo);
                for(int i=0;i<20-correo.size();i++){
                    record.correo[i] = correo[i];
                }
                cin.ignore();
                cout << "Ingrese edad: "; int ciclo; cin>>ciclo;
                cout << "Ingrese la carrera"; string carrera; getline(cin,carrera);
                for(int i=0;i<20-carrera.size();i++){
                    record.carrera[i] = carrera[i];
                }
                cout << "Ingrese el codigo"; long int codigo; cin>>codigo;
                
                insertRecordSequential(record, "main2.dat", "aux2.dat",0);
                std::cout << "Se insertó correctamente" << std::endl;

            }
            else{
                cout << "Opcion invalida" << endl;
            }
        }
        else if(opcion==2){
            cout << "1. Ingresar consulta SQL(DELETE O SELECT ALL O RANGE SEARCH)" << endl; //Solo soporta delete y select all
            cout << "2. Buscar" << endl;
            cout << "3. Insertar" << endl;
            cout << "4. Salir" << endl;
            cout << "Ingrese opcion: ";
            int opcion2; cin>>opcion2;
            if(opcion2==1){
                string sql;
                cout << "Ingrese consulta SQL: "; getline(cin,sql);
                try
                {
                    if(sql.find("DELETE")!=string::npos){
                        db.deleteRecordE(sql);
                        std::cout << "Registro eliminado correctamente." << std::endl;
                    }
                    else if(sql.find("SELECT ALL")!=string::npos){
                        db.selectAllRecordsE(sql);
                    }
                    else if(sql.find("BETWEEN")!=string::npos){
                        db.selectWhere(sql);
                    }
                    else{
                        cout << "Consulta invalida" << endl;
                    }
                }
                catch (const std::runtime_error &e)
                {
                    std::cerr << "Error: " << e.what() << std::endl;
                }
            }
            else if(opcion2==2){
                long id ;
                cout << "Ingrese id a buscar: "; cin>>id;
                Record record = db.hash.searchRecord(id);
                record.display();

            }
            else if(opcion2==3){
                Record record;

                cout << "Ingrese id: "; long int id; cin>>id;
                cin.ignore();
                cout << "Ingrese nombre: "; string nombre; getline(cin,nombre);
                for(int i=0;i<20-nombre.size();i++){
                    record.nombre[i] = nombre[i];
                }
                cout << "Ingrese apellido: "; string correo; getline(cin,correo);
                for(int i=0;i<20-correo.size();i++){
                    record.correo[i] = correo[i];
                }
                cin.ignore();
                cout << "Ingrese edad: "; int ciclo; cin>>ciclo;
                cout << "Ingrese la carrera"; string carrera; getline(cin,carrera);
                for(int i=0;i<20-carrera.size();i++){
                    record.carrera[i] = carrera[i];
                }
                cout << "Ingrese el codigo"; long int codigo; cin>>codigo;

                db.hash.insertRecord(record);
                cout << "Se insertó correctamente" << endl;
                
                
            }
            else if(opcion2==4){
                break;
            }
            else{
                cout << "Opcion invalida" << endl;
            }

        }
        else if(opcion==3){
            break;
        }
        else{
            cout << "Opcion invalida" << endl;   
        }

    }



    return 0;
}