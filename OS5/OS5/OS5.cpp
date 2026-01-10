#include <windows.h>
#include <iostream>
#include <string>
#include "employee.h"

using namespace std;

#define PIPE_NAME L"\\\\.\\pipe\\employee_pipe"
#define _CRT_SECURE_NO_WARNINGS

HANDLE hPipe;


bool connectToServer() {

    for (int i = 0; i < 5; i++) {
        hPipe = CreateFile(
            PIPE_NAME,
            GENERIC_READ | GENERIC_WRITE,
            0,
            NULL,
            OPEN_EXISTING,
            0,
            NULL
        );

        if (hPipe != INVALID_HANDLE_VALUE) {
            return true;
        }

        if (GetLastError() != ERROR_PIPE_BUSY) {
            cerr << "Не удалось подключиться к серверу. Код ошибки: " << GetLastError() << endl;
            return false;
        }


        if (!WaitNamedPipe(PIPE_NAME, 5000)) {
            cerr << "Таймаут ожидания канала" << endl;
            return false;
        }
    }

    return false;
}


bool sendRequest(Request& req, Response& resp) {
    DWORD bytesWritten, bytesRead;


    if (!WriteFile(hPipe, &req, sizeof(Request), &bytesWritten, NULL)) {
        cerr << "Ошибка отправки запроса: " << GetLastError() << endl;
        return false;
    }


    if (!ReadFile(hPipe, &resp, sizeof(Response), &bytesRead, NULL)) {
        cerr << "Ошибка получения ответа: " << GetLastError() << endl;
        return false;
    }

    return true;
}


void readRecord() {
    int id;
    cout << "\nВведите ID сотрудника для чтения: ";
    cin >> id;

    Request req = { OP_READ, id };
    Response resp;

    if (sendRequest(req, resp)) {
        if (resp.success) {
            cout << "\n=== ДАННЫЕ СОТРУДНИКА ===\n";
            cout << "ID: " << resp.emp.num << endl;
            cout << "Имя: " << resp.emp.name << endl;
            cout << "Часы: " << resp.emp.hours << endl;
            cout << "=========================\n";
        }
        else {
            cout << "Ошибка: " << resp.error_msg << endl;
        }
    }

    cout << "\nНажмите Enter для продолжения...";
    cin.ignore();
    cin.get();
}


void modifyRecord() {
    int id;
    cout << "\nВведите ID сотрудника для изменения: ";
    cin >> id;


    Request req = { OP_READ, id };
    Response resp;

    if (!sendRequest(req, resp)) {
        return;
    }

    if (!resp.success) {
        cout << "Ошибка: " << resp.error_msg << endl;
        cout << "\nНажмите Enter для продолжения...";
        cin.ignore();
        cin.get();
        return;
    }


    cout << "\n=== ТЕКУЩИЕ ДАННЫЕ ===\n";
    cout << "ID: " << resp.emp.num << endl;
    cout << "Имя: " << resp.emp.name << endl;
    cout << "Часы: " << resp.emp.hours << endl;
    cout << "======================\n";

    employee updatedEmp = resp.emp;

    cout << "\nВведите новые данные:\n";
    cout << "Новое имя (до 9 символов): ";
    cin >> updatedEmp.name;
    cout << "Новое количество часов: ";
    cin >> updatedEmp.hours;


    req.op = OP_MODIFY;

    DWORD bytesWritten, bytesRead;


    if (!WriteFile(hPipe, &req, sizeof(Request), &bytesWritten, NULL)) {
        cerr << "Ошибка отправки запроса: " << GetLastError() << endl;
        return;
    }


    if (!WriteFile(hPipe, &updatedEmp, sizeof(employee), &bytesWritten, NULL)) {
        cerr << "Ошибка отправки данных: " << GetLastError() << endl;
        return;
    }


    if (!ReadFile(hPipe, &resp, sizeof(Response), &bytesRead, NULL)) {
        cerr << "Ошибка получения ответа: " << GetLastError() << endl;
        return;
    }

    if (resp.success) {
        cout << "\nЗапись успешно обновлена!\n";
        cout << "Обновленные данные:\n";
        cout << "ID: " << resp.emp.num << endl;
        cout << "Имя: " << resp.emp.name << endl;
        cout << "Часы: " << resp.emp.hours << endl;
    }
    else {
        cout << "Ошибка: " << resp.error_msg << endl;
    }

    cout << "\nНажмите Enter для продолжения...";
    cin.ignore();
    cin.get();
}

int main() {
    setlocale(LC_ALL, "Russian");


    cout << "Подключение к серверу...\n";
    if (!connectToServer()) {
        cout << "Не удалось подключиться к серверу.\n";
        cout << "Убедитесь, что сервер запущен.\n";
        cout << "Нажмите любую клавишу для выхода...";
        cin.get();
        return 1;
    }

    cout << "Подключение к серверу установлено!\n";


    DWORD mode = PIPE_READMODE_MESSAGE;
    if (!SetNamedPipeHandleState(hPipe, &mode, NULL, NULL)) {
        cerr << "Ошибка установки режима: " << GetLastError() << endl;
        CloseHandle(hPipe);
        return 1;
    }


    bool running = true;
    while (running) {
        system("cls");
        cout << "=== КЛИЕНТ ДОСТУПА К ФАЙЛУ СОТРУДНИКОВ ===\n";
        cout << "1. Прочитать запись\n";
        cout << "2. Изменить запись\n";
        cout << "3. Выйти\n";
        cout << "Выберите операцию: ";

        int choice;
        cin >> choice;

        switch (choice) {
        case 1:
            readRecord();
            break;
        case 2:
            modifyRecord();
            break;
        case 3:
            running = false;
            break;
        default:
            cout << "Неверный выбор!\n";
            Sleep(1000);
        }
    }


    Request req = { OP_EXIT, 0 };
    Response resp;
    sendRequest(req, resp);

    CloseHandle(hPipe);
    cout << "Клиент завершил работу.\n";
    cout << "Нажмите Enter для выхода...";
    cin.ignore();
    cin.get();

    return 0;
}