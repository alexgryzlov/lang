//
// Created by alex on 12/20/19.
//

#ifndef LANG_LEXER_H
#define LANG_LEXER_H
#include <iostream>
#include <vector>
#include <utility>
#include <string>
#include <map>
#include "common.h"

class Lexer {
public:
    explicit            Lexer(std::string  text, const std::map<std::string, Keyword>& keywords);
    std::vector<Token>  separate_tokens();
private:
    TokenType   classify_word(const char* start, size_t size);
    void        load_word(const char*& start, size_t& size, bool& status, int shift = 0);
    static bool is_separator(char c);
    static bool is_operator(char c);
    static bool is_compound(const char* c);

    std::string text;
    const std::map<std::string, Keyword>& keywords;
    std::vector<Token> tokens;
};


#endif //LANG_LEXER_H
