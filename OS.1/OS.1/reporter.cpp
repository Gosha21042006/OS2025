#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>

using namespace std;

#pragma pack(push, 1)
struct employee {
    int num;        // идентификационный номер сотрудника
    char name[10];  // имя сотрудника
    double hours;   // количество отработанных часов
};
#pragma pack(pop)

int main(int argc, char* argv[]) {
    if (argc != 4) {
        cerr << "Usage: reporter <input_file> <output_file> <hourly_payment>" << endl;
        return 1;
    }

    string input_filename = argv[1];
    string output_filename = argv[2];
    double hourly_payment = stod(argv[3]);

    ifstream input_file(input_filename, ios::binary);
    if (!input_file) {
        cerr << "Error: Cannot open input file " << input_filename << endl;
        return 1;
    }

    ofstream output_file(output_filename);
    if (!output_file) {
        cerr << "Error: Cannot create output file " << output_filename << endl;
        return 1;
    }

    // Заголовок отчета
    output_file << "Отчет по файлу \"" << input_filename << "\"" << endl;
    output_file << "Номер сотрудника\tИмя сотрудника\tЧасы\tЗарплата" << endl;

    employee emp;
    while (input_file.read(reinterpret_cast<char*>(&emp), sizeof(employee))) {
        double salary = emp.hours * hourly_payment;
        output_file << emp.num << "\t\t\t"
            << emp.name << "\t\t"
            << fixed << setprecision(2) << emp.hours << "\t"
            << fixed << setprecision(2) << salary << endl;
    }

    input_file.close();
    output_file.close();

    cout << "Report file " << output_filename << " created successfully." << endl;

    return 0;
}