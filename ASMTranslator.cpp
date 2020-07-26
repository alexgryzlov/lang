#include "ASMTranslator.h"

ASMTranslator::ASMTranslator(const char* filename, Node* root, struct SYMBOL_TABLE& SYMBOL_TABLE) : root(root), TABLE(SYMBOL_TABLE), return_reg(101), exp_cnt(0), global_var(TABLE.global_var), max_var(TABLE.max_var) {
    FILE* f = fopen(filename, "w");
    fprintf(f, "jmp main\n");
    for (auto& i : root->children) {
        evaluate(i, f, i->data);
    }
    fclose(f);
}

void ASMTranslator::evaluate(Node* node, FILE* file, const std::string& func) {
    if (node->type == TokenType::IDENTIFICATOR) {
        if (node->children.empty()) {
            fprintf(file, "pushr r%d\n", TABLE.TABLE[func][node->data]);
        }
        else {
            if (node->data == "sqrt") {
                evaluate(node->children[0], file, func);
                fprintf(file, "sqrt\n");
            } else {
                for (int i = global_var; i < max_var; ++i) {
                    fprintf(file, "pushr r%d\n", i);
                }
                evaluate(node->children[0], file, func);
                fprintf(file, "call %s\n", node->data.c_str());
                for (int i = max_var - 1; i >= global_var; --i) {
                    fprintf(file, "popr r%d\n", i);
                }
                fprintf(file, "pushr r%d\n", return_reg);
            }
        }
    }
    else if (node->type == TokenType::INTEGER_LITERAL) {
        fprintf(file, "push %s\n", node->data.c_str());
    }
    else if (node->type == TokenType::FUNCTION) {
        fprintf(file, "$%s\n", node->data.c_str());
        for (int i = node->children[0]->children.size() + global_var - 1; i >= global_var; --i) {
            fprintf(file, "popr r%d\n", i);
        }
        evaluate(node->children[1], file, func);
        if (node->data == "main") {
            fprintf(file, "end\n");
        } else {
            fprintf(file, "ret\n");
        }

    }
    else if (node->type == TokenType::OPERATOR) {
        if (node->data == "=") {
            evaluate(node->children[1], file, func);
            fprintf(file, "popr r%d\n", TABLE.TABLE[func][node->children[0]->data]);
        } else {
            for (auto& j : node->children) {
                evaluate(j, file, func);
            }
            if (node->data == "+") {
                fprintf(file, "add\n");
            }
            if (node->data == "-") {
                if (node->children.size() == 1) {
                    fprintf(file, "push -1\n"
                                  "mul\n");
                } else {
                    fprintf(file, "sub\n");
                }
            }
            if (node->data == "*") {
                fprintf(file, "mul\n");
            }
            if (node->data == "/") {
                fprintf(file, "div\n");
            }
            if (node->data == "<") {
                fprintf(file, "less\n");
            }
            if (node->data == "==") {
                fprintf(file, "equal\n");
            }
        }
    }
    else if (node->data == "COMPOUND" || node->data == "STAT") {
        for (auto& j : node->children) {
            evaluate(j, file, func);
        }
    }
    else if (node->data == "ARG") {
        for (auto& j : node->children) {
            evaluate(j, file, func);
        }
        return;
    }
    else if (node->type == TokenType::KEYWORD) {
        if (node->data == "out") {
            for (auto& i : node->children[0]->children) {
                evaluate(i, file, func);
                fprintf(file, "out\n"
                              "pop\n");
            }
        }
        else if (node->data == "in") {
            for (auto& i : node->children[0]->children) {
                fprintf(file, "in\n"
                              "popr r%d\n", TABLE.TABLE[func][i->data]);
            }
        }
        else if (node->data == "if") {
            int cur_cnt = exp_cnt++;
            evaluate(node->children[0], file, func);
            fprintf(file, "push 0\n"
                          "cmptop\n"
                          "je notif%d\n", cur_cnt);
            evaluate(node->children[1], file, func);
            fprintf(file, "$notif%d\n", cur_cnt);
        }
        else if (node->data == "return") {
            evaluate(node->children[0], file, func);
            fprintf(file, "popr r%d\n", return_reg);
            fprintf(file, "ret\n");
        }
        else if (node->data == "while") {
            int cur_cnt = exp_cnt++;
            int exit_cnt = exp_cnt++;
            fprintf(file, "$while%d\n", cur_cnt);
            evaluate(node->children[0], file, func);
            fprintf(file, "push 0\n"
                          "cmptop\n"
                          "je notif%d\n", exit_cnt);
            evaluate(node->children[1], file, func);
            fprintf(file, "jmp while%d\n"
                          "$notif%d\n", cur_cnt, exit_cnt);
        }
    }
}