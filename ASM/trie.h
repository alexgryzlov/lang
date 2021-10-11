#pragma once

#include "text_proc.h"

#include <string>
#include <vector>

class Trie {
public:
    Trie();

    ~Trie();

    int get_code(const string_view &command);

    void insert(const string_view &s, int code);

private:
    struct Node {
        Node *go[36];
        int code;

        Node() : code(-1), go{nullptr} {}
    };

    Node *root;
    std::vector<Node *> vertices;

    Node *go(Node *from, char symbol);
};
