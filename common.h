#ifndef LANG_COMMON_H
#define LANG_COMMON_H
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <utility>
#include <vector>
#include <cctype>
#include <set>
#include <map>
#include <string>
#include <string_view>
#include <fstream>
#include <sstream>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define PRINTF_ERR_DESCR(code, descr) case code : {printf(ANSI_COLOR_RED descr ANSI_COLOR_RESET "\n"); break;}

enum class ErrorCode {
    NO_ERROR        = 0,
    FUNCTION_ERROR  = 1,
    ARGUMENT_ERROR  = 2,
    STATEMENT_ERROR = 3,
    DATA_ERROR      = 4
};

enum class TokenType {
    NULL_TYPE       = 0,
    KEYWORD         = 1,
    SEPARATOR       = 2,
    OPERATOR        = 3,
    INTEGER_LITERAL = 4,
    STRING_LITERAL  = 5,
    IDENTIFICATOR   = 6,
    FUNCTION        = 7
};

enum class Keyword {
    RETURN  = 0,
    IF      = 1,
    WHILE   = 2,
    DATA    = 3,
    IN      = 4,
    OUT     = 5
};


struct Lexeme {
    const char * start;
    size_t       size;
};

struct Token {
    Lexeme lexeme;
    TokenType type;
};

struct Node {
    std::vector<Node*> children;
    std::string data;
    int num;
    TokenType type;
    Node() = default;

    explicit Node(std::string s, TokenType type = TokenType::NULL_TYPE) : data(std::move(s)), children(), num(0), type(type) {}
};

struct SYMBOL_TABLE {
    std::map<std::string, std::map<std::string, int>> TABLE;
    int global_var;
    int max_var;
};

#endif //LANG_COMMON_H
