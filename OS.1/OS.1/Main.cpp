#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>

using namespace std;

#pragma pack(push, 1)
struct employee {
    int num;    
    char name[10]; 
    double hours;   

};
#pragma pack(pop)


void printBinaryFile(const string& filename) {
    ifstream file(filename, ios::binary);
    if (!file) {
        cerr << "Error: Cannot open file " << filename << endl;
        return;
    }

    cout << "\nContents of binary file " << filename << ":" << endl;
    cout << "ID\tName\tHours" << endl;
    cout << "------------------------" << endl;

    employee emp;
    while (file.read(reinterpret_cast<char*>(&emp), sizeof(employee))) {
        cout << emp.num << "\t" << emp.name << "\t"  << emp.hours << endl;
    }
    file.close();
}

void printReportFile(const string& filename) {
    ifstream file(filename);
    if (!file) {
        cerr << "Error: Cannot open file " << filename << endl;
        return;
    }

    cout << "\nContents of report file " << filename << ":" << endl;
    cout << "==========================================" << endl;

    string line;
    while (getline(file, line)) {
        cout << line << endl;
    }
    file.close();
}

int main() {

    string binary_filename, report_filename;
    int record_count;
    double hourly_payment;

    cout << "=== Employee System Main Program ===" << endl;

    cout << "\nEnter binary filename: ";
    cin >> binary_filename;
    cout << "Enter number of records: ";
    cin >> record_count;

    string creator_command = "creator.exe " + binary_filename + " " + to_string(record_count);

    STARTUPINFOA siCreator;
    PROCESS_INFORMATION piCreator;
    ZeroMemory(&siCreator, sizeof(siCreator));
    siCreator.cb = sizeof(siCreator);
    ZeroMemory(&piCreator, sizeof(piCreator));

    if (CreateProcessA(NULL,
        (LPSTR)creator_command.c_str(),
        NULL, NULL, FALSE, 0, NULL, NULL, &siCreator, &piCreator)) {
        WaitForSingleObject(piCreator.hProcess, INFINITE);
        CloseHandle(piCreator.hProcess);
        CloseHandle(piCreator.hThread);
        cout << "Creator completed." << endl;
    }
    else {
        cerr << "Error starting Creator" << endl;
        return 1;
    }

    printBinaryFile(binary_filename);
    cout << "\nEnter report filename: ";
    cin >> report_filename;
    cout << "Enter hourly payment: ";
    cin >> hourly_payment;

    string reporter_command = "reporter.exe " + binary_filename + " " +
        report_filename + " " + to_string(hourly_payment);

    STARTUPINFOA siReporter;
    PROCESS_INFORMATION piReporter;
    ZeroMemory(&siReporter, sizeof(siReporter));
    siReporter.cb = sizeof(siReporter);
    ZeroMemory(&piReporter, sizeof(piReporter));

    if (CreateProcessA(NULL,
        (LPSTR)reporter_command.c_str(),
        NULL, NULL, FALSE, 0, NULL, NULL, &siReporter, &piReporter)) {
        WaitForSingleObject(piReporter.hProcess, INFINITE);
        CloseHandle(piReporter.hProcess);
        CloseHandle(piReporter.hThread);
        cout << "Reporter completed." << endl;
    }
    else {
        cerr << "Error starting Reporter" << endl;
        return 1;
    }

    printReportFile(report_filename);

    cout << "\nProgram completed. Press Enter to exit...";
    cin.ignore();
    cin.get();

    return 0;
}