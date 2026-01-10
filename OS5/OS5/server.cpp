#include <windows.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "employee.h"

using namespace std;

#define PIPE_NAME L"\\\\.\\pipe\\employee_pipe"
#define FILENAME "employees.dat"
#define _CRT_SECURE_NO_WARNINGS


HANDLE hPipe;
vector<employee> employees;
CRITICAL_SECTION csFile; 

void createFile() {
    int n;
    cout << "Введите количество сотрудников: ";
    cin >> n;

    employees.resize(n);

    for (int i = 0; i < n; i++) {
        cout << "\nСотрудник " << i + 1 << ":\n";
        cout << "  ID: ";
        cin >> employees[i].num;
        cout << "  Имя (до 9 символов): ";
        cin >> employees[i].name;
        cout << "  Отработано часов: ";
        cin >> employees[i].hours;
    }

    ofstream file(FILENAME, ios::binary);
    if (!file) {
        cerr << "Ошибка создания файла!\n";
        return;
    }

    file.write((char*)employees.data(), n * sizeof(employee));
    file.close();

    cout << "\nФайл создан успешно!\n";
}


void loadFile() {
    ifstream file(FILENAME, ios::binary | ios::ate);
    if (!file) {
        cerr << "Файл не найден!\n";
        return;
    }

    streamsize size = file.tellg();
    file.seekg(0, ios::beg);

    employees.resize((size_t)(size / sizeof(employee))); 
    file.read((char*)employees.data(), size);
    file.close();
}


void printFile() {
    cout << "\n=== СОДЕРЖИМОЕ ФАЙЛА ===\n";
    cout << "ID\tИмя\t\tЧасы\n";
    cout << "---------------------------\n";

    for (const auto& emp : employees) {
        cout << emp.num << "\t" << emp.name << "\t\t" << emp.hours << endl;
    }
    cout << "===========================\n";
}


void saveFile() {
    EnterCriticalSection(&csFile);

    ofstream file(FILENAME, ios::binary);
    if (!file) {
        cerr << "Ошибка сохранения файла!\n";
        LeaveCriticalSection(&csFile);
        return;
    }

    file.write((char*)employees.data(), employees.size() * sizeof(employee));
    file.close();

    LeaveCriticalSection(&csFile);
}


int findEmployee(int id) {
    for (int i = 0; i < (int)employees.size(); i++) {
        if (employees[i].num == id) {
            return i;
        }
    }
    return -1;
}


void safe_strcpy(char* dest, const char* src, size_t dest_size) {
    if (dest_size > 0) {
        strncpy_s(dest, dest_size, src, _TRUNCATE);
        dest[dest_size - 1] = '\0'; 
    }
}


DWORD WINAPI handleClient(LPVOID lpParam) {
    Request req;
    Response resp;
    DWORD bytesRead, bytesWritten;


    if (!ReadFile(hPipe, &req, sizeof(Request), &bytesRead, NULL)) {
        cerr << "Ошибка чтения запроса: " << GetLastError() << endl;
        return 1;
    }


    switch (req.op) {
    case OP_READ: {
        int idx = findEmployee(req.employee_id);
        if (idx != -1) {
            resp.success = 1;
            resp.emp = employees[idx];
            safe_strcpy(resp.error_msg, "", sizeof(resp.error_msg));
        }
        else {
            resp.success = 0;
            safe_strcpy(resp.error_msg, "Сотрудник не найден", sizeof(resp.error_msg));
        }
        break;
    }

    case OP_MODIFY: {
        EnterCriticalSection(&csFile); 

        int idx = findEmployee(req.employee_id);
        if (idx != -1) {

            employee updatedEmp;
            if (!ReadFile(hPipe, &updatedEmp, sizeof(employee), &bytesRead, NULL)) {
                cerr << "Ошибка чтения данных: " << GetLastError() << endl;
                LeaveCriticalSection(&csFile);
                return 1;
            }


            employees[idx] = updatedEmp;
            saveFile();

            resp.success = 1;
            resp.emp = employees[idx];
            safe_strcpy(resp.error_msg, "", sizeof(resp.error_msg));
        }
        else {
            resp.success = 0;
            safe_strcpy(resp.error_msg, "Сотрудник не найден", sizeof(resp.error_msg));
        }

        LeaveCriticalSection(&csFile);
        break;
    }

    case OP_EXIT:
        resp.success = 1;
        safe_strcpy(resp.error_msg, "Выход", sizeof(resp.error_msg));
        break;

    default:
        resp.success = 0;
        safe_strcpy(resp.error_msg, "Неизвестная операция", sizeof(resp.error_msg));
    }

    if (!WriteFile(hPipe, &resp, sizeof(Response), &bytesWritten, NULL)) {
        cerr << "Ошибка отправки ответа: " << GetLastError() << endl;
        return 1;
    }

    return 0;
}

int main() {
    setlocale(LC_ALL, "Russian");

    InitializeCriticalSection(&csFile);

    createFile();
    printFile();

    hPipe = CreateNamedPipe(
        PIPE_NAME,
        PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_MESSAGE | PIPE_WAIT,
        1,                   
        1024, 1024,          
        0,                   
        NULL          
    );

    if (hPipe == INVALID_HANDLE_VALUE) {
        cerr << "Ошибка создания канала: " << GetLastError() << endl;
        DeleteCriticalSection(&csFile);
        return 1;
    }

    cout << "\nСервер запущен. Ожидание подключения клиента...\n";

    if (!ConnectNamedPipe(hPipe, NULL)) {
        DWORD err = GetLastError();
        if (err != ERROR_PIPE_CONNECTED) {
            cerr << "Ошибка подключения: " << err << endl;
            CloseHandle(hPipe);
            DeleteCriticalSection(&csFile);
            return 1;
        }
    }

    cout << "Клиент подключен!\n";

    bool running = true;
    while (running) {
        handleClient(NULL);
        running = false;
    }

    DisconnectNamedPipe(hPipe);
    CloseHandle(hPipe);

    cout << "\n=== ОБНОВЛЕННЫЙ ФАЙЛ ===\n";
    loadFile();
    printFile();

    cout << "\nНажмите Enter для завершения сервера...";
    cin.ignore();
    cin.get();

    DeleteCriticalSection(&csFile);

    return 0;
}