#include <string>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <bitset>
#include <stack>

using namespace std;

class Record {
public:
    int id, manager_id;
    std::string bio, name;
Record() : id(0), manager_id(0), bio(""), name(""){}

    Record(vector<std::string> fields) {
        id = stoi(fields[0]);
        name = fields[1];
        bio = fields[2];
        manager_id = stoi(fields[3]);
    }

    void print() {
        cout << "\tID: " << id << "\n";
        cout << "\tNAME: " << name << "\n";
        cout << "\tBIO: " << bio << "\n";
        cout << "\tMANAGER_ID: " << manager_id << "\n";
    }
};


class StorageBufferManager {

private:
    
    const int BLOCK_SIZE = 3; // initialize the  block size allowed in main memory according to the question 

    // You may declare variables based on your need 
    const int PAGE_SIZE = 4096;
    int numRecords = 0;
    int currentRecordPos = 0;
    int currPage = 1;
    int SlotDsize = 0;
    string gNewFileName;
    vector<char> Data;
    stack<string> SlotD;

    void StoreRecord() {
        string sizeofData = to_string(Data.size());
        string sizeofentries = to_string(numRecords);

        fstream fin;
        fin.open(gNewFileName,ios::binary| ios::out| ios::in);
        if(fin.is_open()) { 
            fin.seekp(PAGE_SIZE*(currPage - 1));
            for (int i = 0; i < Data.size(); i++) {
                fin << Data[i];
            }
            fin.seekp(PAGE_SIZE*currPage - SlotDsize - sizeofentries.length() - sizeofData.length() - 3);
            //stop string
            fin << "$";
            while(!SlotD.empty()) {
                string kpop = SlotD.top();
                fin << kpop;
                SlotD.pop();
            }
            fin << sizeofentries;
            fin << ",";
            fin << sizeofData;
            fin << "\n";
        }
        fin.close();
        return;
    }


    // Insert new record 
    void insertRecord(Record record) {
        // record.print();
        vector<char> tempData;
        int tempsize;
        string t;
        t = to_string(record.id);
        // Add record into temp vec
        char const* id_char = t.c_str();
        for (int i = 0; i < t.length();i++) {
            tempData.push_back(id_char[i]);
        }
        tempData.push_back(',');
        char const* name_char = record.name.c_str();
        for (int i = 0; i < record.name.length();i++) {
            tempData.push_back(name_char[i]);
        }
        tempData.push_back(',');
        char const* bio_char = record.bio.c_str();
        for (int i = 0; i < record.bio.length();i++) {
            tempData.push_back(bio_char[i]);
        }
        t = to_string(record.manager_id);
        tempData.push_back(',');
        char const* manid_char = t.c_str(); 
        for (int i = 0; i < t.length();i++) {
            tempData.push_back(manid_char[i]);
        }
        tempData.push_back(',');

        // Catch record size
        tempsize = tempData.size();
        string index = to_string(currentRecordPos) + "," + to_string(tempsize) + ",";
        // Add record to the block

        // Catch SlotDirectory in temp vec
        
        string sizeofData = to_string(Data.size());
        string sizeofentries = to_string(numRecords);
        
// This basic idea is to design a page that doesn't exist 4096 KB
        // Our page will be like (Records + "$" sign + SlotDirectory + Number of entries + "," + Pointer to free space + "\n")
        // If the size of new record plus the size of the exists records doesn't exceed the size of a page, we add this record
        if ((tempData.size() + index.length()) + (Data.size() + SlotDsize) + 1 + sizeofentries.length() + 1 + sizeofData.length() + 1 <= PAGE_SIZE) {
            currentRecordPos += tempsize;
            numRecords++;
            for (int i = 0; i < tempsize; i++) {
                Data.push_back(tempData[i]);
            }
            SlotD.push(index);
            SlotDsize += index.length();
            return;
        }
        
        // Take neccessary steps if capacity is reached (you've utilized all the blocks in main memory)
        // Because the page fault, we write our page into the file and flush the page for new records
        else {
            //First we merge the record and slotdirectory
            
            fstream fin;
            fin.open(gNewFileName,ios::binary| ios::out| ios::in);
            if(fin.is_open()) { 
                fin.seekp(PAGE_SIZE*(currPage - 1));
                for (int i = 0; i < Data.size(); i++) {
                    fin << Data[i];
                }
                fin.seekp(PAGE_SIZE*currPage - SlotDsize - sizeofentries.length() - sizeofData.length() - 3);
                //stop string
                fin << "$";
                while(!SlotD.empty()) {
                    string kpop = SlotD.top();
                    fin << kpop;
                    SlotD.pop();
                }
                fin << sizeofentries;
                fin << ",";
                fin << sizeofData;
                fin << "\n";
            }
            fin.close();
            currPage++;

            //clear after write into the file
            SlotDsize = 0;
            numRecords = 0;
            currentRecordPos = 0;
            Data.clear();
            SlotD = {};

        }
        // Then write after clear, because it's first record, we don't think, we push
        numRecords++;
        index = to_string(currentRecordPos) + "$" + to_string(tempsize) + "$";
        currentRecordPos += tempsize;
        for (int i = 0; i < tempsize; i++) {
            Data.push_back(tempData[i]);
        }
        SlotD.push(index);
        SlotDsize += index.length();

    }

public:
    StorageBufferManager(string NewFileName) {
        
        //initialize your variables
        ofstream fin;

        // Create your EmployeeRelation file 
        fin.open(NewFileName,ios::binary| ios::out);
        if(!fin) { 
            cerr<<"Error in creating file!!!\n"; 
            return ; 
        } 
        fin.close();
        gNewFileName = NewFileName;

        return;
        
    }

    // Read csv file (Employee.csv) and add records to the (EmployeeRelation)
    void createFromFile(string csvFName) {
        fstream fin;
        fin.open(csvFName,ios::in);
        if (!fin.is_open()) {
            cerr << "Couldn't read file: " << csvFName << "\n";
            return ;
        }
        
        string lineinfile;
        bool Flag = true;
        while (getline(fin,lineinfile) || Flag) {
            if (lineinfile.empty()) {
                // End of the file, just record it to target file
                Flag = false;
                StoreRecord();
                break;
            }
            string sttrinfile;
            vector<string> attrvec;
            stringstream stringline(lineinfile);
            while(getline(stringline,sttrinfile,',')) {
                attrvec.push_back(sttrinfile);
            }
            Record somerecord(attrvec);
            insertRecord(somerecord);
            // break;

        }
        fin.close();

        

    }

    // Given an ID, find the relevant record and print it
    Record findRecordById(int id) {
        ifstream fin(gNewFileName, ios::in);
        if (!fin.is_open()) {
            cerr << "Error: could not open file " << gNewFileName << "\n";
            return Record();
        }

        string line;
        bool found = false;
        Record record;

        while (getline(fin, line)) {
            vector<string> fields; // 在 while 循环内部声明 fields 向量
            fields.clear();

            if (line.find('$') != string::npos) {
                line = line.substr(0, line.find('$')); 
            }

            stringstream ss(line);
            string field;
            while (getline(ss, field, ',')) {
                if (field == "$") {
                    break;
                }
                fields.push_back(field);
            }
            //cout << "Size of fields vector: " << fields.size() << endl;
        
            /*cout << "Values in fields[]: ";
            for (const auto& value : fields) {
                cout << value << " ";
            }
            cout << endl;*/
            for (size_t i = 0; i < fields.size(); ++i) {
                try {
                    if (stoi(fields[i]) == id ) { // Exclude manager_id field
                        if (i % 4 == 0) { // 假设 id 总是每4个字段中的第一个
                            found = true;
                            vector<string> recordFields(fields.begin() + i, fields.begin() + i + 4); // 获取整个记录

                            record = Record(recordFields);
                            break; // 找到记录，退出循环
                        }
                    }
                } catch (const std::exception& e) {
                    // Ignore non-integer fields
                }
            }

            if (found) {
                break; // Exit the while loop if record is found
            }
        }

        fin.close();

        if (found) {
            
            return record;
        } else {
            cerr << "Record with ID " << id << " not found\n";
            return Record();
        }
        
    }

};