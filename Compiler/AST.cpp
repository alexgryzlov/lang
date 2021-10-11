#include "AST.h"

AST::AST(const std::vector<Token>& tokens, const std::map<std::string, Keyword>& keywords) : ptr(0), keywords(keywords), tokens(tokens), root(nullptr), err_code(ErrorCode::NO_ERROR){
    root = new Node("ROOT");
    while (tokens[ptr].type != TokenType::NULL_TYPE) {
        if (tokens[ptr].type == TokenType::KEYWORD) {
            Keyword current_keyword = keywords.at(std::string(tokens[ptr].lexeme.start, tokens[ptr].lexeme.size));
            if (current_keyword == Keyword::DATA) {
                ptr++;
                root->children.push_back(get_data());
            } else {
                err_code = ErrorCode::DATA_ERROR;
                raise_syntax_error();
            }
        }
        else {
            root->children.push_back(get_function());
        }
    }
}

Node* AST::get_identificator() {
    if (tokens[ptr].type == TokenType::IDENTIFICATOR) {
        Node* result = new Node(std::string(tokens[ptr].lexeme.start, tokens[ptr].lexeme.size), TokenType::IDENTIFICATOR);
        ptr++;
        return result;
    }
    else {
        return nullptr;
    }
}

Node* AST::get_arguments() {
    Node* result = new Node("ARG");
    Node* first_arg = get_identificator();
    if (first_arg) {
        result->children.push_back(first_arg);
    }
    while (*tokens[ptr].lexeme.start == ',') {
        ptr++;
        result->children.push_back(get_identificator());
    }
    return result;
}



Node* AST::get_function() {
    Node *result = get_identificator();
    if (*tokens[ptr].lexeme.start == '(') {
        ptr++;
        result->children.push_back(get_arguments());
    } else {
        err_code = ErrorCode::FUNCTION_ERROR;
        raise_syntax_error();
    }
    if (*tokens[ptr].lexeme.start == ')') {
        ptr++;
    } else {
        err_code = ErrorCode::FUNCTION_ERROR;
        raise_syntax_error();
    }

    result->children.push_back(get_statement());
    result->type = TokenType::FUNCTION;
    return result;
}

Node* AST::get_statement() {
    Node* result = nullptr;
    if (*(tokens[ptr].lexeme.start) == '{') {
        result = new Node("COMPOUND");
        ptr++;
        while (*tokens[ptr].lexeme.start != '}') {
            result->children.push_back(get_statement());
        }
        ptr++;
    }
    else if (tokens[ptr].type == TokenType::KEYWORD) {
        if (keywords.contains(std::string(tokens[ptr].lexeme.start, tokens[ptr].lexeme.size))) {
            Keyword current_keyword = keywords.at(std::string(tokens[ptr].lexeme.start, tokens[ptr].lexeme.size));
            switch (current_keyword) {
                case Keyword::IF:
                    result = new Node("if", TokenType::KEYWORD);
                    ptr++;
                    result->children.push_back(get_expression());
                    result->children.push_back(get_statement());
                    break;
                case Keyword::WHILE:
                    result = new Node("while", TokenType::KEYWORD);
                    ptr++;
                    result->children.push_back(get_expression());
                    result->children.push_back(get_statement());
                    break;
                case Keyword::RETURN:
                    result = new Node("return", TokenType::KEYWORD);
                    ptr++;
                    result->children.push_back(get_expression());
                    if (*tokens[ptr].lexeme.start == ';') {
                        ptr++;
                    } else {
                        err_code = ErrorCode::STATEMENT_ERROR;
                        raise_syntax_error();
                    }
                    break;
                case Keyword::IN:
                    result = new Node("in", TokenType::KEYWORD);
                    ptr++;
                    result->children.push_back(get_arguments());
                    if (*tokens[ptr].lexeme.start == ';') {
                        ptr++;
                    } else {
                        err_code = ErrorCode::STATEMENT_ERROR;
                        raise_syntax_error();
                    }
                    break;
                case Keyword::OUT:
                    result = new Node("out", TokenType::KEYWORD);
                    ptr++;
                    result->children.push_back(get_expression_list());
                    if (*tokens[ptr].lexeme.start == ';') {
                        ptr++;
                    } else {
                        err_code = ErrorCode::STATEMENT_ERROR;
                        raise_syntax_error();
                    }
                    break;
                case Keyword::DATA:
                    ptr++;
                    result = get_data();
                    break;

            }
        }
    }

    else {
        result = get_expression();
        if (*tokens[ptr].lexeme.start == ';') {
            ptr++;
        } else {
            err_code = ErrorCode::STATEMENT_ERROR;
            raise_syntax_error();
        }
    }
    return result;
}

Node* AST::get_data() {
    Node* result = new Node("data", TokenType::KEYWORD);
    Node* current_id;
    while (current_id = get_identificator()) {
        result->children.push_back(current_id);
        if (*tokens[ptr].lexeme.start == ',') {
            ptr++;
        }
    }
    if (*tokens[ptr].lexeme.start == ';') {
        ptr++;
    } else {
        err_code = ErrorCode::DATA_ERROR;
        raise_syntax_error();
    }
    return result;
}

Node* AST::get_expression_list() {
    Node* result = new Node("ARG");
    Node* first_expr = get_expression();
    if (first_expr) {
        result->children.push_back(first_expr);
    }
    while (*tokens[ptr].lexeme.start == ',') {
        ptr++;
        result->children.push_back(get_expression());
    }
    return result;
}

Node* AST::get_expression() {
    Node* result = nullptr;
    if (*tokens[ptr].lexeme.start == '-') {
        result = new Node(std::string(tokens[ptr].lexeme.start, tokens[ptr].lexeme.size), TokenType::OPERATOR);
        ptr += 1;
        result->children.push_back(get_expression());
        return result;
    }
    if (!(result = get_number())) {
        if ((result = get_identificator()) || !strncmp(tokens[ptr].lexeme.start, "sqrt", 4)) {
            if (*tokens[ptr].lexeme.start == '(') {
                if (!strncmp(tokens[ptr].lexeme.start, "sqrt", 4)) {
                    result = new Node("sqrt");
                }
                ptr++;
                result->children.push_back(get_expression_list());
                if (*tokens[ptr].lexeme.start == ')') {
                    ptr++;
                }
            }
            else if (*tokens[ptr].lexeme.start == '=') {
                Node* tmp = result;
                result = new Node(std::string(tokens[ptr].lexeme.start, tokens[ptr].lexeme.size), TokenType::OPERATOR);
                ptr++;
                result->children.push_back(tmp);
                result->children.push_back(get_expression());
                return result;
            }
        }
    }
    if ( *tokens[ptr].lexeme.start == '(') {
        ptr++;
        result = get_expression();
        if ( *tokens[ptr].lexeme.start == ')') {
            ptr++;
        }
    }
    if (tokens[ptr].type == TokenType::OPERATOR) {
        Node* tmp = result;
        result = new Node(std::string(tokens[ptr].lexeme.start, tokens[ptr].lexeme.size), TokenType::OPERATOR);
        ptr += 1;
        result->children.push_back(tmp);
        result->children.push_back(get_expression());
    }
    return result;
}


Node* AST::get_number() {
    if (tokens[ptr].type == TokenType::INTEGER_LITERAL) {
        Node * result = new Node(std::string(tokens[ptr].lexeme.start, tokens[ptr].lexeme.size), TokenType::INTEGER_LITERAL);
        ptr++;
        return result;
    }
    else {
        return nullptr;
    }
}

Node *AST::get_root() const {
    return this->root;
}

void AST::raise_syntax_error() {
    printf(ANSI_COLOR_RED "Syntax Error at position %d, lexeme \'%.*s\'.\n" ANSI_COLOR_RESET, ptr, tokens[ptr].lexeme.size, tokens[ptr].lexeme.start);
    ERROR_INFO();
    exit(1);
}

void AST::ERROR_INFO() {
    switch (err_code) {
        PRINTF_ERR_DESCR(ErrorCode::NO_ERROR, "No error.")
        PRINTF_ERR_DESCR(ErrorCode::DATA_ERROR,  "Failed to parse DATA.")
        PRINTF_ERR_DESCR(ErrorCode::STATEMENT_ERROR,  "Failed to parse STATEMENT.")
        PRINTF_ERR_DESCR(ErrorCode::FUNCTION_ERROR,  "Failed to parse FUNCTION.")
        PRINTF_ERR_DESCR(ErrorCode::ARGUMENT_ERROR,  "Failed to parse ARGUMENTS.")
    }
}
