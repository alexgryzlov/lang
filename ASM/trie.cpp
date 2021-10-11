#include "trie.h"

Trie::Trie() {
    root = new Node;
    vertices.push_back(root);
}

Trie::~Trie() { for (auto &i : vertices) delete i; }

int Trie::get_code(const string_view &command) {
    Node *current_node = root;
    for (size_t i = 0; i < command.len; ++i) {
        current_node = go(current_node, command.ptr[i]);
        if (current_node == nullptr) {
            return -1;
        }
    }
    return current_node->code;
}

void Trie::insert(const string_view &s, int code) {
    Node *current_node = root;
    for (size_t i = 0; i < s.len; ++i) {
        if (s.ptr[i] >= 'a' && s.ptr[i] <= 'z') {
            if (!current_node->go[s.ptr[i] - 'a']) {
                current_node->go[s.ptr[i] - 'a'] = new Node;
                vertices.push_back(current_node->go[s.ptr[i] - 'a']);
            }
            current_node = current_node->go[s.ptr[i] - 'a'];
        } else if (s.ptr[i] >= '0' && s.ptr[i] <= '9') {
            if (!current_node->go[s.ptr[i] - '0' + 26]) {
                current_node->go[s.ptr[i] - '0' + 26] = new Node;
                vertices.push_back(current_node->go[s.ptr[i] - '0' + 26]);
            }
            current_node = current_node->go[s.ptr[i] - '0' + 26];
        }
    }
    current_node->code = code;
}

Trie::Node *Trie::go(Trie::Node *from, char symbol) {
    if (symbol >= 'a' && symbol <= 'z') {
        return from->go[symbol - 'a'];
    }
    if (symbol >= '0' && symbol <= '9') {
        return from->go[symbol - '0' + 26];
    }
    return nullptr;
}
