#include "Semantic.h"

std::map<std::string, std::map<std::string, int>> Semantic::create_symbol_table() {
    int f_cnt = 0;
    int data_index = -1;
    // Create all functions
    for (size_t i = 0; i < root->children.size(); ++i) {
        if (root->children[i]->type == TokenType::KEYWORD) {
            data_index = i;
            continue;
        }
        TABLE[root->children[i]->data];
    }

    // Create standard functions
    TABLE["sqrt"];
    TABLE["sqr"];
    if (data_index != -1) {
        global_var = root->children[data_index]->children.size();
        for (size_t i = 0; i < root->children[data_index]->children.size(); ++i) {
            for (size_t j = 0; j < root->children.size(); ++j) {
                if (j == data_index) {
                    continue;
                }
                TABLE[root->children[j]->data][root->children[data_index]->children[i]->data] = i;
            }
        }
    }
    for (size_t i = 0; i < root->children.size(); ++i) {
        if (i == data_index) {
            continue;
        }
        for (size_t j = 0; j < root->children[i]->children[0]->children.size(); ++j) {
            TABLE[root->children[i]->data][root->children[i]->children[0]->children[j]->data] = TABLE[root->children[i]->data].size();
        }
    }
    for (size_t i = 0; i < root->children.size(); ++i) {
        if (i == data_index) {
            continue;
        }
        add_local_vars(root->children[i], root->children[i]->data);
    }
    for (size_t i = 0; i < root->children.size(); ++i) {
        if (i == data_index) {
            continue;
        }
        if (TABLE[root->children[i]->data].size() > max_var) {
            max_var = TABLE[root->children[i]->data].size();
        }
    }
    return TABLE;
}

void Semantic::add_local_vars(Node* node, const std::string& func) {
    if (node->type == TokenType::KEYWORD) {
        if (keywords.at(node->data) == Keyword::DATA) {
            for (auto& i : node->children) {
                TABLE[func][i->data] = TABLE[func].size();
            }
            return;
        }
    }
    for (auto& i : node->children) {
        add_local_vars(i, func);
    }
}

void Semantic::name_validation_initial() {
    if (!TABLE.contains("main")) {
        printf("No entry point!");
        return;
    }
    for (auto& i : root->children) {
        if (i->data == "data") {
            continue;
        }
        name_validation(i->children[1], i->data);
    }
}

void Semantic::name_validation(Node* node, const std::string& func) {
    if (node->type == TokenType::IDENTIFICATOR) {
        if (!TABLE.contains(node->data) && !TABLE[func].contains(node->data)) {
            printf("FAIL no such name: %s\n", node->data.c_str());
            // RAISE SEMANTIC ERROR
        }
    }
    for (auto& i : node->children) {
        name_validation(i, func);
    }
}

Semantic::Semantic(Node *root, const std::map<std::string, Keyword> &keywords) : root(root), keywords(keywords), global_var(0), max_var(0) {
    create_symbol_table();
    name_validation_initial();
}

SYMBOL_TABLE Semantic::get_symbol_table() {
    return {TABLE, global_var, max_var};
}

