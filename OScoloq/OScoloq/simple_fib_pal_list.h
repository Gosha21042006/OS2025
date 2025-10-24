#ifndef SIMPLE_FIB_PAL_LIST_H
#define SIMPLE_FIB_PAL_LIST_H

#include <vector>
#include <string>


std::vector<int> generateFibonacci(int n);


bool isPalindrome(int number);
bool isPalindrome(const std::string& str);


struct Node {
    int data;
    Node* next;
    Node(int val) : data(val), next(nullptr) {}
};

Node* reverseList(Node* head);
Node* createList(const std::vector<int>& values);
std::vector<int> listToVector(Node* head);
void deleteList(Node* head);

#endif