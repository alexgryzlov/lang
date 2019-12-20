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

///                 <general> ::= <function>* | "data" <arguments>
///                 <function> := <identificator>(<arguments>)<statement>
///                 <arguments> := <identificator>{,<identificator>}* | NONE
///                 <statement> := {<statement>+} | ["if", "while"]<expression><statement> | "return"<expression>>; | "in"<arguments>; | "out"<expression-list>; | <expression>;
///                 <expression-list> := <expression>{,<expression-list>}* | NONE
///                 <expression> := <number> | <identificator> | (<expression>) | -<expression> | <identificator>(<expression-list>) || <expression> [+-<>] <expression> | <identificator> = <expression>
///                 <number> := [0-9]+



#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define PRINTF_ERR_DESCR(code, descr) case code : {printf(ANSI_COLOR_RED descr ANSI_COLOR_RESET "\n"); break;}

class Parser {
public:


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

    struct Node {
        std::vector<Node*> children;
        std::string data;
        int num;
        TokenType type;
        Node() = default;

        explicit Node(std::string s, TokenType type = TokenType::NULL_TYPE) : data(std::move(s)), children(), num(0), type(type) {}
    };

    struct Lexeme {
        const char * start;
        size_t       size;
    };

    struct Token {
        Lexeme lexeme;
        TokenType type;
    };


    Parser(std::string  text) : ptr(0), err_code(ErrorCode::NO_ERROR), text(std::move(text)), root(nullptr) {
        keywords["return"] = Keyword::RETURN;
        keywords["if"]     = Keyword::IF;
        keywords["while"]  = Keyword::WHILE;
        keywords["data"]   = Keyword::DATA;
        keywords["in"]     = Keyword::IN;
        keywords["out"]    = Keyword::OUT;
    }

    size_t ptr;
    size_t cnt = 0;
    size_t exp_cnt = 0;
    ErrorCode err_code;
    Node* root;
    std::map<std::string, Keyword> keywords;
    std::map<std::string, int> CHAR_TABLE;
    std::vector<Token> tokens;
    std::string text;

    void raise_syntax_error();
    void ERROR_INFO();


    void build_AST() {
        ptr = 0;
        root = new Node("ROOT");
        while (tokens[ptr].type != TokenType::NULL_TYPE) {
            if (tokens[ptr].type == TokenType::KEYWORD) {
                Keyword current_keyword = keywords[std::string(tokens[ptr].lexeme.start, tokens[ptr].lexeme.size)];
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

    Node* get_identificator() {
        if (tokens[ptr].type == TokenType::IDENTIFICATOR) {
            Node* result = new Node(std::string(tokens[ptr].lexeme.start, tokens[ptr].lexeme.size), TokenType::IDENTIFICATOR);
            ptr++;
            return result;
        }
        else {
            return nullptr;
        }
    }

    Node* get_arguments() {
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



    Node* get_function() {
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

        result->children.push_back(new Node("STAT"));
        result->children[1]->children.push_back(get_statement());
        result->type = TokenType::FUNCTION;
        return result;
    }

    Node* get_statement() {
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
                Keyword current_keyword = keywords[std::string(tokens[ptr].lexeme.start, tokens[ptr].lexeme.size)];
                switch (current_keyword) {
                    case Keyword::IF:
                        result = new Node("IF", TokenType::KEYWORD);
                        ptr++;
                        result->children.push_back(get_expression());
                        result->children.push_back(get_statement());
                        break;
                    case Keyword::WHILE:
                        result = new Node("WHILE", TokenType::KEYWORD);
                        ptr++;
                        result->children.push_back(get_expression());
                        result->children.push_back(get_statement());
                        break;
                    case Keyword::RETURN:
                        result = new Node("RETURN", TokenType::KEYWORD);
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
                        result = new Node("IN", TokenType::KEYWORD);
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
                        result = new Node("OUT", TokenType::KEYWORD);
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

    Node* get_data() {
        Node* result = new Node("DATA");
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

    Node* get_expression_list() {
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

    Node* get_expression() {
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


    Node* get_number() {
        if (tokens[ptr].type == TokenType::INTEGER_LITERAL) {
            Node * result = new Node(std::string(tokens[ptr].lexeme.start, tokens[ptr].lexeme.size), TokenType::INTEGER_LITERAL);
            ptr++;
            return result;
        }
        else {
            return nullptr;
        }
    }


    void separate_tokens();
    Parser::TokenType classify_word(const char* start, size_t size);
    void load_word(const char*& start, size_t& size, bool& status, int shift = 0);
    bool is_separator(char c);
    bool is_operator(char c);
    bool is_compound(const char* c);

    void print_dot(const char* file_name) {
        cnt = 0;
        enumerate(root);
        FILE *dot_output = fopen(file_name, "w");
        fprintf(dot_output, "digraph G {\n");
        fprintf(dot_output, "\tnode [shape=record];\n");
        DFS_print(root, dot_output);
        fprintf(dot_output, "}\n");
        fclose(dot_output);
    }

    void enumerate(Node* node) {
        if (!node) {
            return;
        }
        node->num = cnt++;
        for (auto& i : node->children) {
            enumerate(i);
        }
    }

    void DFS_print(Node* node, FILE* file) {
        if (!node) {
            return;
        }
        if (*node->data.c_str() == '<' || *node->data.c_str()) {
            fprintf(file, "\tNode%d [label=\"{<f0> 0x%08x |{\\%s}}\"];\n", node->num, node, node->data.c_str());
        }
        else {
            fprintf(file, "\tNode%d [label=\"{<f0> 0x%08x |{%s}}\"];\n", node->num, node, node->data.c_str());
        }
        for (auto& i : node->children) {
                fprintf(file, "\tNode%d -> Node%d:f0;\n", node->num, i->num);
        }
        for (auto & i : node->children) {
            DFS_print(i, file);
        }
    }

    // TODO: local namespaces
    void create_char_table() {
        int f_cnt = 0;
        for (auto& i : root->children) {
            if (i->data == "DATA") {
                int var_cnt = 0;
                for (auto& j : i->children) {
                    CHAR_TABLE[j->data] = var_cnt++;
                }
                continue;
            }
            CHAR_TABLE[i->data] = f_cnt++;
        }
    }

    void name_validation_initial() {
        if (!CHAR_TABLE.contains("main")) {
            printf("No entry point!");
            return;
        }
        for (auto& i : root->children) {
            if (i->data == "DATA") {
                continue;
            }
            name_validation(i->children[1]);
        }
    }

    void name_validation(Node* node) {
        if (node->type == TokenType::IDENTIFICATOR) {
            if (!CHAR_TABLE.contains(node->data)) {
                printf("FAIL no such name: %s\n", node->data.c_str());
                // RAISE SEMANTIC ERROR
            }
        }
        for (auto& i : node->children) {
            name_validation(i);
        }
    }

    void evaluate_init(const char* filename) {
        FILE* f = fopen(filename, "w");
        fprintf(f, "jmp main\n");
        for (auto& i : root->children) {
            evaluate(i, f);
        }
    }

    void evaluate(Node* node, FILE* file) {
        if (node->type == TokenType::IDENTIFICATOR) {
            if (node->children.empty()) {
                fprintf(file, "pushr r%d\n", CHAR_TABLE[node->data]);
            }
            else {
                if (node->data == "sqrt") {
                    evaluate(node->children[0], file);
                    fprintf(file, "sqrt\n");
                } else {
                    fprintf(file, "call %s\n", node->data.c_str());
                    // TODO add arguments
                }

            }
        }
        else if (node->type == TokenType::INTEGER_LITERAL) {
            fprintf(file, "push %s\n", node->data.c_str());
        }
        else if (node->type == TokenType::FUNCTION) {
            fprintf(file, "$%s\n", node->data.c_str());
            // TODO add arguments
            evaluate(node->children[1], file);
            if (node->data == "main") {
                fprintf(file, "end\n");
            } else {
                fprintf(file, "ret\n");
            }

        }
        else if (node->type == TokenType::OPERATOR) {
            if (node->data == "=") {
                evaluate(node->children[1], file);
                fprintf(file, "popr r%d\n", CHAR_TABLE[node->children[0]->data]);
            } else {
                for (auto& j : node->children) {
                    evaluate(j, file);
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
                evaluate(j, file);
            }
        }
        else if (node->data == "ARG") {
            for (auto& j : node->children) {
                evaluate(j, file);
            }
            return;
        }
        else if (node->type == TokenType::KEYWORD) {
            if (node->data == "OUT") {
                for (auto& i : node->children[0]->children) {
                    evaluate(i, file);
                    fprintf(file, "out\n"
                                  "pop\n");
                }
            }
            else if (node->data == "IN") {
                for (auto& i : node->children[0]->children) {
                    fprintf(file, "in\n"
                                  "popr r%d\n", CHAR_TABLE[i->data]);
                }
            }
            else if (node->data == "IF") {
                int cur_cnt = exp_cnt++;
                evaluate(node->children[0], file);
                fprintf(file, "push 0\n"
                              "cmptop\n"
                              "je notif%d\n", cur_cnt);
                evaluate(node->children[1], file);
                fprintf(file, "$notif%d\n", cur_cnt);
            }
            else if (node->data == "RETURN") {
                evaluate(node->children[0], file);
                fprintf(file, "ret\n");
            }
            else if (node->data == "WHILE") {
                int cur_cnt = exp_cnt++;
                int exit_cnt = exp_cnt++;
                fprintf(file, "$while%d\n", cur_cnt);
                evaluate(node->children[0], file);
                fprintf(file, "popr r999\n"
                              "push 0\n"
                              "popr r1000\n"
                              "cmp r999, r1000\n"
                              "je notif%d\n", exit_cnt);
                evaluate(node->children[1], file);
                fprintf(file, "jmp while%d\n"
                              "$notif%d\n", cur_cnt, exit_cnt);
            }
        }
    }
};

void Parser::ERROR_INFO() {
    switch (err_code) {
        PRINTF_ERR_DESCR(ErrorCode::NO_ERROR, "No error.")
        PRINTF_ERR_DESCR(ErrorCode::DATA_ERROR,  "Failed to parse DATA.")
        PRINTF_ERR_DESCR(ErrorCode::STATEMENT_ERROR,  "Failed to parse STATEMENT.")
        PRINTF_ERR_DESCR(ErrorCode::FUNCTION_ERROR,  "Failed to parse FUNCTION.")
        PRINTF_ERR_DESCR(ErrorCode::ARGUMENT_ERROR,  "Failed to parse ARGUMENTS.")
    }
}


void Parser::raise_syntax_error() {
    printf(ANSI_COLOR_RED "Syntax Error at position %zu, lexeme \'%.*s\'.\n" ANSI_COLOR_RESET, ptr, tokens[ptr].lexeme.size, tokens[ptr].lexeme.start);
    ERROR_INFO();
    exit(1);
}



void Parser::separate_tokens() {
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
}

bool Parser::is_separator(char c) {
    return c == ',' || c == '(' || c == ')' || c == '{' || c == '}' || c == ';';
}

bool Parser::is_operator(char c) {
    return c == '+' || c == '*' || c == '/' || c == '-' || c == '=' || c == '>' || c == '<';
}

bool Parser::is_compound(const char *c) {
    return (((*c == '!') && (*(c + 1) == '=')) || ((*c == '=') && (*(c + 1) == '=')) || ((*c == '<') && (*(c + 1) == '=')) ||
            ((*c == '>') && (*(c + 1) == '=')));
}

Parser::TokenType Parser::classify_word(const char *start, size_t size) {
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

void Parser::load_word(const char *&start, size_t &size, bool &status, int shift) {
    auto type = classify_word(start, size);
    tokens.push_back(Token{{start, size}, type});
    start += size + shift;
    size = 0;
    status = false;
}


int main() {
    std::ifstream t("quadratic.:)");
    std::stringstream buffer;
    buffer << t.rdbuf();
    Parser parser(buffer.str());
    parser.separate_tokens();
    parser.build_AST();
    parser.create_char_table();
    parser.name_validation_initial();
    parser.print_dot("initial.dot");
    system("dot -Tsvg -o initial.svg initial.dot");
    parser.evaluate_init("a.txt");
    return 0;
}