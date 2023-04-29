#include <iostream>
#include <fstream>
#include <string.h>
#include <sstream>
#include <vector>


using namespace std;


class Seq_File
	{
		char File_Name[15];
		public:
		Seq_File();
		Seq_File(const char F[]);		
		void Create();
		void Display();
		void Add();
		void Remove(int Id);
		void Modify(int Id);
    void Load_Data_From_CSV(const string& file_name);
    vector<Record> search(int Id);
	};
Seq_File::Seq_File()
	{
		    ofstream  File("Student.txt");
		    strcpy(File_Name," Student.txt");
		    cout<<"\nConstructor estandar";
		    if(File)
		       {
		          cout<<"\nArchivo abierto con éxito";
		          File.close();
		       }
		    else cout<<"\nError al crear el archivo";   
		
	}
Seq_File::Seq_File(const char F[15]) {
    ofstream File;
    strcpy(File_Name, F);
    File.open(F);
    if (File) {
        cout << "\nArchivo abierto con éxito";
        File.close();
    } else {
        cout << "\nError al crear el archivo";
    }
}
void Seq_File::Create()
	{
		ofstream  File;
		Record S;
		File.open(File_Name);
		S.get_Data();
		File.write(reinterpret_cast<char*>(&S),sizeof(S));
		File.close();
	}
	
void Seq_File::Display()
	{
		ifstream  File;
		Record S;
		File.open(File_Name);
		cout<<"\nRoll No\t Student Name";
		File.read(reinterpret_cast<char*>(&S),sizeof(S));
		while(!File.eof())
		            {
                      S.put_Data();
                      File.read(reinterpret_cast<char*>(&S),sizeof(S));
		            }
		File.close();
		
	}
void Seq_File::Add()
	{
		ofstream  File;
		Record S;
		File.open(File_Name,ios::app);
		S.get_Data();
		File.write(reinterpret_cast<char*>(&S),sizeof(S));
		File.close();
	}	
void Seq_File::Remove(int Id)
	{
		 ifstream File;
		 ofstream Temp;
		 Record S;
		 int Flag=0;
		 File.open(File_Name);
		 Temp.open("Temp.Txt");
		 File.read(reinterpret_cast<char*>(&S),sizeof(S));
		 while(!File.eof())
		            {
		                     if(Id==S.return_Id())
		                         {   S.put_Data(); Flag=1;}
		                      else
               Temp.write(reinterpret_cast<char*>(&S),sizeof(S));
               File.read(reinterpret_cast<char*>(&S),sizeof(S));
		            }
 if(Flag==0) cout<<"ID No. "<<Id<<" no existente \n";      
		File.close();
		Temp.close();
		remove(File_Name);
		rename("Temp.Txt",File_Name); 
	}
void Seq_File::Modify(int Id)
	{
		 ifstream File;
		 ofstream Temp;
		 Record S;
		 int Flag=0;
		 File.open(File_Name);
		 Temp.open("Temp.Txt");
		 File.read(reinterpret_cast<char*>(&S),sizeof(S));
		while(!File.eof())
		            {
		                     if(Id==S.return_Id())
		                          {
		                                S.put_Data();
                                cout<<"\n Ingresa data a modificar";
		                                S.get_Data();
		                                Flag=1;                                
		                           }     
             Temp.write(reinterpret_cast<char*>(&S),sizeof(S));
             File.read(reinterpret_cast<char*>(&S),sizeof(S));
		            }
 if(Flag==0) cout<<"Id No. "<<Id<<" no existente \n";      
		File.close();
		Temp.close();
		remove(File_Name);
		rename("Temp.Txt",File_Name); 
	}

void Seq_File::Load_Data_From_CSV(const string& file_name)
{
    ifstream file(file_name);
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        long int id;
        string nombre, apellido, carrera;
        int ciclo, codigo;
        getline(ss, line, ',');
        id = stol(line);
        getline(ss, nombre, ',');
        getline(ss, apellido, ',');
        ss >> ciclo;
        ss.ignore();
        getline(ss, carrera, ',');
        ss >> codigo;
        Record R(id, nombre.c_str(), apellido.c_str(), ciclo, carrera.c_str(), codigo);
        ofstream out(File_Name, ios::app);
        out.write(reinterpret_cast<char*>(&R),sizeof(R));
        out.close();
    }
    file.close();
}

vector<Record> Seq_File::search(int Id) {
    vector<Record> results;
    ifstream File;
    Record S;
    File.open(File_Name);

    if (!File) {
        cerr << "Error al abrir el archivo." << endl;
        return results;
    }

    File.read(reinterpret_cast<char*>(&S), sizeof(S));
    while (!File.eof()) {
        if (S.return_Id() == Id && !S.is_Deleted()) {
            results.push_back(S);
        }
        File.read(reinterpret_cast<char*>(&S), sizeof(S));
    }

    File.close();
    return results;
}