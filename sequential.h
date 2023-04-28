#ifndef SEQUENTIAL_H
#define SEQUENTIAL_H

#define AddressType int32_t
#define INVALID 'x'
#define MAX_CAPACITY 5 // 5 records per auxiliary file

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <stdexcept>

using namespace std;

enum FileID {AUXFILE,DATAFILE}; // 0 para auxFile, 1 para dataFile
template <class Record, class Key>
class SequentialFile{
private:
    string DATAFILE_DP;
    string AUXFILE_DP;
    struct RecordData{
        AddressType pos;
        char ref;
        Record record;
        RecordData(){
            pos = -1;
        }
        RecordData(AddressType pos_, char ref_, Record record_){
            pos = pos_;
            ref = ref_;
            record = record_;
        }
    };
    void write_record(AddressType pos, fstream& file, Record& record, FileID FileID){
        if (!file.is_open())
            throw out_of_range("File not open @ write_record");
        if (DATAFILE == FileID)
            file.seekp(pos * sizeof(Record) + sizeof(AddressType) + sizeof(char) + sizeof(bool),ios::beg);
        else if (AUXFILE == FileID)
            file.seekp(pos*sizeof(Record),ios::beg);
        else
            throw invalid_argument("FileID invalid @ write_record");
        file.write((char*)&record, sizeof(Record));
    }
    void first_write_record_data(fstream& file, AddressType pos, char ref){
        if (!file.is_open())
            throw out_of_range("File not open @ first_write_record_data");
        file.seekp(0,ios::beg);
        file.write((char*)&pos, sizeof(AddressType));
        file.write((char*)&ref, sizeof(char));
    }

    void insert_with_sequential_search(Record& record){
        fstream dataFile(DATAFILE_DP,ios::binary | ios::in | ios::out);
        fstream auxFile(AUXFILE_DP,ios::binary | ios::in | ios::out);
        AddressType pos;
        char ref;
        first_read_record_data(dataFile,pos,ref);
        cout << "Key" << record.get_key() << endl;
        auto search_result = sequential_search(record.get_key());
        char curr_ref = search_result.second.ref;
        char prev_ref = search_result.first.ref;

        if (prev_ref == INVALID){
            first_write_record_data(dataFile,number_of_records(AUXFILE_DP,AUXFILE),'a');
        }
        if (curr_ref == INVALID && prev_ref == INVALID){
            record.nextDel = number_of_records(dataFile,DATAFILE);
            record.ref = INVALID;
        }
        else if (prev_ref == INVALID){
            record.nextDel = pos;
            record.ref = ref;
        }
        else{
            Record prev_record = search_result.first.record;
            AddressType prev_pos = search_result.first.pos;
            record.nextDel = prev_record.nextDel;
            record.ref = prev_record.ref;
            prev_record.nextDel = number_of_records(AUXFILE_DP,AUXFILE);
            prev_record.ref = 'a';
            if (prev_ref == 'd')
                write_record(prev_pos,dataFile,prev_record,DATAFILE);
            else if (prev_ref == 'a')
                write_record(prev_pos,auxFile,prev_record,AUXFILE);
            else
                throw invalid_argument("Invalid reference at second else @ insert_with_sequential_search");
        }
        auxFile.close();
        auxFile.open(AUXFILE_DP,ios::binary | ios::out | ios::app);
        auxFile.write((char*)&record, sizeof(Record));
        auxFile.close();
        dataFile.close();
    }


    void read_record(AddressType pos, fstream& file, Record& record, FileID FileID){
        if (!file.is_open())
            throw out_of_range("File not open @ read_record");
        if (DATAFILE == FileID)
            file.seekg(pos * sizeof(Record) + sizeof(AddressType) + sizeof(char) + sizeof(bool),ios::beg);
        else if (AUXFILE == FileID)
            file.seekg(pos*sizeof(Record),ios::beg);
        else
            throw invalid_argument("FileID invalid @ read_record");
        file.read((char*)&record, sizeof(Record));
    }
    void first_read_record_data(fstream& file, AddressType& pos, char& ref){
        if (file.is_open()){
            file.seekg(0,ios::beg);
            file.read((char*)&pos, sizeof(AddressType));
            file.read((char*)&ref, sizeof(char));
        }
        else{
            pos = -1;
            ref = INVALID;
        }
    }
    int number_of_records(fstream& file, FileID FileID){
        if (FileID != DATAFILE && FileID != AUXFILE)
            throw invalid_argument("FileID invalid @ number_of_records");
        if (file.is_open()){
            file.seekg(0,ios::end);
            int n = (FileID == DATAFILE) ?
                ((int) file.tellg() - sizeof(AddressType) -sizeof(char) - sizeof(bool)) /sizeof(Record):
                ((int) file.tellg() - sizeof(Record));
            file.seekg(0,ios::beg);
            return n;
        }
        return 0;
    }
    int number_of_records(string filename, FileID FileID){
        fstream file(filename,ios::binary |ios::in);
        auto n = number_of_records(file,FileID);
        file.close();
        return n;
    }
    static bool compare_records(Record& r1, Record& r2){
        return r1 < r2;
    }
    bool is_empty(FileID FileID){
        if (FileID == DATAFILE)
            return number_of_records(this->DATAFILE_DP,DATAFILE) == 0;
        else if(FileID == AUXFILE)
            return number_of_records(this->AUXFILE_DP,AUXFILE) == 0;
        else
            throw invalid_argument("FileID invalid @ is_empty (record)");
    }
    bool is_full(){
        if (number_of_records(this->AUXFILE_DP,AUXFILE) == MAX_CAPACITY)
            return true;
        return false;
    }
    bool is_removable(){
        AddressType pos;
        char ref;
        fstream dataFile(this->DATAFILE_DP,ios::binary | ios::in);
        first_read_record_data(dataFile,pos,ref);
        dataFile.close();
        return ref != INVALID;
    }
    void read_status_for_deleted_record(fstream&file, bool& status){
        if (!file.is_open())
            throw out_of_range("File not open @ read_status_for_deleted_record");
        file.seekg(sizeof(AddressType) + sizeof(char), ios::beg);
        file.read((char*)& status, sizeof(bool));
    }
    void write_status_for_deleted_record(fstream&file, bool status){
        if (!file.is_open())
            throw out_of_range("File not open @ write_status_for_deleted_record");
        file.seekp(sizeof(AddressType) + sizeof(char), ios::beg);
        file.write((char*)&status,sizeof(bool));
    }
public:
    SequentialFile(string DATAFILE_DP_, string AUXFILE_DP_){
        this->DATAFILE_DP = DATAFILE_DP_;
        this->AUXFILE_DP = AUXFILE_DP_;
    }
    void insert_all(vector<Record> &record){
        if (is_empty(DATAFILE)){
            sort(record.begin(), record.end(),compare_records);
            fstream file(this->DATAFILE_DP,ios::binary | ios::out);
            first_write_record_data(file,0,'d');
            write_status_for_deleted_record(file,false);
            for (auto i =0; i < record.size(); i++){
                record[i].nextDel = i + 1;
                if (i == record.size()-1)
                    record[i].ref = INVALID;
                else
                    record[i].ref = 'd';
                write_record(i,file,record[i],DATAFILE);
            }
            file.close();
        }
        else{
            for (AddressType i=0;i<record.size();i++)
                add_record(record[i]);
        }
    }

    void add_record(Record record){
        if (is_empty(DATAFILE)){
            fstream file(this->DATAFILE_DP, ios::binary | ios::out);
            record.nextDel = 1;
            record.ref = INVALID;
            first_write_record_data(file, 0, 'd');
            write_status_for_deleted_record(file,false);
            write_record(0, file, record, DATAFILE);
            file.close();
            return;
        }
        if (is_full()){
            auto records = load();

            remove(this->DATAFILE_DP.c_str());
            remove(this->AUXFILE_DP.c_str());

            fstream dataFile(this->DATAFILE_DP, ios::binary | ios::out);
            fstream auxFile(this->AUXFILE_DP, ios::binary | ios::out);

            first_write_record_data(dataFile,0,'d');
            write_status_for_deleted_record(dataFile,false);

            for (int i = 0; i < records.size(); i++)
                write_record(i,dataFile,records[i],DATAFILE);

            dataFile.close();
            auxFile.close();
        }
        fstream file(this->DATAFILE_DP,ios::binary | ios::in | ios::out);
        bool status;
        read_status_for_deleted_record(file,status);
        file.close();
        if (status){
            cout << "Entering if condition @ insertion with seq search" << endl;
            insert_with_sequential_search(record);
        }
        else{
            cout << "Entering else condition @ insertion with seq search" << endl;
            insert_with_sequential_search(record);
        }
    }

    pair<RecordData,RecordData> sequential_search(Key key){
        fstream dataFile(this->DATAFILE_DP,ios::binary | ios::in);
        fstream auxFile(this->AUXFILE_DP,ios::binary | ios::in);
        AddressType curr_pos, prev_pos = -1;
        char curr_ref, prev_ref = INVALID;
        first_read_record_data(dataFile,curr_pos,curr_ref);
        Record curr_record, prev_record;
        while (curr_ref != INVALID) {
            cout << "current reference" << curr_ref << endl;
            if(curr_ref == 'd')
                read_record(curr_pos,dataFile,curr_record,DATAFILE);
            else if(curr_ref == 'a')
                read_record(curr_pos,auxFile,curr_record,AUXFILE);
            else
                throw invalid_argument("Invalid reference @ sequential_search");
            if (!curr_record.less_than_key(key))
                break;
            prev_pos = curr_pos;
            prev_ref = curr_ref;
            prev_record = curr_record;
            curr_pos = curr_record.nextDel;
            curr_ref = curr_record.ref;
        }
        return  {RecordData(prev_pos,prev_ref,prev_record),RecordData(curr_pos,curr_ref,curr_record)};
    }
    vector<Record> search_record(Key key){
        vector<Record> result;
        auto search_result = sequential_search(key);
        Record curr_record = search_result.second.record;
        if (curr_record.equal_key(key)){
            result.push_back(curr_record);
            fstream dataFile(this->DATAFILE_DP,ios::binary | ios::in);
            fstream auxFile(this->AUXFILE_DP,ios::binary | ios::in);
            auto curr_pos = curr_record.nextDel;
            char curr_ref = curr_record.ref;
            while (curr_ref != INVALID){
                if (curr_ref == 'd')
                    read_record(curr_pos,dataFile,curr_record,DATAFILE);
                else if (curr_ref == 'a')
                    read_record(curr_pos,auxFile,curr_record,AUXFILE);
                else
                    throw invalid_argument("Invalid reference @ search_record");
                if (curr_record.equal_key(key))
                    result.push_back(curr_record);
                curr_pos = curr_record.nextDel;
                curr_ref = curr_record.ref;
            }
        }
        return result;
    }
    vector<Record> search_per_range(Key start, Key end){
        vector<Record> result;
        auto search_result = sequential_search(start);
        Record curr_record = search_result.second.record;
        auto curr_pos = search_result.second.pos;
        char curr_ref = search_result.second.ref;
        fstream dataFile(this->DATAFILE_DP,ios::binary | ios::in);
        fstream auxFile(this->AUXFILE_DP, ios::binary | ios::in);
        while (curr_ref != INVALID){
            if (curr_record.greater_or_equal(start) && curr_record.less_or_equal(end))
                result.push_back(curr_record);
            curr_pos = curr_record.nextDel;
            curr_ref = curr_record.ref;
            if (curr_ref == 'd'){
                read_record(curr_pos,dataFile,curr_record,DATAFILE);
            }
            else if (curr_ref == 'a'){
                read_record(curr_pos,auxFile,curr_record,AUXFILE);
            }
        }
        sort(result.begin(),result.end(),compare_records);
        return result;
    }

    void remove_record(Key key){
        if (!is_removable())
            return;
        auto search_result = sequential_search(key);
        Record curr_record = search_result.second.record;
        if (!curr_record.equal_key(key))
            return;
        fstream dataFile(this->DATAFILE_DP,ios::binary | ios::in | ios::out);
        fstream auxFile(this->AUXFILE_DP,ios::binary | ios::in | ios::out);

        auto prev_pos = search_result.first.pos;
        auto prev_ref = search_result.first.ref;
        Record prev_record = search_result.first.record;

        auto curr_pos = search_result.second.pos;
        auto curr_ref = search_result.second.ref;

        auto next_pos = curr_record.nextDel;
        auto next_ref = curr_record.ref;

        if (prev_ref == INVALID){
            first_write_record_data(dataFile,next_pos,next_ref);
        }else{
            prev_record.nextDel = next_pos;
            prev_record.ref = next_ref;
            if (prev_ref == 'd')
                write_record(prev_pos,dataFile,prev_record,DATAFILE);
            else if (prev_ref == 'a')
                write_record(prev_pos,auxFile,prev_record,AUXFILE);
            else
                throw invalid_argument("Invalid reference @ remove_record");
        }

        curr_record.nextDel = -1;
        curr_record.ref = INVALID;

        if (curr_ref == 'd')
            write_record(curr_pos,dataFile,curr_record,DATAFILE);
        else if (curr_ref == 'a')
            write_record(curr_pos,auxFile,curr_record,AUXFILE);
        else
            throw invalid_argument("Invalid reference @ remove_record");

        write_status_for_deleted_record(dataFile,true);
        dataFile.close();
        auxFile.close();
    }

    vector<Record> load(){
        fstream dataFile(this->DATAFILE_DP,ios::binary | ios::in | ios::out);
        fstream auxFile(this->AUXFILE_DP,ios::binary | ios::in | ios::out);
        int curr_pos, next_pos, i =0;
        char curr_ref, next_ref;
        first_read_record_data(dataFile,curr_pos,curr_ref);
        vector<Record> records;
        vector<Record> unordered_records;
        while (curr_ref != INVALID){
            Record record;
            if (curr_ref == 'd')
                read_record(curr_pos,dataFile,record,DATAFILE);
            else if (curr_ref =='a')
                read_record(curr_pos,auxFile,record,AUXFILE);
            else
                throw invalid_argument("Invalid reference @ load()");

            next_pos = record.nextDel;
            next_ref = record.ref;

            if (curr_ref == 'a')
                unordered_records.push_back(record);
            else{
                ++i;
                record.nextDel = i;
                record.ref = 'd';
                records.push_back(record);
            }

            if (next_ref == 'd' || next_ref == INVALID){
                sort(unordered_records.begin(), unordered_records.end(),compare_records);
                for (auto&r : unordered_records){
                    ++i;
                    record.nextDel = i;
                    record.ref = 'd';
                }
                records.insert(records.end(),unordered_records.begin(),unordered_records.end());
                unordered_records.clear();
            }
            curr_ref = next_ref;
            curr_pos = next_pos;
        }

        if (!unordered_records.empty())
            cout << "Unordered records not empty @ load()" << endl;

        if (!records.empty())
            records[records.size()-1].ref = INVALID;

        return records;
    }
};




#endif // SEQUENTIAL_H

