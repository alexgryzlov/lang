#include <iostream>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <cctype>
#include <set>
#include <map>
#include <string>
#include <string_view>
#include <fstream>
#include <sstream>

///                 G - General, F - function, A - agruments, I - ID, V - variable, S - statement, E - exprssion
///                 G := F* | data A
///                 F := I(A)S | I()S
///                 A := I{,I}* | NONE
///                 S := {S+} | ["if", "while"]ES | "return"E; | "data"V+ | ["in", "out"] A; | E;
///                 V := I=E,V | I,V | I; | I=E;
///                 E := N | STR | I | (E) | I(C) | I() | E [+-<>] E | I = E
///                 N := [0-9]+
///                 STR := [a-zA-Z]
///                 C := E,C | E


#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define PRINTF_ERR_DESCR(code, descr) case code : {printf(ANSI_COLOR_RED descr ANSI_COLOR_RESET "\n"); break;}

class Parser {
public:


    enum class ErrorCode {
        NO_ERROR = 0,
        G_ERROR  = 1,
        N_ERROR  = 2,
        E_ERROR  = 3,
        P_ERROR = 4
    };

    enum class TokenType {
        NULL_TYPE = 0,
        KEYWORD = 1,
        SEPARATOR = 2,
        OPERATOR = 3,
        INTEGER_LITERAL = 4,
        STRING_LITERAL = 5,
        IDENTIFICATOR = 6,
        FUNCTION = 7
    };

    struct Node {
        std::vector<Node*> children;
        std::string data;
        int num;
        TokenType type;
        Node() = default;

        explicit Node(const std::string &s, TokenType type = TokenType::NULL_TYPE) : data(s), children(), num(0), type(type) {}
    };

    struct Lexeme {
        const char * start;
        size_t       size;
    };

    struct Token {
        Lexeme lexeme;
        TokenType type;
    };


    Parser(const std::string& text) : ptr(0), err_code(ErrorCode::NO_ERROR), text(text), root(nullptr) {
        token_type["return"] = TokenType::KEYWORD;
        token_type["if"]     = TokenType::KEYWORD;
        token_type["while"]  = TokenType::KEYWORD;
        token_type["data"]   = TokenType::KEYWORD;
        token_type["in"]     = TokenType::KEYWORD;
        token_type["out"]    = TokenType::KEYWORD;
    }

    size_t ptr;
    size_t cnt = 0;
    size_t exp_cnt = 0;
    ErrorCode err_code;
    Node* root;
    Node* curr_node;
    std::map<std::string, TokenType> token_type;
    std::map<std::string, int> CHAR_TABLE;
    std::vector<Token> tokens;
    std::string text;

    void raise_syntax_error();
    void ERROR_INFO();


    void build_AST() {
        ptr = 0;
        root = new Node("ROOT");
        while (tokens[ptr].type != TokenType::NULL_TYPE) {
            if (!strncmp(tokens[ptr].lexeme.start, "data", 4)) {
                ptr++;
                root->children.push_back(get_V());
            }
            else {
                root->children.push_back(get_F());
            }
        }
    }

    Node* get_I() {
        if (tokens[ptr].type == TokenType::IDENTIFICATOR) {
            Node* result = new Node(std::string(tokens[ptr].lexeme.start, tokens[ptr].lexeme.size), TokenType::IDENTIFICATOR);
            ptr++;
            return result;
        }
        else {
            return nullptr;
        }
    }

    Node* get_A() {
        Node* result = new Node("ARG");
        result->children.push_back(get_I());
        while (*tokens[ptr].lexeme.start == ',') {
            ptr++;
            result->children.push_back(get_I());
        }
        return result;
    }



    Node* get_F() {
        Node *result = get_I();
        if (*tokens[ptr].lexeme.start == '(') {
            ptr++;
            if (*tokens[ptr].lexeme.start == ')') {
                result->children.push_back(new Node("ARG"));
            } else {
                result->children.push_back(get_A());
            }
        }
        if (*tokens[ptr].lexeme.start == ')') {
            ptr++;
        }

        result->children.push_back(new Node("STAT"));
        result->children[1]->children.push_back(get_S());
        result->type = TokenType::FUNCTION;
        return result;
    }

    Node* get_S() {
        Node* result = nullptr;
        if (*(tokens[ptr].lexeme.start) == '{') {
            result = new Node("COMPOUND");
            ptr++;
            while (*tokens[ptr].lexeme.start != '}') {
                result->children.push_back(get_S());
            }
            ptr++;
        }
        else if (tokens[ptr] .type == TokenType::KEYWORD) {
            if (!strncmp(tokens[ptr].lexeme.start, "if", 2)) {
                result = new Node("IF", TokenType::KEYWORD);
                ptr++;
                result->children.push_back(get_E());
                result->children.push_back(get_S());
            }
            else if (!strncmp(tokens[ptr].lexeme.start, "while", 5)) {
                result = new Node("WHILE", TokenType::KEYWORD);
                ptr++;
                if (*tokens[ptr].lexeme.start == '(') {
                    result->children.push_back(get_E());
                }
                if (*tokens[ptr].lexeme.start == ')') {
                    ptr++;
                }
                result->children.push_back(get_S());
            }
            else if (!strncmp(tokens[ptr].lexeme.start, "return", 6)) {
                result = new Node("RETURN", TokenType::KEYWORD);
                ptr++;
                result->children.push_back(get_E());
                if (*tokens[ptr].lexeme.start == ';') {
                    ptr++;
                }
            }
            else if (!strncmp(tokens[ptr].lexeme.start, "return", 6)) {
                result = new Node("RETURN", TokenType::KEYWORD);
                ptr++;
                result->children.push_back(get_E());
                if (*tokens[ptr].lexeme.start == ';') {
                    ptr++;
                }
            }
            else if (!strncmp(tokens[ptr].lexeme.start, "in", 2)) {
                result = new Node("IN", TokenType::KEYWORD);
                ptr++;
                result->children.push_back(get_A());
                if (*tokens[ptr].lexeme.start == ';') {
                    ptr++;
                }
            }
            else if (!strncmp(tokens[ptr].lexeme.start, "out", 2)) {
                result = new Node("OUT", TokenType::KEYWORD);
                ptr++;
                result->children.push_back(get_C());
                if (*tokens[ptr].lexeme.start == ';') {
                    ptr++;
                }
            }
        }
        else {
            result = get_E();
            if (*tokens[ptr].lexeme.start == ';') {
                ptr++;
            }
        }
        return result;
    }

    Node* get_V() {
        Node* result = new Node("DATA");
        while (*tokens[ptr].lexeme.start != ';') {
            result->children.push_back(get_I());
            if (*tokens[ptr].lexeme.start == ',') {
                ptr++;
            }
        }
        ptr++;
        return result;
    }

    Node* get_C() {
        Node* result = new Node("ARG");
        result->children.push_back(get_E());
        while (*tokens[ptr].lexeme.start == ',') {
            ptr++;
            result->children.push_back(get_E());
        }
        ptr++;
        return result;

    }

    Node* get_E() {
        Node* result = nullptr;
        result = get_N();
        if (!result) {
            result = get_STR();
            if (!result) {
                result = get_I();
                if (result) {
                    if (*tokens[ptr].lexeme.start == '(') {
                        ptr++;
                        if (*tokens[ptr].lexeme.start == ')') {
                            result->children.push_back(new Node("ARG"));
                        } else {
                            result->children.push_back(get_A());
                        }
                        if (*tokens[ptr].lexeme.start == ')') {
                            ptr++;
                        }
                    }
                    else if (*tokens[ptr].lexeme.start == '=') {
                        Node* tmp = result;
                        result = new Node(std::string(tokens[ptr].lexeme.start, tokens[ptr].lexeme.size), TokenType::OPERATOR);
                        ptr++;
                        result->children.push_back(tmp);
                        result->children.push_back(get_E());
                        return result;
                    }
                }
            }
            if ( *tokens[ptr].lexeme.start == '(') {
                ptr++;
                result = get_E();
                if ( *tokens[ptr].lexeme.start == ')') {
                    ptr++;
                }
            }
        }
        if ((tokens[ptr].lexeme.size == 1 && ( *tokens[ptr].lexeme.start == '-' || *tokens[ptr].lexeme.start == '/'|| *tokens[ptr].lexeme.start == '+' ||
            *tokens[ptr].lexeme.start == '*' || *tokens[ptr].lexeme.start == '<' || *tokens[ptr].lexeme.start == '>')) ||
            (tokens[ptr].lexeme.size == 2 && (!strncmp(tokens[ptr].lexeme.start, "!=", tokens[ptr].lexeme.size) ||
            !strncmp(tokens[ptr].lexeme.start, "==", tokens[ptr].lexeme.size)))) {
            Node* tmp = result;
            result = new Node(std::string(tokens[ptr].lexeme.start, tokens[ptr].lexeme.size), TokenType::OPERATOR);
            ptr += 1;
            result->children.push_back(tmp);
            result->children.push_back(get_E());
        }
        return result;
    }


    Node* get_N() {
        if (tokens[ptr].type == TokenType::INTEGER_LITERAL) {
            Node * result = new Node(std::string(tokens[ptr].lexeme.start, tokens[ptr].lexeme.size), TokenType::INTEGER_LITERAL);
            ptr++;
            return result;
        }
        else {
            return nullptr;
        }
    }

    Node* get_STR() {
        if (tokens[ptr].type == TokenType::STRING_LITERAL) {
            Node * result = new Node(std::string(tokens[ptr].lexeme.start, tokens[ptr].lexeme.size), TokenType::STRING_LITERAL);
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
                fprintf(file, "call %s\n", node->data.c_str());
                // TODO add arguments
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
                    fprintf(file, "sub\n");
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
                fprintf(file, "popr r999\n"
                              "push 0\n"
                              "popr r1000\n"
                              "cmp r999, r1000\n"
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
        PRINTF_ERR_DESCR(ErrorCode::G_ERROR,  "G failed to parse.")
        PRINTF_ERR_DESCR(ErrorCode::E_ERROR,  "E failed to parse.")
        PRINTF_ERR_DESCR(ErrorCode::N_ERROR,  "N failed to parse.")
        PRINTF_ERR_DESCR(ErrorCode::P_ERROR,  "P failed to parse.")
    }
}


void Parser::raise_syntax_error() {
    printf(ANSI_COLOR_RED "Syntax Error at position %zu, lexeme \'%.*s\'.\n" ANSI_COLOR_RESET, ptr, tokens[ptr].lexeme.size, tokens[ptr].lexeme.start);
    ERROR_INFO();
    exit(0);
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
    return c == ',' || c == '(' || c == ')' || c == '{' || c == '}' || c == ';' || c == '>' || c == '<';
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
    try {
        type = token_type.at(std::string(start, size));
    }
    catch (const std::out_of_range& e) {
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
    std::ifstream t("while.:)");
    std::stringstream buffer;
    buffer << t.rdbuf();
    Parser parser(buffer.str());
    int * a = nullptr;
    parser.separate_tokens();
    parser.build_AST();
    parser.create_char_table();
    parser.name_validation_initial();
    parser.print_dot("initial.dot");
    system("dot -Tsvg -o initial.svg initial.dot");
    parser.evaluate_init("a.txt");
    return 0;
}