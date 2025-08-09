/* This is a skeleton code for Optimized Merge Sort, you can make modifications as long as you meet 
   all question requirements*/  

#include <string>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <bitset>
#include <queue>
#include <iomanip>
#include "record_class.h"

using namespace std;

//defines how many blocks are available in the Main Memory 
#define buffer_size 22

Records buffers[buffer_size]; //use this class object of size 22 as your main memory
struct JoinedRecord {
    int eid;
    string ename;
    int age;
    double salary;
    int did;
    string dname;
    double budget;
    int managerid;
};

JoinedRecord ParseJoinedRecord(const string& line) {
    JoinedRecord joinedRecord;
    stringstream s(line);
    string word;

    getline(s, word, ',');
    joinedRecord.eid = stoi(word);
    getline(s, word, ',');
    joinedRecord.ename = word;
    getline(s, word, ',');
    joinedRecord.age = stoi(word);
    getline(s, word, ',');
    joinedRecord.salary = stod(word);
    getline(s, word, ',');
    joinedRecord.did = stoi(word);
    getline(s, word, ',');
    joinedRecord.dname = word;
    getline(s, word, ',');
    joinedRecord.budget = stod(word);
    getline(s, word, ',');
    joinedRecord.managerid = stoi(word);

    return joinedRecord;
}
/***You can change return type and arguments as you want.***/

//Sorting the buffers in main_memory and storing the sorted records into a temporary file (runs) 
void Merge(Records* buffer, int left, int mid, int right, bool isEmp) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    // Create temp arrays
    Records* L = new Records[n1];
    Records* R = new Records[n2];

    // Copy data to temp arrays
    for (int i = 0; i < n1; i++)
        L[i] = buffer[left + i];
    for (int j = 0; j < n2; j++)
        R[j] = buffer[mid + 1 + j];

    // Merge the temp arrays back into buffer
    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (isEmp) {
            if (L[i].emp_record.eid <= R[j].emp_record.eid) {
                buffer[k] = L[i];
                i++;
            } else {
                buffer[k] = R[j];
                j++;
            }
        } else {
            if (L[i].dept_record.managerid <= R[j].dept_record.managerid) {
                buffer[k] = L[i];
                i++;
            } else {
                buffer[k] = R[j];
                j++;
            }
        }
        k++;
    }

    // Copy remaining elements
    while (i < n1) {
        buffer[k] = L[i];
        i++;
        k++;
    }
    while (j < n2) {
        buffer[k] = R[j];
        j++;
        k++;
    }

    delete[] L;
    delete[] R;
}

void MergeSort(Records* buffer, int left, int right, bool isEmp) {
    if (left < right) {
        int mid = left + (right - left) / 2;

        MergeSort(buffer, left, mid, isEmp);
        MergeSort(buffer, mid + 1, right, isEmp);

        Merge(buffer, left, mid, right, isEmp);
    }
}
void Sort_Buffer(fstream& inputFile, int runNumber, bool isEmp) {
    Records buffer[buffer_size]; 
    int count = 0;

    
    while (count < buffer_size && !inputFile.eof()) {
        if (isEmp) {
            buffer[count] = Grab_Emp_Record(inputFile);
        } else {
            buffer[count] = Grab_Dept_Record(inputFile);
        }

        if (buffer[count].no_values == -1) break;
        count++;
    }

   
    MergeSort(buffer, 0, count - 1, isEmp);

    
    ofstream out("run" + to_string(runNumber) + (isEmp ? "_emp.csv" : "_dept.csv"));
    out << fixed << setprecision(2);
    for (int i = 0; i < count; i++) {
        if (isEmp) {
            out << buffer[i].emp_record.eid << "," << buffer[i].emp_record.ename << ","
                << buffer[i].emp_record.age << "," << buffer[i].emp_record.salary << endl;
        } else {
            out << buffer[i].dept_record.did << "," << buffer[i].dept_record.dname << ","
                << buffer[i].dept_record.budget << "," << buffer[i].dept_record.managerid << endl;
        }
    }
    out.close();
}

//Prints out the attributes from empRecord and deptRecord when a join condition is met 
//and puts it in file Join.csv
/*void PrintJoin(const std::string& intermediateFileName, const std::string& finalOutputFileName) {
    std::ifstream intermediateFile(intermediateFileName);
    std::ofstream finalOutputFile(finalOutputFileName);

    finalOutputFile << std::fixed << std::setprecision(2);
    
    std::string line;
    while (std::getline(intermediateFile, line)) {
        finalOutputFile << line << std::endl;
    }

    intermediateFile.close();
    finalOutputFile.close();
}*/



//Use main memory to Merge and Join tuples 
//which are already sorted in 'runs' of the relations Dept and Emp 
void MergeJoinRuns(const std::string& outputFileName, int RunNumber) {
    std::ofstream tempOutputFile(outputFileName);

    for (int i = 0; i <= RunNumber; ++i) {
        std::string empFileName = "run" + std::to_string(i) + "_emp.csv";
        fstream empFile(empFileName);
        if (!empFile) {
            //std::cerr << "Emp run file " << empFileName << " not found." << std::endl;
            continue;
        }

        for (int j = 0; j <= RunNumber; ++j) {
            std::string deptFileName = "run" + std::to_string(j) + "_dept.csv";
            fstream deptFile(deptFileName);
            if (!deptFile) {
                //std::cerr << "Dept run file " << deptFileName << " not found." << std::endl;
                continue;
            }

            Records empRecord, deptRecord;
            if (!empFile.eof()) empRecord = Grab_Emp_Record(empFile);
            if (!deptFile.eof()) deptRecord = Grab_Dept_Record(deptFile);

            while (empFile.good() && deptFile.good()) {
                if (empRecord.no_values == -1 || deptRecord.no_values == -1) {
                    break;
                }

                if (deptRecord.dept_record.managerid == empRecord.emp_record.eid) {
                    tempOutputFile << empRecord.emp_record.eid << "," << empRecord.emp_record.ename << ","
                                   << empRecord.emp_record.age << "," << empRecord.emp_record.salary << ","
                                   << deptRecord.dept_record.did << "," << deptRecord.dept_record.dname << ","
                                   << deptRecord.dept_record.budget << "," << deptRecord.dept_record.managerid << std::endl;

                    deptRecord = Grab_Dept_Record(deptFile);
                } else if (deptRecord.dept_record.managerid < empRecord.emp_record.eid) {
                    deptRecord = Grab_Dept_Record(deptFile);
                } else {
                    empRecord = Grab_Emp_Record(empFile);
                }
            }
            deptFile.close();
        }
        empFile.close();
    }

    tempOutputFile.close();
}

void SortJoinedFile(const std::string& inputFile, const std::string& outputFile) {
    std::ifstream input(inputFile);
    std::ofstream output(outputFile);
    std::vector<JoinedRecord> records;
    std::string line;

    
    while (std::getline(input, line)) {
        records.push_back(ParseJoinedRecord(line));
    }

    
    std::sort(records.begin(), records.end(), [](const JoinedRecord& a, const JoinedRecord& b) {
        return a.eid < b.eid;  
    });

   
   for (const auto& record : records) {
        output << record.eid << "," 
               << record.ename << "," 
               << record.age << "," 
               << std::fixed << std::setprecision(2) << record.salary << "," 
               << record.did << "," 
               << record.dname << "," 
               << std::fixed << std::setprecision(2) << record.budget << "," 
               << record.managerid << std::endl;
    }
    
    input.close();
    output.close();
}

int main() {
    
  
    fstream empin("Emp.csv", std::ios::in);
    fstream deptin("Dept.csv", std::ios::in);
    fstream joinout;
    joinout.open("Join.csv", ios::out | ios::app);

    int runNumber = 0;
    while (!empin.eof()) {
        Sort_Buffer(empin, runNumber, true);
        Sort_Buffer(deptin, runNumber, false);
        runNumber++;
    }
    
    empin.close();
    deptin.close();

    
    MergeJoinRuns("JoinedResult.csv", runNumber  );

   
    SortJoinedFile("JoinedResult.csv", "Join.csv");
    

    std::remove("JoinedResult.csv");


    //Please delete the temporary files (runs) after you've joined both Emp.csv and Dept.csv
    for (int i = 0; i < runNumber; ++i) {
        std::remove(("run" + std::to_string(i) + "_emp.csv").c_str());
        std::remove(("run" + std::to_string(i) + "_dept.csv").c_str());
    }
    
    return 0;
}

