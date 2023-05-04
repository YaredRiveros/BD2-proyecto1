#include <iostream>
#include "Interfaz.h"

using namespace std;

int main(){
    Interfaz interfaz;

    int opcion = 0;
    while(opcion!=6){
        cout << "1. Ingresar consulta SQL (carga de datos)" << endl;
        cout << "2. Insertar" << endl;
        cout << "3. Eliminar" << endl;
        cout << "4. Búsqueda exacta" << endl;
        cout << "5. Búsqueda por rango" << endl;
        cout << "6. Salir" << endl;
        cout << "Ingrese opcion: ";
        cin >> opcion;
        switch(opcion){
            case 1:
                interfaz.leerConsulta();
                break;
            case 2:
                interfaz.insert();
                break;
            case 3:
                interfaz.deleteRecord();
                cout << "Saliendo..." << endl;
                break;
            case 4:
                interfaz.search();
                cout << "Opcion invalida" << endl;
                break;
            case 5:
                interfaz.rangeSearch();
        }
    }



    return 0;
}