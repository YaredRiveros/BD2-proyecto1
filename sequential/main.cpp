#include <iostream>
#include <fstream>
#include <string.h>
#include <sstream>
#include <vector>
#include "record_seq.h"
#include "sequentialFile.h"
using namespace std;

int main()
      {
	int Choice;
	char F[15];
	int R;
    string csv_file_name = "data.csv";
  
	cout << "\nIntroduzca el nombre del archivo: ";
  cin >> F;
	Seq_File  sFile(F);  
  sFile.Load_Data_From_CSV(csv_file_name);
	do
	{
cout<<"\n1: Crear base de datos\n2: Mostrar base de datos\n3: Añadir un registro\n4: Eliminar un registro\n5: Modificar un registro\n6: Buscar registros por ID\nIngrese su opción:  ";
	  cin>>Choice;
	  switch (Choice) {
            case 1:
                sFile.Create();
                break;
            case 2:
                sFile.Display();
                break;
            case 3:
                sFile.Add();
                break;
            case 4:
                cout << "\nIntroduzca el ID para eliminar: ";
                cin >> R;
                sFile.Remove(R);
                break;
            case 5:
                cout << "\nIntroduzca el ID para modificar: ";
                cin >> R;
                sFile.Modify(R);
                break;
            case 6:
                cout << "\nIntroduzca el ID para buscar: ";
                cin >> R;
                vector<Record> search_results = sFile.search(R);
                if (search_results.empty()) {
                    cout << "\nNo se encontraron registros con el ID: " << R;
                } else {
                    cout << "\nResultados de la búsqueda:";
                    for (const auto &result : search_results) {
                        result.put_Data();
                    }
                }
                break;
        }
    } while (Choice < 7);
    cout << "\n";
    return 1;
}