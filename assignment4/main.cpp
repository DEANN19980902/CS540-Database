/* This is a skeleton code for External Memory Sorting, you can make modifications as long as you meet 
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

/***You can change return type and arguments as you want.***/

//PASS 1
//Sorting the buffers in main_memory and storing the sorted records into a temporary file (Runs) 

void Merge(Records* buffer, int left, int mid, int right) {
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
        if (L[i].emp_record.eid <= R[j].emp_record.eid) {
            buffer[k] = L[i];
            i++;
        } else {
            buffer[k] = R[j];
            j++;
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

void MergeSort(Records* buffer, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;

        // Sort first and second halves
        MergeSort(buffer, left, mid);
        MergeSort(buffer, mid + 1, right);

        Merge(buffer, left, mid, right);
    }
}

void Sort_Buffer(fstream& empinside, int runNumber) {
    Records buffer[buffer_size];
    int count = 0;

    // Read records into buffer
    while (count < buffer_size && !empinside.eof()) {
        buffer[count] = Grab_Emp_Record(empinside);
        if (buffer[count].no_values == -1) break; // Check if no more records
        count++;
    }

    // Sort buffer based on eid
    MergeSort(buffer, 0, count - 1);

    // Write sorted buffer to a temporary file (run)
    ofstream out("run" + to_string(runNumber) + ".csv");
    out << fixed << setprecision(2);
    for (int i = 0; i < count; i++) {
        out << buffer[i].emp_record.eid << "," << buffer[i].emp_record.ename << ","
            << buffer[i].emp_record.age << "," << buffer[i].emp_record.salary << endl;
    }
    out.close();
}
//PASS 2
//Use main memory to Merge the Runs 
//which are already sorted in 'runs' of the relation Emp.csv 
void PrintSorted(const Records& record, ofstream& outputFile) {
    outputFile << fixed << setprecision(2); 
    // Write the single record to the outputFile
    outputFile << record.emp_record.eid << "," 
               << record.emp_record.ename << ","
               << record.emp_record.age << ","
               << record.emp_record.salary << endl;
}


void Merge_Runs(int numRuns) {
    // Define a priority queue with a lambda function for comparison
    auto compare = [](const pair<Records, int>& a, const pair<Records, int>& b) {
        return a.first.emp_record.eid > b.first.emp_record.eid;
    };
    priority_queue<pair<Records, int>, vector<pair<Records, int>>, decltype(compare)> pq(compare);

    vector<fstream> runFiles(numRuns);
    string line, word;

    // Open each run file and push the first record to the priority queue
    for (int i = 0; i < numRuns; i++) {
        runFiles[i].open("run" + to_string(i) + ".csv", ios::in);
        if (getline(runFiles[i], line)) {
            stringstream s(line);
            Records rec;
            getline(s, word, ',');
            rec.emp_record.eid = stoi(word);
            getline(s, word, ',');
            rec.emp_record.ename = word;
            getline(s, word, ',');
            rec.emp_record.age = stoi(word);
            getline(s, word, ',');
            rec.emp_record.salary = stod(word);
            pq.push(make_pair(rec, i));
        }
    }

    ofstream outputFile("EmpSorted.csv");
    while (!pq.empty()) {
        pair<Records, int> top = pq.top();
        pq.pop();
        PrintSorted(top.first, outputFile);

        // Fetch the next record from the same run file
        if (getline(runFiles[top.second], line)) {
            stringstream s(line);
            Records rec;
            getline(s, word, ',');
            rec.emp_record.eid = stoi(word);
            getline(s, word, ',');
            rec.emp_record.ename = word;
            getline(s, word, ',');
            rec.emp_record.age = stoi(word);
            getline(s, word, ',');
            rec.emp_record.salary = stod(word);
            pq.push(make_pair(rec, top.second));
        }
    }

    for (auto& runFile : runFiles) {
        runFile.close();
    }
    outputFile.close();
}


int main() {

    //Open file streams to read and write
    //Opening out the Emp.csv relation that we want to Sort
    fstream empinside;
    empinside.open("Emp.csv", ios::in);
   
    //Creating the EmpSorted.csv file where we will store our sorted results
    fstream SortOut;
    SortOut.open("EmpSorted.csv", ios::out | ios::app);

    //1. Create runs for Emp which are sorted using Sort_Buffer()
    int runNumber = 0;
    while (!empinside.eof()) {
        Sort_Buffer(empinside, runNumber);
        runNumber++;
    }
    empinside.close();

    //2. Use Merge_Runs() to Sort the runs of Emp relations 
    Merge_Runs(runNumber);

    //Please delete the temporary files (runs) after you've sorted the Emp.csv
    for (int i = 0; i < runNumber; i++) {
        remove(("run" + to_string(i) + ".csv").c_str());
    }
    return 0;
}
