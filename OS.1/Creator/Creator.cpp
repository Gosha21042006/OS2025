#include <iostream>
#include <fstream>
#include <string>

using namespace std;

#pragma pack(push, 1)
struct employee {
    int num;       
    char name[10];  
    double hours;   
};
#pragma pack(pop)


int main(int argc, char* argv[]) {

    if (argc != 3) {
        cerr << "Usage: creator <filename> <record_count>" << endl;
        return 1;
    }

    string filename = argv[1];
    int record_count = stoi(argv[2]);

    ofstream file(filename, ios::binary);
    if (!file) {
        cerr << "Error: Cannot create file " << filename << endl;
        return 1;
    }

    cout << "Enter " << record_count << " employee records:" << endl;

    for (int i = 0; i < record_count; ++i) {
        employee emp;

        cout << "Record " << (i + 1) << ":" << endl;

        cout << "  ID: ";
        cin >> emp.num;

        cout << "  Name (max 9 characters): ";
        cin.ignore();
        cin.getline(emp.name, 10);

        cout << "  Hours: ";
        cin >> emp.hours;

        file.write(reinterpret_cast<char*>(&emp), sizeof(employee));
    }

    file.close();
    cout << "Binary file " << filename << " created successfully with " << record_count << " records." << endl;

    return 0;
}