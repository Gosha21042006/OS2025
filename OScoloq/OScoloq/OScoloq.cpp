#include <iostream>
#include "simple_fib_pal_list.h"

using namespace std;

void demoFibonacci() {
    cout << "1. Числа Фибоначчи:" << endl;

    try {
        vector<int> fib10 = generateFibonacci(10);
        cout << "   Первые 10 чисел: ";
        for (int num : fib10) {
            cout << num << " ";
        }
        cout << endl;

        vector<int> fib5 = generateFibonacci(5);
        cout << "   Первые 5 чисел: ";
        for (int num : fib5) {
            cout << num << " ";
        }
        cout << endl;
    }
    catch (const exception& e) {
        cout << "   Ошибка: " << e.what() << endl;
    }
}

void demoPalindrome() {
    cout << "\n2. Проверка палиндромов:" << endl;

    int testNumbers[] = { 121, 123, 1221, 12321, 12345, 0, 7 };

    for (int num : testNumbers) {
        bool result = isPalindrome(num);
        cout << "   " << num << " -> " << (result ? "палиндром" : "не палиндром") << endl;
    }

    cout << "   Строки:" << endl;
    string testStrings[] = { "radar", "hello", "madam", "world" };

    for (const string& str : testStrings) {
        bool result = isPalindrome(str);
        cout << "   \"" << str << "\" -> " << (result ? "палиндром" : "не палиндром") << endl;
    }
}

void demoLinkedList() {
    cout << "\n3. Разворот связного списка:" << endl;

    vector<int> original = { 1, 2, 3, 4, 5 };
    Node* list = createList(original);

    cout << "   Исходный список: ";
    vector<int> originalVec = listToVector(list);
    for (int val : originalVec) {
        cout << val << " ";
    }
    cout << endl;

    Node* reversed = reverseList(list);

    cout << "   Развернутый список: ";
    vector<int> reversedVec = listToVector(reversed);
    for (int val : reversedVec) {
        cout << val << " ";
    }
    cout << endl;

    deleteList(reversed);
}

int main() {
    cout << "=== ДЕМОНСТРАЦИЯ РАБОТЫ ПРОГРАММЫ ===" << endl;

    demoFibonacci();
    demoPalindrome();
    demoLinkedList();

    cout << "\n=== ВСЕ ДЕМОНСТРАЦИИ ЗАВЕРШЕНЫ ===" << endl;

    return 0;
}