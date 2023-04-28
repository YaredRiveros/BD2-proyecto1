#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
//#include "record.h"
#include "sequential.h"
using namespace std;

class MyRecord {
public:
    int key;
    int value;
    int nextDel;
    char ref;

    MyRecord() : key(-1), value(-1), nextDel(-1), ref(INVALID) {}
    MyRecord(int k, int v) : key(k), value(v), nextDel(-1), ref(INVALID) {}

    int get_key() const { return key; }
    bool less_than_key(int k) const { return key < k; }
    bool equal_key(int k) const { return key == k; }
    bool greater_or_equal(int k) const { return key >= k; }
    bool less_or_equal(int k) const { return key <= k; }

    bool operator<(const MyRecord& other) const { return key < other.key; }
};

int main() {
    SequentialFile<MyRecord, int> seqFile("datafile.bin", "auxfile.bin");

    // Inserta registros en el archivo secuencial
    cout << "Inserción de registros" << endl;
    seqFile.add_record(MyRecord(1, 100));
    seqFile.add_record(MyRecord(2, 200));
    seqFile.add_record(MyRecord(3, 300));

    // Busca registros por clave
    cout << "Búsqueda de registros" << endl;
    auto records = seqFile.search_record(2);
    for (const auto& record : records) {
        std::cout << "Record found: key=" << record.key << ", value=" << record.value << std::endl;
    }

    // Elimina un registro por clave
    cout << "Eliminación de registro 2" << endl;
    seqFile.remove_record(2);

    return 0;
}