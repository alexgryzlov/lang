#include "common.h"

#include "Lexer.cpp"
#include "AST.cpp"
#include "Semantic.cpp"
#include "ASMTranslator.cpp"


void DFS_print(Node *node, FILE *file) {
    if (!node) {
        return;
    }
    if (*node->data.c_str() == '<' || *node->data.c_str()) {
        fprintf(file, "\tNode%d [label=\"{<f0> 0x%08x |{\\%s}}\"];\n", node->num, node, node->data.c_str());
    } else {
        fprintf(file, "\tNode%d [label=\"{<f0> 0x%08x |{%s}}\"];\n", node->num, node, node->data.c_str());
    }
    for (auto &i : node->children) {
        fprintf(file, "\tNode%d -> Node%d:f0;\n", node->num, i->num);
    }
    for (auto &i : node->children) {
        DFS_print(i, file);
    }
}


void enumerate(Node *node, int &cnt) {
    if (!node) {
        return;
    }
    node->num = cnt++;
    for (auto &i : node->children) {
        enumerate(i, cnt);
    }
}

void print_dot(const char *file_name, Node *root) {
    int cnt = 0;
    enumerate(root, cnt);
    FILE *dot_output = fopen(file_name, "w");
    fprintf(dot_output, "digraph G {\n");
    fprintf(dot_output, "\tnode [shape=record];\n");
    DFS_print(root, dot_output);
    fprintf(dot_output, "}\n");
    fclose(dot_output);
}

void write_node(Node *node, FILE *&output, int offset) {
    if (!node) {
        return;
    }
    std::string printf_format_string =
            std::string(offset - 1, '\t') + "{\n" + std::string(offset, '\t') + "\"%s\" %d\n";
    std::string printf_format_string1 = std::string(offset - 1, '\t') + "}\n";
    fprintf(output, printf_format_string.c_str(), node->data.c_str(), node->children.size());

    for (auto &i : node->children) {
        write_node(i, output, offset + 1);
    }
    fprintf(output, "%s", printf_format_string1.c_str());
}


Node *DFS_load(const std::string &str, int &ptr) {
    int i = 0;
    int start = 0;
    while (str[ptr] != '{') {
        ptr++;
    }
    while (str[ptr] != '\"') {
        ptr++;
    }
    ptr++;
    start = ptr;
    while (str[ptr] != '\"') {
        ptr++;
        i++;
    }
    std::string data = std::string((str.data() + start), i);
    TokenType type = TokenType::NULL_TYPE;
    ptr += 2;
    if (data == "return" || data == "if" || data == "in" || data == "out" || data == "while" || data == "data") {
        type = TokenType::KEYWORD;
    } else if (isdigit(data[0])) {
        type = TokenType::INTEGER_LITERAL;
    } else if (data == "ARG" || data == "STAT" || data == "COMPOUND") {
        type = TokenType::NULL_TYPE;
    } else if (isalpha(data[0])) {
        type = TokenType::IDENTIFICATOR;
    } else if (data == "+" || data == "-" || data == "/" || data == "*" || data == "=" || data == "<" || data == "==") {
        type = TokenType::OPERATOR;
    }
    size_t num = str[ptr];
    Node *result = new Node(data, type);
    for (int j = 0; j < num - '0'; ++j) {
        result->children.push_back(DFS_load(str, ptr));
    }
    return result;
}

Node *load(const char *filename) {
    int i = 0;
    std::ifstream t(filename);
    std::stringstream buffer;
    buffer << t.rdbuf();


    Node *root = DFS_load(buffer.str(), i);
    for (auto &i : root->children) {
        if (i->data == "data") {
            continue;
        }
        i->type = TokenType::FUNCTION;
    }
    return root;
}


std::map<std::string, Keyword> prepare_keywords() {
    static const std::map<std::string, Keyword> keywords = {
            {"return", Keyword::RETURN},
            {"if",     Keyword::IF},
            {"while",  Keyword::WHILE},
            {"data",   Keyword::DATA},
            {"in",     Keyword::IN},
            {"out",    Keyword::OUT}
    };
    return keywords;
}

const std::string HELP_STRING = "Invalid number of arguments. Expected 3.\n"
                                "[input_file] [asm_output] [AST_img]\n";

int main(int argc, char *argv[]) {
    if (argc != 4) {
        std::cout << HELP_STRING << std::endl;
        return 1;
    }

    auto keywords = prepare_keywords();
    std::ifstream reader(argv[1]);
    std::stringstream input;
    input << reader.rdbuf();
    Lexer lexer(input.str(), keywords);
    auto tokens = lexer.separate_tokens();

    Node *root = AST(tokens, keywords).get_root();
    auto char_table = Semantic(root, keywords).get_symbol_table();
    ASMTranslator(argv[2], root, char_table);

    auto tree_dot = std::string(argv[3]) + ".dot";
    auto tree_svg = std::string(argv[3]) + ".svg";
    auto tree_dump = std::string(argv[3]) + ".dump";
    print_dot(tree_dot.c_str(), root);

    auto command = "dot -Tsvg -o " + tree_svg + " " + tree_dot;
    system(command.c_str());

    FILE *dump = fopen(tree_dump.c_str(), "w");
    write_node(root, dump, 1);
    fclose(dump);
}