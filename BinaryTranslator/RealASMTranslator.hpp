#ifndef LANG_REALASMTRANSLATOR_H
#define LANG_REALASMTRANSLATOR_H

#include "../common.h"
#include "OP.hpp"

class RealASMTranslator {
public:
  RealASMTranslator(Node* root);

private:
  Node* root;

  void evaluate(Node* node, /* FILE* file , */ const std::string& func);

  int global_var;
  int max_var;
  int return_reg;
  int exp_cnt;
};


#endif //LANG_ASMTRANSLATOR_H
