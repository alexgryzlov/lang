//
// Created by alex on 12/20/19.
//

#ifndef LANG_AST_H
#define LANG_AST_H

#include "common.h"

class AST {
public:
    explicit AST(const std::vector<Token>& tokens,  const std::map<std::string, Keyword>& keywords);
    Node* get_root() const;
private:
    Node* get_identificator();
    Node* get_arguments();
    Node* get_function();
    Node* get_statement();
    Node* get_data();
    Node* get_expression_list();
    Node* get_expression();
    Node* get_number();



    void raise_syntax_error();
    void ERROR_INFO();


    int ptr;
    Node* root;
    ErrorCode err_code;
    const std::map<std::string, Keyword>& keywords;
    const std::vector<Token>& tokens;
};


#endif //LANG_AST_H
