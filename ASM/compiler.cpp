#include <unistd.h>

#include "text_proc.h"
#include "trie.cpp"

class Compiler {
public:
    Compiler();

    ~Compiler() {
        if (text) free(text);
        if (compiled_text) free(compiled_text);
    }

    void compile(const char *OUTPUT_FILE, const char *INPUT_FILE);

    void list(const char *OUTPUT_FILE, const char *INPUT_FILE);

    void dissasm(const char *OUTPUT_FILE, const char *INPUT_FILE);

private:
    int pc;
    int size;
    Trie command_trie;
    Trie label_trie;
    int *compiled_text;
    size_t text_size;
    string_view *text;
    const int LABEL_CODE = 14631;

    inline bool is_label(const string_view &word) { return word.ptr[0] == '$'; }

    int get_lexeme(const string_view &word);
};

Compiler::Compiler() : pc(0), size(0), compiled_text(nullptr), text_size(0), text(nullptr) {
#include "codegen/create_trie.cpp"
}

void Compiler::compile(const char *OUTPUT_FILE, const char *INPUT_FILE) {
    char *initial_text = nullptr;
    auto compiled = fopen(OUTPUT_FILE, "w");
    long long SIZE = read_input(INPUT_FILE, initial_text);
    text_size = separate_by_words(initial_text, SIZE, text);
    // First stage
    for (pc = 0; pc < text_size; ++pc) {
        if (is_label(text[pc])) {
            label_trie.insert({text[pc].ptr + 1, text[pc].len - 1}, pc);
        }
    }
    // Second stage
    compiled_text = (int *) calloc(text_size, sizeof(int));
    this->size = text_size;
    for (pc = 0; pc < text_size; ++pc) {
        uint64_t current_lexeme = get_lexeme(text[pc]);
        compiled_text[pc] = current_lexeme;
        // Get command arguments
        string_view tmp;
        switch (current_lexeme) {
#include "codegen/compile.cpp"
        }
    }
    for (int i = 0; i < text_size; ++i) {
        fprintf(compiled, "%d ", compiled_text[i]);
    }
    fclose(compiled);
}

int Compiler::get_lexeme(const string_view &word) {
    if (is_label(word)) {
        return LABEL_CODE;
    }
    return command_trie.get_code(word);
}

void Compiler::dissasm(const char *OUTPUT_FILE, const char *INPUT_FILE) {
    auto intial = fopen(OUTPUT_FILE, "w");
    int label_cnt = 0;
    char *raw_compiled_text = nullptr;
    string_view *compiled_string_view = nullptr;
    long long SIZE = read_input(INPUT_FILE, raw_compiled_text);
    text_size = separate_strings(raw_compiled_text, SIZE, compiled_string_view);
    compiled_text = (int *) calloc(text_size, sizeof(int));
    for (long long i = 0; i < text_size; ++i) {
        compiled_text[i] = strtol(compiled_string_view[i].ptr, NULL, 10);
    }
    for (pc = 0; pc < text_size; ++pc) {
        if (compiled_text[pc] == LABEL_CODE) {
            compiled_text[pc] = 0b10000000 + label_cnt++;
        }
    }
    for (pc = 0; pc < text_size; ++pc) {
        uint64_t current_command = compiled_text[pc];
        // Get command arguments
        if (compiled_text[pc] & 0b10000000) {
            fprintf(intial, "$label%d\n", compiled_text[pc] & 0b1111);
            continue;
        }
        switch (current_command) {
#include "codegen/disassembly.cpp"
        }
    }
    free(raw_compiled_text);
    free(compiled_text);
}

void Compiler::list(const char *OUTPUT_FILE, const char *INPUT_FILE) {
    char *initial_text = nullptr;
    auto listing = fopen(OUTPUT_FILE, "w");
    long long SIZE = read_input(INPUT_FILE, initial_text);
    text_size = separate_strings(initial_text, SIZE, text);
    // First stage
    for (pc = 0; pc < text_size; ++pc) {
        if (is_label(text[pc])) {
            label_trie.insert({text[pc].ptr + 1, text[pc].len - 1}, pc);
        }
    }
    // Second stage
    compiled_text = (int *) calloc(text_size, sizeof(int));
    this->size = text_size;
    for (pc = 0; pc < text_size; ++pc) {
        uint64_t current_lexeme = get_lexeme(text[pc]);
        compiled_text[pc] = current_lexeme;
        // Get command arguments
        string_view tmp;
        switch (current_lexeme) {
#include "codegen/compile.cpp"
        }
    }
    int pc1 = 0;
    int pc2 = 0;
    for (int pc1 = 0; pc1 < text_size; ++pc1, ++pc2) {
        int current_lexeme = compiled_text[pc1];
        switch (current_lexeme) {
#include "codegen/listing.cpp"
        }
    }
    free(initial_text);
    fclose(listing);
}


int main(int argc, char **argv) {
    Compiler compiler;
    char default_output[] = "a.txt";
    char default_input[] = "input.txt";
    char *output_filename = default_output;
    char *input_filename = default_input;
    int key = 0;
    bool listing = false;
    while ((key = getopt(argc, argv, ":o:i:l")) != -1) {
        switch (key) {
            case 'l':
                listing = true;
                break;
            case 'o':
                output_filename = optarg;
                break;
            case 'i':
                input_filename = optarg;
                break;
        }
    }
    if (optind != argc) {
        input_filename = argv[optind];
    }
    if (listing) {
        compiler.list("listing", input_filename);
        return 0;
    }
    compiler.compile(output_filename, input_filename);
    return 0;
}
