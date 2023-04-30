#include <iostream>
#include <fstream>

using namespace std;
struct ejemplo{
    int a ;
    char b[3];

    ejemplo(){
        a = 0;
        b[0] = '0';
        b[1] = '0';
        b[2] = '0';
    }
};


int main(){
    //Crear archivo de lectura y escritura binario
    ofstream file;
    file.open("ceñora.bin", ios::out | ios::binary | ios::app);
    ejemplo ej;
    
    ej.a = 1;
    ej.b[0] = 'a';
    ej.b[1] = 'b';
    ej.b[2] = 'c';

    file.write((char*)&ej, sizeof(ejemplo));
    
    
    ej.a += 2;
    ej.b[0] = 'd';
    ej.b[1] = 'e';
    ej.b[2] = 'f';


    file.write((char*)&ej, sizeof(ejemplo));

    file.close();

    ifstream file2;
    file2.open("ceñora.bin", ios::in | ios::binary);

    //leemos para verificar cambios
    cout << "--Lectura ejemplo1--" << endl;
    ejemplo ej2;
    file2.seekg(0, ios::beg);
    file2.read((char*)&ej2, sizeof(ejemplo));
    cout << ej2.a << " " << ej2.b[0] << ej2.b[1] << ej2.b[2] << endl;

    cout << "--Lectura ejemplo2--" << endl;
    file2.read((char*)&ej2, sizeof(ejemplo));
    cout << ej2.a << " " << ej2.b[0] << ej2.b[1] << ej2.b[2] << endl;

    return 0;
}