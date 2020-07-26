#ifndef LANG_SEMANTIC_H
#define LANG_SEMANTIC_H

#include "common.h"

class Semantic {
public:
    explicit Semantic(Node* root, const std::map<std::string, Keyword>& keyword);
    SYMBOL_TABLE get_symbol_table();
private:
    Node* root;
    int global_var;
    int max_var;
    std::map<std::string, std::map<std::string, int>> TABLE;
    const std::map<std::string, Keyword>& keywords;

    std::map<std::string, std::map<std::string, int>> create_symbol_table();
    void add_local_vars(Node* node, const std::string& func);
    void name_validation_initial();
    void name_validation(Node* node, const std::string& func);
};


#endif //LANG_SEMANTIC_H
