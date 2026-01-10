#include <windows.h>
#include <iostream>
using namespace std;

int* arr;
int n, m;

HANDLE* hThreads;
HANDLE* canNotContinue;
HANDLE* continueWork;
HANDLE* finishWork;
HANDLE startEvent;

HANDLE mutexArr;

DWORD WINAPI marker(LPVOID p)
{
    int id = (int)p;
    srand(id);

    WaitForSingleObject(startEvent, INFINITE);

    while (true)
    {
        int value = rand();
        int index = value % n;

        WaitForSingleObject(mutexArr, INFINITE);

        if (arr[index] == 0)
        {
        LeaveCriticalSection: 
            Sleep(5);
            arr[index] = id;
            Sleep(5);

            ReleaseMutex(mutexArr);
        }
        else
        {
            int count = 0;
            for (int i = 0; i < n; i++)
                if (arr[i] == id) count++;

            cout << "\nПоток " << id
                << " не может продолжать работу. Пометил: "
                << count
                << " Индекс конфликта: " << index << endl;

            ReleaseMutex(mutexArr);

            SetEvent(canNotContinue[id]);

            HANDLE waitEvents[2] = { continueWork[id], finishWork[id] };
            DWORD r = WaitForMultipleObjects(2, waitEvents, FALSE, INFINITE);

            if (r == WAIT_OBJECT_0 + 1)
            {
                WaitForSingleObject(mutexArr, INFINITE);
                for (int i = 0; i < n; i++)
                    if (arr[i] == id)
                        arr[i] = 0;
                ReleaseMutex(mutexArr);

                return 0;
            }
        }
    }
}

int main()
{
    setlocale(LC_ALL, "ru");

    cout << "Введите размер массива: ";
    cin >> n;

    arr = new int[n];
    for (int i = 0; i < n; i++) arr[i] = 0;

    cout << "Введите количество потоков marker: ";
    cin >> m;

    hThreads = new HANDLE[m];
    canNotContinue = new HANDLE[m];
    continueWork = new HANDLE[m];
    finishWork = new HANDLE[m];

    mutexArr = CreateMutex(NULL, FALSE, NULL);
    startEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    for (int i = 0; i < m; i++)
    {
        canNotContinue[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
        continueWork[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
        finishWork[i] = CreateEvent(NULL, FALSE, FALSE, NULL);

        hThreads[i] = CreateThread(NULL, 0, marker, (LPVOID)i, 0, NULL);
    }

    SetEvent(startEvent);

    int alive = m;

    while (alive > 0)
    {
        WaitForMultipleObjects(m, canNotContinue, TRUE, INFINITE);

        cout << "\nТекущее состояние массива:\n";
        for (int i = 0; i < n; i++) cout << arr[i] << " ";
        cout << endl;

        cout << "\nВведите номер потока для завершения: ";
        int k;
        cin >> k;

        SetEvent(finishWork[k]);
        WaitForSingleObject(hThreads[k], INFINITE);
        alive--;

        cout << "\nПосле освобождения мест потоком " << k << ":\n";
        for (int i = 0; i < n; i++) cout << arr[i] << " ";
        cout << endl;

        for (int i = 0; i < m; i++)
        {
            if (i == k) continue;
            ResetEvent(canNotContinue[i]);
            SetEvent(continueWork[i]);
        }
    }

    for (int i = 0; i < m; i++)
    {
        CloseHandle(hThreads[i]);
        CloseHandle(canNotContinue[i]);
        CloseHandle(continueWork[i]);
        CloseHandle(finishWork[i]);
    }

    CloseHandle(startEvent);
    CloseHandle(mutexArr);

    delete[] arr;
    delete[] hThreads;
    delete[] canNotContinue;
    delete[] continueWork;
    delete[] finishWork;

    return 0;
}
