#include "simple_fib_pal_list.h"
#include <stdexcept>
#include <algorithm>

using namespace std;


vector<int> generateFibonacci(int n) {
    if (n <= 0) {
        throw invalid_argument("n должно быть больше 0");
    }

    vector<int> result;

    if (n >= 1) result.push_back(0);
    if (n >= 2) result.push_back(1);

    for (int i = 2; i < n; i++) {
        int next = result[i - 1] + result[i - 2];
        result.push_back(next);
    }

    return result;
}


bool isPalindrome(int number) {
    if (number < 0) return false;

    string str = to_string(number);
    string reversed = str;
    reverse(reversed.begin(), reversed.end());

    return str == reversed;
}

bool isPalindrome(const std::string& str) {
    string reversed = str;
    reverse(reversed.begin(), reversed.end());
    return str == reversed;
}

Node* createList(const vector<int>& values) {
    if (values.empty()) return nullptr;

    Node* head = new Node(values[0]);
    Node* current = head;

    for (size_t i = 1; i < values.size(); i++) {
        current->next = new Node(values[i]);
        current = current->next;
    }

    return head;
}

Node* reverseList(Node* head) {
    Node* prev = nullptr;
    Node* current = head;
    Node* next = nullptr;

    while (current != nullptr) {
        next = current->next;
        current->next = prev;
        prev = current;
        current = next;
    }

    return prev;
}

vector<int> listToVector(Node* head) {
    vector<int> result;
    Node* current = head;

    while (current != nullptr) {
        result.push_back(current->data);
        current = current->next;
    }

    return result;
}

void deleteList(Node* head) {
    while (head != nullptr) {
        Node* temp = head;
        head = head->next;
        delete temp;
    }
}