#include <iostream>
#include "extendibleHash.h"

using namespace std;

int main(){
    ExtendibleHash eh("index.bin", "records.bin");
    eh.insertRecord(Record(1, "nombre1", "apellido1", 1, "carrera1", 1));

    cout << "Aun no acaba el main" << endl;
    return 0;
}