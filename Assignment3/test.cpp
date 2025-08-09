#include <string>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <bitset>
#include <fstream>
using namespace std;

class Record {
public:
    int id, manager_id;
    std::string bio, name;

    Record() : id(0), manager_id(0), bio(""), name("") {}

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

class LinearHashIndex {

private:
const int PAGE_SIZE = 4096;
    const int MAX_RECORDS_PER_BLOCK = PAGE_SIZE / sizeof(Record);
    const int MAX_RECORDS_PER_PAGE = MAX_RECORDS_PER_BLOCK;
    vector<int> pageDirectory; // Where pageDirectory[h(id)] gives page index of block
    // can scan to pages using index*PAGE_SIZE as offset (using seek function)
    int numBlocks; // n
    int n;  // The number of indexes in blockDirectory currently being used
    int i;	// The number of least-significant-bits of h(id) to check. Will need to increase i once n > 2^i
    int numRecords;    // Records currently in index. Used to test whether to increase n
    int nextFreeBlock; // Next place to write a bucket. Should increment it by BLOCK_SIZE whenever a bucket is written to EmployeeIndex
    int numRecords; // Records in index
    int nextFreePage; // Next page to write to
    int block_size; // Number of records in a block
    int numPages;
    string fName; // Name of the index file// Insert new record into index
    static const int nameMaxSize = 100; // 假设的最大长度
    static const int bioMaxSize = 256;  // 假设的最大长度


    int hashFunction(int id) {
        return id % 216;
    };

    int getNextFreePage() {
    int currentPage = nextFreePage;
    nextFreePage += PAGE_SIZE; // 假设每个新页面都会占用 PAGE_SIZE 的空间
    return currentPage;
    }
    vector<Record> readBlock(int pageIndex){
            // 计算在文件中的偏移量
            int offset = pageIndex * PAGE_SIZE;
            
            // 打开文件
            ifstream indexFile(fName, ios::binary);
            if (!indexFile) {
                cerr << "Cannot open index file." << endl;
                return vector<Record>(); // 返回一个空向量
            }

            // 移动到正确的位置
            indexFile.seekg(offset);

            vector<Record> block;
            for (int i = 0; i < block_size; ++i) {
                Record record;
                // 从文件中读取一个 Record
                if (indexFile.read(reinterpret_cast<char*>(&record), sizeof(Record))) {
                    block.push_back(record);
                } else {
                    break; // 如果读取失败，则停止
                }
            }

            indexFile.close(); // 关闭文件
            return block; // 返回读取到的块
        }
    void insertRecord(Record record) {

        // No records written to index yet
        

         
        if (numRecords == 0) {

            // Initialize index with first blocks (start with 2)

            vector<Record> newBlock;

            newBlock.push_back(record);

            writeBlock(newBlock, 2);

            pageDirectory.push_back(2);

            numBlocks++;

            numRecords++;

        } else {

            int hash = hashFunction(record.id);

            int pageIndex = pageDirectory[hash];

            int blockIndex = (hash * block_size) % MAX_RECORDS_PER_BLOCK;

            vector<Record> block = readBlock(pageIndex);

            // Check if block is full

            if (block.size() == block_size) {

                // Create overflow block

                vector<Record> overflowBlock;

                overflowBlock.push_back(record);

                int overflowPageIndex = getNextFreePage();

                writeBlock(overflowBlock, overflowPageIndex);

                // Update page directory

                pageDirectory[hash] = overflowPageIndex;

                pageDirectory.push_back(overflowPageIndex);

                numBlocks++;

            } else {

                block.push_back(record);

                writeBlock(block, pageIndex);

            }

            numRecords++;

            // Check if average number of records per block exceeds 70% of the block capacity
            
            if ((double)numRecords / numPages > MAX_RECORDS_PER_PAGE * 0.7) {
            numPages++;

            }

        }

    }



// Write a block to the index file

void writeBlock(vector<Record> block, int pageIndex) {

    ofstream indexFile(fName, ios::binary | ios::in | ios::out);

    indexFile.seekp(pageIndex * PAGE_SIZE);

    for (Record record : block) {

        indexFile.write(reinterpret_cast<const char*>(&record), sizeof(record));

    }

    indexFile.close();

}


public:
    LinearHashIndex(string indexFileName) {
        n = 4; // Start with 4 buckets in index
        i = 2; // Need 2 bits to address 4 buckets
        numRecords = 0;
        nextFreeBlock = 0;
        fName = indexFileName;


        ofstream indexFile(fName, ios::binary | ios::out | ios::trunc); // 以二进制模式打开文件，并清空文件内容
        if (!indexFile) {
            cerr << "Cannot open index file for initialization." << endl;
            return;
        }

        // 初始化 4 个桶的数据
        for (int j = 0; j < n; ++j) {
            vector<Record> initialBlock; // 创建一个空的初始块
            // 假设 writeBlock 方法接受一个块和位置，并将其写入文件
            writeBlock(initialBlock, nextFreeBlock);
            nextFreeBlock += block_size; // 增加 nextFreeBlock 以指向下一个空闲块的位置
        }

        indexFile.close(); // 关闭文件
    }
    // Read csv file and add records to the index
    void createFromFile(string csvFName) {
        ifstream csvFile(csvFName);
        if (!csvFile) {
            cerr << "Cannot open CSV file." << endl;
            return;
        }

        string line;
        while (getline(csvFile, line)) {
            // Assuming the CSV format is known and consistent
            stringstream ss(line);
            string token;
            vector<string> tokens;
            while (getline(ss, token, ',')) {
                tokens.push_back(token);
            }

            cout << "Parsed tokens:" << endl;
            for (const auto& t : tokens) {
                cout << t << " ";
            }
            cout << endl;

            // 确保 tokens 向量中有足够的元素
            if (tokens.size() < 4) {
                cerr << "Error: not enough fields in line." << endl;
                continue; // 跳过这一行
            }
            // Create a Record object from the parsed line
            Record record(tokens);
            // Set other record fields as per your CSV structure

            // Insert the record into the index
            insertRecord(record);
        }

        csvFile.close();
    }

// Given an ID, find the relevant record and print it
    Record findRecordById(int id) {
        ifstream indexFile(fName, ios::binary);

        if (!indexFile.is_open()) {
            cerr << "Error: could not open " << fName << endl;
            return Record{};
        }

        int pageIndex = pageDirectory[hashFunction(id)];
        int offset = 0;
        int recordSize = sizeof(int) + nameMaxSize + bioMaxSize + sizeof(int);

        while (pageIndex != -1) {
            indexFile.seekg(pageIndex * PAGE_SIZE + offset);

            for (int i = 0; i < block_size; i++) {
                int recordId;
                indexFile.read(reinterpret_cast<char*>(&recordId), sizeof(int));

                if (recordId == id) {
                    char name[nameMaxSize];
                    indexFile.read(name, nameMaxSize);

                    char bio[bioMaxSize];
                    indexFile.read(bio, bioMaxSize);

                    int managerId;
                    indexFile.read(reinterpret_cast<char*>(&managerId), sizeof(int));

                    indexFile.close();
                    return Record({to_string(recordId), name, bio, to_string(managerId)});
                }
            }

            indexFile.read(reinterpret_cast<char*>(&offset), sizeof(int));
            pageIndex = (offset == 0 ? -1 : pageIndex + 1);
            offset = 0;
        }

        indexFile.close();
        cerr << "Record with ID " << id << " not found" << endl;
        return Record{};
    }
};