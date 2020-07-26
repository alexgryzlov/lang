//
// Created by alex on 12/20/19.
//

#ifndef LANG_ASMTRANSLATOR_H
#define LANG_ASMTRANSLATOR_H

#include "common.h"

class ASMTranslator {
public:
    ASMTranslator(const char* filename, Node* root, SYMBOL_TABLE& SYMBOL_TABLE);

private:
    Node* root;
    SYMBOL_TABLE& TABLE;

    void evaluate(Node* node, FILE* file, const std::string& func);

    int global_var;
    int max_var;
    int return_reg;
    int exp_cnt;
};


#endif //LANG_ASMTRANSLATOR_H
