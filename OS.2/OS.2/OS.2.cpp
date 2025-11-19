#include <iostream>
#include <windows.h>
#include <vector>
#include <limits>

using namespace std;

vector<double> arr;
double minElement, maxElement;
double averageValue;

DWORD WINAPI min_max_thread(LPVOID param) {
    if (arr.empty()) return 0;

    minElement = arr[0];
    maxElement = arr[0];

    for (size_t i = 1; i < arr.size(); i++) {
        if (arr[i] < minElement) {
            minElement = arr[i];
        }
        if (arr[i] > maxElement) {
            maxElement = arr[i];
        }
        Sleep(7); 
    }

    cout << "Минимальный элемент: " << minElement << endl;
    cout << "Максимальный элемент: " << maxElement << endl;

    return 0;
}

DWORD WINAPI average_thread(LPVOID param) {
    if (arr.empty()) return 0;

    double sum = 0;

    for (size_t i = 0; i < arr.size(); i++) {
        sum += arr[i];
        Sleep(12); 
    }

    averageValue = sum / arr.size();
    cout << "Среднее арифметическое: " << averageValue << endl;

    return 0;
}

int main() 
{
    setlocale(LC_ALL, "rus");
    int n;
    cout << "Введите размерность массива: ";
    cin >> n;

    if (n <= 0) {
        cout << "Размерность массива должна быть положительной!" << endl;
        return 1;
    }

    arr.resize(n);
    cout << "Введите " << n << " элементов массива:" << endl;
    for (int i = 0; i < n; i++) {
        cin >> arr[i];
    }

    cout << "Исходный массив: ";
    for (int i = 0; i < n; i++) {
        cout << arr[i] << " ";
    }
    cout << endl;

    HANDLE hMinMax = CreateThread(NULL, 0, min_max_thread, NULL, 0, NULL);
    HANDLE hAverage = CreateThread(NULL, 0, average_thread, NULL, 0, NULL);

    if (hMinMax == NULL || hAverage == NULL) {
        cout << "Ошибка при создании потоков!" << endl;
        return 1;
    }

    WaitForSingleObject(hMinMax, INFINITE);
    WaitForSingleObject(hAverage, INFINITE);

    CloseHandle(hMinMax);
    CloseHandle(hAverage);

    for (int i = 0; i < n; i++) {
        if (arr[i] == minElement || arr[i] == maxElement) {
            arr[i] = static_cast<int>(averageValue);
        }
    }

    cout << "Модифицированный массив: ";
    for (int i = 0; i < n; i++) {
        cout << arr[i] << " ";
    }
    cout << endl;

    return 0;
}