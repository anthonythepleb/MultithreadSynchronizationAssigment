#pragma once
#include <iostream>
#include <queue>
#include <vector>
struct Node
{
    char data;
    int freq;
    Node *left;
    Node *right;
    int counter;
    Node(char d, int f, int c)
    {
        left = nullptr;
        right = nullptr;
        data = d;
        freq = f;
        counter = c;
    }
};

struct compare
{
    bool operator()(const Node *left, const Node *right)
    {
        if (left->freq == right->freq)
        {
            if (left->data == right->data)
            {
                return left->counter < right->counter;
            }
            return (int)left->data > (int)right->data;
        }
        return left->freq > right->freq;
    }
};

class huffman
{
private:
    Node *head;

public:
    huffman() { head = nullptr; }
    Node *getHead() { return head; }
    void printCodes(Node *root, std::string str)
    {
        if (root == nullptr)
        {
            return;
        }
        printCodes(root->left, str + "0");
        if (root->data != '\0')
        {
            std::cout << "Symbol: " << root->data << ", Frequency: " << root->freq
                      << ", Code: ";
        }
        printCodes(root->right, str + "1");
    }

    Node *createTree(std::vector<std::pair<char, int>> &a)
    {
        Node *left, *right, *top;
        int count = 0;
        std::priority_queue<Node *, std::vector<Node *>, compare> pq;
        for (auto pair : a)
        {
            pq.push(new Node(pair.first, pair.second, count));
            count++;
        }

        while (pq.size() > 1)
        {
            left = pq.top();
            pq.pop();
            right = pq.top();
            pq.pop();

            top = new Node('\0', left->freq + right->freq, count++);
            top->left = left;
            top->right = right;

            pq.push(top);
        }
        head = pq.top();
        return head;
    }
    std::string decode_file(std::string s)
    {
        Node *curr = head;
        std::string final;
        for (char c : s)
        {
            if (c == '0')
            {
                curr = curr->left;
            }
            else
            {
                curr = curr->right;
            }
            if (!curr->left && !curr->right)
            {
                final += curr->data;
                curr = head;
            }
        }
        return final + "\0";
    }
};