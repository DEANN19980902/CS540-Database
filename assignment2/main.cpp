/*
Skeleton code for storage and buffer management
*/

#include <string>
#include <ios>
#include <fstream>
#include <vector>
#include <string>
#include <string.h>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <cmath>
#include "classes.h"
using namespace std;


int main(int argc, char* const argv[]) {

    // Create the EmployeeRelation file from Employee.csv
    StorageBufferManager manager("EmployeeRelation");
    manager.createFromFile("Employee.csv");
    
    // Loop to lookup IDs until user is ready to quit
    cout << "Employee Record System" << endl;
    cout << "Enter an ID to search for an employee record, or -1 to quit:" << endl;

    int id;
    while (true) {
        cout << "ID: ";
        cin >> id;
        if (id == -1) {
            break; // Exit loop if user enters -1
        }

        try {
            Record foundRecord = manager.findRecordById(id);
            foundRecord.print(); // Display found record
        } catch (const std::exception& e) {
            cout << "Record not found or error: " << e.what() << endl;
        }
    }


    return 0;
}
