#ifndef HASH_H
#define HASH_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <stdexcept>

#define BLOCKFACTOR 10

/// M >= (n/fb)*(1+d) --> cantidad de buckets

using namespace std;

template<class Record, class Key>
class Bucket {
private:
    Record arrayRecords[BLOCKFACTOR];
    int nextBucket;
    int count;

public:
    int nextDel;
    Bucket():nextBucket(-1), count(0), nextDel(0) {}
    Bucket(int _nextBucket_):nextBucket(_nextBucket_), count(0), nextDel(0) {}
    int getNextBucket() const{
        return nextBucket;
    }
    int getNextDel(){
        return nextDel;
    }
    void setNextDel(int pos){this->nextDel=pos;}
    void setNextBucket(int nextBucket){
        nextBucket = nextBucket;
    }

    void addRecord(Record record){
        if(count < BLOCKFACTOR){
            arrayRecords[count]=record;
            count++;
        }
        else
            throw out_of_range("Bucket is Full!");
    }
    void setRecords(vector<Record> vector){
        count = 0;
        for(count = 0; count<vector.size(); count++)
            arrayRecords[count]=vector[count];
    }
    void sortBucket(){
        sort(arrayRecords, arrayRecords+count, compareRecords);
    }
    bool isFull(){
        return count==BLOCKFACTOR;
    };
    bool isEmpty(){
        return count==0;
    }
    static bool compareRecords(Record& recordA, Record& recordB){
        return recordA<recordB;
    }
    vector<Record> getRecords(){
        return vector<Record>(arrayRecords, arrayRecords+count);
    }
    vector<Record> getAllDifferentRecords(Key key){
        vector<Record> output;
        for(auto& record : this->getRecords()){
            if(!record.equal_key(key))
                output.push_back(record);
        }
        return output;
    }
};

#include <unistd.h>
using namespace std;

template<class Record>
class FreeList {
private:
    string fileName;
    int readHeader(fstream& file){
        int header;
        file.seekg(0, ios::beg);
        file.read((char*)&header, sizeof(int));
        return header;
    }
    void writeHeader(int header, fstream& file){
        file.seekp(0, ios::beg);
        file.write((char*)& header, sizeof(int));
    }
    void writeRecord(Record& record, int pos, fstream& file){
        file.seekp(pos*sizeof(Record)+ sizeof(int), ios::beg);
        file.write((char*)& record, sizeof(Record));
    }
    Record readRecord(unsigned int pos, fstream& file){
        Record record;
        file.seekg(pos*sizeof(Record)+sizeof(int), ios::beg);
        file.read((char*)& record, sizeof(Record));
        return record;
    }
    int length(fstream& file){
        file.open(fileName, ios::ate|ios::binary|ios::in);
        if(!file.is_open())
            throw new exception;
        int bytes = (int)file.tellg() - sizeof(int);
        return bytes/ sizeof(Record);
    }

public:
    FreeList(){};
    FreeList(string _fileName_): fileName(_fileName_){}
    int length(){
        fstream file;
        return length(file);
    }

    vector<Record> scanAll(){
        vector<Record> vector;
        fstream file(fileName, ios::in|ios::binary);
        if(!file.is_open())
            throw new exception;
        Record temp;
        file.seekg(sizeof(int), ios::beg);
        while(!file.eof()){
            file.read((char*)& temp, sizeof(Record));
            if(!temp.nextDel)
                vector.push_back(temp);
        }
        file.close();
        return vector;
    }
    ///Al agregar un registro, devuelve la posicion logica del registro agregado.
    int addRecord(Record record) {
        //Fuente:https://www.delftstack.com/es/howto/c/c-check-if-file-exists/
        if(!access(fileName.c_str(), F_OK)){
            fstream file(fileName, ios::in|ios::out|ios::binary);
            if(!file.is_open()){
                throw new exception;
            }
            int header = readHeader(file);
            int recordPosition;
            if( (int)header == -1) {
                recordPosition = length(file);
                file.close();
                file.open(fileName, ios::out|ios::binary);
                writeRecord(record, recordPosition, file);
            }
            else{
                Record temp = readRecord(header,file);
                recordPosition = header;
                header = temp.nextDel;
                writeHeader(header,file);
                writeRecord(record, recordPosition, file);
            }
            file.close();
            return recordPosition;
        }
        else{
            fstream file(fileName, ios::out|ios::binary);
            int header = -1;
            writeHeader(header,file);
            writeRecord(record,0,file);
            return 0;
        }
    }
    Record readRecord(int pos){
        fstream file(fileName, ios::in|ios::binary);
        int numRecords = length(file);
        if(numRecords == 0){
            cout<<"Could not read record in this position, because empty file!" << endl;
            throw new exception;
        }
        if( 0<=pos && pos<numRecords){
            Record record = readRecord(pos, file);
            file.close();
            return record;
        }
        else
        {cout<<"Could not read record, because position is wrong!" << endl;
            throw new exception;}
    }
    void writeRecord(Record record, int pos){
        fstream file(fileName, ios::in|ios::binary);
        int numRecords = length(file);
        if(numRecords==0){
            cout << "Could not write record, because empty file!" << endl;
            return;
        }
        if(pos<numRecords && pos >=0){
            file.close();
            file.open(fileName, ios::in|ios::binary);
            writeRecord(record,pos,file);
            file.close();
        }
        else{
            cout<<"Could not write record, because position is wrong!" << endl;
            return;
        }
    }
    bool deleteRecord(int pos){
        fstream file(fileName, ios::in|ios::out|ios::binary);
        int numRecords = length(file);
        if(numRecords==0){
            cout << "Could not delete record, because empty file!" << endl;
            return false;
        }

        if(pos< numRecords && pos>=0){
            int header = readHeader(file);
            writeHeader(pos, file);
            Record record = readRecord(pos,file);
            record.nextDel = header;
            writeRecord(record, pos, file);
            file.close();
            return true;
        }
        else{
            cout<<"Could not delete record, because position is wrong!"<<endl;
            return false;
        }
    }
    void setFileName(string fileName){
        FreeList::fileName = fileName;
    }
};

#define MAXHEIGHT 3

using namespace std;

struct HashNode{
private:
    int position;
public:
    int left, right;
    int nextDel=0;
    bool isLeaf=true;

    HashNode() = default;
    HashNode(int _left_, int _right_):left(_left_), right(_right_), isLeaf(false){}
    HashNode(int _position_):position(_position_){}
    int getPosition(){
        if(!isLeaf){
            cout<<"This node isnt a leaf!"<<endl;
            throw new exception;
        }
        return this->position;
    }
    void setPosition(int _position_){
        if(!isLeaf){
            cout<<"This node isnt a leaf!"<<endl;
            throw new exception;
        }
        this->position = _position_;
    }
};

template<class Record, class Key, class Hash=hash<Key>>
class ExtendibleHash{
private:
    Hash hash;
    FreeList<Bucket<Record, Key>> bucketFile;
    FreeList<HashNode> indexFile;
public:
    ExtendibleHash()= default;

    void setBucketName(string bucketFileName){
        bucketFile.setFileName(bucketFileName);
    }
    void setIndexName(string indexFileName){
        indexFile.setFileName(indexFileName);
    }

    void insertAll(vector<Record> records){
            if(bucketFile.length() == 0)
                initializerValues();

            for(auto& record : records){
                insert(record);
            }
        }


    vector<Record> scanAll(){
        int numBuckets = bucketFile.length();
        vector<Record> result;
        for(int i=0; i<numBuckets; i++){
            Bucket<Record, Key> temp = bucketFile.readRecord(i);
            if(temp.getNextDel()==-1)
                continue;
            for(auto& item: temp.getRecords())
                result.push_back(item);
        }
        sort(result.begin(), result.end(), Bucket<Record,Key>::compareRecords);
        return result;
    }
    vector<Record> searchRange(Key beginKey, Key endKey){
        int numBuckets = bucketFile.length();
        vector<Record> result;
        for(int i = 0; i < numBuckets; ++i){
            Bucket<Record, Key> bucket = bucketFile.readRecord(i);
            for(auto& item : bucket.getRecords()){
                if(item.greater_or_equal(beginKey) && item.less_or_equal(endKey)) {
                    result.push_back(item);
                }
            }
        }
        sort(result.begin(), result.end(), Bucket<Record, Key>::compareRecords);
        return result;
    }
    vector<Record> search(Key key){

        bitset<MAXHEIGHT> hashKey = hash(key);
        int height = 0;
        int curNodePos = searchNode(0, hashKey, height);
        HashNode curNode = indexFile.readRecord(curNodePos);

        int curBucketPos = curNode.getPosition();
        vector<Record> result;
        while(curBucketPos != -1){
            Bucket<Record, Key> bucket = bucketFile.readRecord(curBucketPos);

            for(auto& item: bucket.getRecords()){
                if(item.equal_key(key)) {
                    result.push_back(item); }
            }
            //current toma el valor del siguiente bucket
            curBucketPos = bucket.getNextBucket();
        }
        return result;
    }
    void initializerValues(){
        HashNode root(1,2), A(0), B(1);
        Bucket<Record, Key> bucket1, bucket2;
        bucketFile.addRecord(bucket1);
        bucketFile.addRecord(bucket2);
        indexFile.addRecord(root);
        indexFile.addRecord(A);
        indexFile.addRecord(B);
    }
    int searchNode(int nodePosition, bitset<MAXHEIGHT> key, int height){
        HashNode node = indexFile.readRecord(nodePosition);
        if(!node.isLeaf)
            if(key[height]!=0)
                return searchNode(node.right, key, ++height);
            else
                return searchNode(node.left, key, ++height);
        else
            return nodePosition;
    }
    void setValues(vector<Record> records){
        if(bucketFile.length()==0)
            initializerValues();
        for(auto& record: records)
            insert(records);
    }
    void remove(Key key){
        int height = 0;

        remove(0, key, hash(key), height);
    }
    void insert(Record record){
        if(bucketFile.length()==0) {
            initializerValues();
        }
        int height =0;
        bitset<MAXHEIGHT> hashKey = hash(record.get_key());
        int curNodePos  = searchNode(0, hashKey, height);
        HashNode curNode = indexFile.readRecord(curNodePos);
        int curBucketPos = curNode.getPosition();
        Bucket<Record, Key> bucket = bucketFile.readRecord(curBucketPos);

        if(!bucket.isFull()){
            bucket.addRecord(record);
            bucket.sortBucket();
            bucketFile.writeRecord(bucket, curBucketPos);
        }
        else {
            split(record, height, curNodePos, curNode, curBucketPos, bucket);
        }
    }
    void split(Record& record, int height, int curNodePos, HashNode& currentNode, int curBucketPos, Bucket<Record, Key>& bucket){
        if(height == MAXHEIGHT){
            Bucket<Record, Key> temp;
            temp.addRecord(record);
            temp.setNextBucket(curBucketPos);
            currentNode.setPosition(bucketFile.addRecord(temp));
            indexFile.writeRecord(currentNode, curNodePos);
            return;
        }
        Bucket<Record, Key> bucket1, bucket2;
        bool splitAgain = false;
        bool splitToLeft = false;
        vector<Record> records = bucket.getRecords();
        records.push_back(record);

        for(auto& item : records){
            bitset<MAXHEIGHT> hashKey = hash(item.get_key());
            if(hashKey[height] == 0){
                if(!bucket1.isFull()) {
                    bucket1.addRecord(item);
                }
                else{
                    splitAgain = true;
                    splitToLeft = true;
                }
            }
            else{
                if(!bucket2.isFull()) {
                    bucket2.addRecord(item);
                }
                else {
                    splitAgain = true;
                }
            }
        }
        bucketFile.deleteRecord(curBucketPos);
        bucket1.sortBucket();
        bucket2.sortBucket();
        int bucketPos1 = bucketFile.addRecord(bucket1);
        int bucketPos2 = bucketFile.addRecord(bucket2);
        HashNode leftNode(bucketPos1), rightNode(bucketPos2);
        currentNode.left = indexFile.addRecord(leftNode);
        currentNode.right = indexFile.addRecord(rightNode);
        currentNode.isLeaf = false;
        indexFile.writeRecord(currentNode,curNodePos);
        if(splitAgain){
            int splitNodePos;
            HashNode splitNode;
            if(splitToLeft) {
                splitNodePos = currentNode.left;
                splitNode = indexFile.readRecord(splitNodePos);
                split(record, height+1, splitNodePos, splitNode, bucketPos1, bucket1);
            }else{
                splitNodePos = currentNode.right;
                splitNode = indexFile.readRecord(splitNodePos);
                split(record, height+1, splitNodePos, splitNode, bucketPos2, bucket2);
            }
        }
    }
    void remove(int nodePos, Key key, bitset<MAXHEIGHT> hashKey, int& height){
        HashNode currentNode = indexFile.readRecord(nodePos);
        if (currentNode.isLeaf) {
            deleteRecordBucket(key, currentNode, nodePos);
            return;
        }
        if(hashKey[height] == 0) {
            remove(currentNode.left, key, hashKey, ++height);
        }
        else {
            remove(currentNode.right, key, hashKey, ++height);
        }

        if(nodePos == 0) return;
        HashNode left = indexFile.readRecord(currentNode.left);
        HashNode right = indexFile.readRecord(currentNode.right);

        if (left.isLeaf && right.isLeaf) {
            Bucket<Record, Key> bucket1 = bucketFile.readRecord(left.getPosition());
            Bucket<Record, Key> bucket2 = bucketFile.readRecord(right.getPosition());
            if (bucket1.isEmpty() && bucket2.isEmpty()) {
                bucketFile.deleteRecord(left.getPosition());
                bucketFile.deleteRecord(right.getPosition());
                indexFile.deleteRecord(currentNode.left);
                indexFile.deleteRecord(currentNode.right);
                Bucket<Record, Key> tempBucket;
                currentNode.isLeaf = true;
                currentNode.setPosition(bucketFile.addRecord(tempBucket));
                indexFile.writeRecord(currentNode,nodePos);
            }
        }
    }
    void deleteRecordBucket(Key key, HashNode &currentNode, int nodePos){
        int curBucketPos = currentNode.getPosition();
        Bucket<Record, Key> bucket = bucketFile.readRecord(curBucketPos);
        vector<Record> differentRecords = bucket.getAllDifferentRecords(key);
        bucket.setRecords(differentRecords);
        bucketFile.writeRecord(bucket, curBucketPos);

        if(bucket.isEmpty() && bucket.getNextBucket() != -1){
            currentNode.setPosition(bucket.getNextBucket());
            bucketFile.deleteRecord(curBucketPos);
            indexFile.writeRecord(currentNode,nodePos);
            deleteRecordBucket(key, currentNode, nodePos);
        }
    }
};


#endif // HASH_H