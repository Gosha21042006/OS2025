#include <iostream>
#include <vector>
#include <cassert>
#include "simple_fib_pal_list.h"

using namespace std;

#define TEST(condition) \
    do { \
        if (!(condition)) { \
            cout << "TEST FAILED: " << #condition << " at line " << __LINE__ << endl; \
            return false; \
        } else { \
            cout << "TEST PASSED: " << #condition << endl; \
        } \
    } while(0)


bool testFibonacci() {
    cout << "\n=== Тесты Фибоначчи ===" << endl;
    
    
    vector<int> fib1 = generateFibonacci(1);
    TEST(fib1.size() == 1);
    TEST(fib1[0] == 0);
    
 
    vector<int> fib5 = generateFibonacci(5);
    vector<int> expected = {0, 1, 1, 2, 3};
    TEST(fib5 == expected);
    

    try {
        generateFibonacci(0);
        TEST(false); 
    } catch (const exception& e) {
        TEST(string(e.what()).find("больше 0") != string::npos);
    }
    
    return true;
}


bool testPalindrome() {
    cout << "\n=== Тесты палиндромов ===" << endl;
    

    TEST(isPalindrome(121) == true);
    TEST(isPalindrome(1221) == true);
    TEST(isPalindrome(12321) == true);
    TEST(isPalindrome(0) == true);
    TEST(isPalindrome(7) == true);
    

    TEST(isPalindrome(123) == false);
    TEST(isPalindrome(1234) == false);
    TEST(isPalindrome(10) == false);
    

    TEST(isPalindrome(-121) == false);
    

    TEST(isPalindrome("aba") == true);
    TEST(isPalindrome("hello") == false);
    
    return true;
}


bool testLinkedList() {
    cout << "\n=== Тесты связного списка ===" << endl;
    

    vector<int> values = {1, 2, 3, 4, 5};
    Node* list = createList(values);
    
    vector<int> listValues = listToVector(list);
    TEST(listValues == values);
    

    Node* reversed = reverseList(list);
    vector<int> reversedValues = listToVector(reversed);
    vector<int> expected = {5, 4, 3, 2, 1};
    TEST(reversedValues == expected);
    
    deleteList(reversed);
    

    Node* emptyList = createList({});
    TEST(emptyList == nullptr);
    
    Node* reversedEmpty = reverseList(emptyList);
    TEST(reversedEmpty == nullptr);
    

    Node* singleList = createList({42});
    Node* reversedSingle = reverseList(singleList);
    vector<int> singleResult = listToVector(reversedSingle);
    TEST(singleResult == vector<int>({42}));
    
    deleteList(reversedSingle);
    
    return true;
}


bool testIntegration() {
    cout << "\n=== Интеграционные тесты ===" << endl;
    

    vector<int> fibNumbers = generateFibonacci(10);
    
    int palindromeCount = 0;
    for (int num : fibNumbers) {
        if (isPalindrome(num)) {
            palindromeCount++;
        }
    }
    
    TEST(palindromeCount > 0); // Должны быть хотя бы 0 и 1
    

    Node* fibList = createList(fibNumbers);
    Node* reversedFibList = reverseList(fibList);
    
    vector<int> reversedFib = listToVector(reversedFibList);
    TEST(reversedFib.size() == fibNumbers.size());
    TEST(reversedFib[0] == fibNumbers[fibNumbers.size()-1]);
    
    deleteList(reversedFibList);
    
    return true;
}

int main() {
    cout << "Запуск простых unit-тестов..." << endl;
    
    bool allTestsPassed = true;
    
    allTestsPassed &= testFibonacci();
    allTestsPassed &= testPalindrome();
    allTestsPassed &= testLinkedList();
    allTestsPassed &= testIntegration();
    
    cout << "\n=== РЕЗУЛЬТАТ ===" << endl;
    if (allTestsPassed) {
        cout << " ВСЕ ТЕСТЫ ПРОЙДЕНЫ!" << endl;
    } else {
        cout << " НЕКОТОРЫЕ ТЕСТЫ ПРОВАЛЕНЫ" << endl;
        return 1;
    }
    
    return 0;
}
