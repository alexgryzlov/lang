#include "Lexer.h"

Lexer::Lexer(std::string text, const std::map<std::string, Keyword>& keywords) : text(std::move(text)), keywords(keywords) {}

std::vector<Token> Lexer::separate_tokens() {
    bool in_word = false;
    size_t word_size = 0;
    const char* current_word_start = text.data();
    for (size_t i = 0; i < text.size(); ++i) {
        if (!in_word && isspace(text[i])) {
            current_word_start++;
        }
        else if (('a' <= text[i] && text[i] <= 'z') || ('0' <= text[i] && text[i] <= '9') || (text[i] == '_')) {
            in_word = true;
            word_size++;
        }
        else if (is_separator(text[i])) {
            if (in_word) {
                load_word(current_word_start, word_size, in_word);
            }
            tokens.push_back(Token{{current_word_start++, 1}, TokenType::SEPARATOR});

        }
        else if (is_compound(&text[i])) {
            if (in_word) {
                load_word(current_word_start, word_size, in_word);
            }
            tokens.push_back(Token{{current_word_start, 2}, TokenType::OPERATOR});
            current_word_start += 2;
            ++i;
        }
        else if (is_operator(text[i])) {
            if (in_word) {
                load_word(current_word_start, word_size, in_word);
            }
            tokens.push_back(Token{{current_word_start++, 1}, TokenType::OPERATOR});
        }
        else if (in_word && isspace(text[i])) {
            load_word(current_word_start, word_size, in_word, 1);
        }
    }

    // This part adds last word and NULL_TYPE to the end of token vector
    if (in_word) {
        load_word(current_word_start, word_size, in_word);
    }
    load_word(current_word_start, word_size, in_word);
    return tokens;
}

bool Lexer::is_separator(char c) {
    return c == ',' || c == '(' || c == ')' || c == '{' || c == '}' || c == ';';
}

bool Lexer::is_operator(char c) {
    return c == '+' || c == '*' || c == '/' || c == '-' || c == '=' || c == '>' || c == '<';
}

bool Lexer::is_compound(const char *c) {
    return (((*c == '!') && (*(c + 1) == '=')) || ((*c == '=') && (*(c + 1) == '=')) || ((*c == '<') && (*(c + 1) == '=')) ||
            ((*c == '>') && (*(c + 1) == '=')));
}

TokenType Lexer::classify_word(const char *start, size_t size) {
    TokenType type = TokenType::NULL_TYPE;
    if (keywords.contains(std::string(start, size))) {
        type = TokenType::KEYWORD;
    }
    else {
        if (isdigit(*start)) {
            type = TokenType::INTEGER_LITERAL;
        }
        else if (*start == '\"') {
            type = TokenType::STRING_LITERAL;
        }
        else if (isalpha(*start)) {
            type = TokenType::IDENTIFICATOR;
        }
        else if (is_separator(*start)) {
            type = TokenType::SEPARATOR;
        }
        else if (is_operator(*start)) {
            type = TokenType::OPERATOR;
        }
    }
    return type;
}

void Lexer::load_word(const char *&start, size_t &size, bool &status, int shift) {
    auto type = classify_word(start, size);
    tokens.push_back(Token{{start, size}, type});
    start += size + shift;
    size = 0;
    status = false;
}


