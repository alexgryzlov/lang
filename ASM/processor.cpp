#define SUPER_DEBUG

#include <iostream>
#include <cmath>
#include <cstring>
#include <cinttypes>
#include <unistd.h>

#include "trie.h"
#include "SafeStackDynamicOnePlace.hpp"
#include "text_proc.h"

class Processor {
public:
    Processor();

    void write_output() {
        for (int i = 0; i < size; ++i) {
            std::cout << compiled_text[i] << std::endl;
        }
    }

    void execute(char *file_name);

private:
    inline void pushr();

    inline void popr();

    inline void add();

    inline void out();

    inline void in();

    inline void push();

    inline void pop();

    inline void jmp();

    inline void je();

    inline void end();

    inline void cmp();

    inline void jb();

    inline void call();

    inline void ret();

    inline void mul();

    inline void sub();

    inline void ja();

    inline void jbe();

    inline void jae();

    inline void jne();

    inline void sqrt();

    inline void sqr();

    inline void div();

    inline void less();

    inline void equal();

    inline void cmptop();

    int pc;
    uint8_t flag;
    int *compiled_text;
    int size;
    Stack<int, 8> stack;
    Stack<int, 8> call_stack;
    int r[1001];
    int cmp_num1, cmp_num2;
    const uint8_t SF = 0b00000010;
    const uint8_t ZF = 0b00000001;
};

Processor::Processor() : compiled_text(nullptr), size(0), stack(get_var_name(stack)),
                         call_stack(get_var_name(call_stack)), pc(0), flag(0) {}

inline void Processor::cmp() {
    flag = 0;
    int arg1 = r[compiled_text[++pc]];
    int result = arg1 - r[compiled_text[++pc]];

    if (result < 0) flag |= SF;
    if (result == 0) flag |= ZF;
}

inline void Processor::cmptop() {
    flag = 0;
    int arg1 = stack.top();
    stack.pop();
    int result = stack.top() - arg1;
    stack.pop();

    if (result < 0) flag |= SF;
    if (result == 0) flag |= ZF;
}

inline void Processor::pushr() {
    stack.push(r[compiled_text[++pc]]);
}

inline void Processor::push() {
    stack.push(compiled_text[++pc]);
}

inline void Processor::pop() {
    stack.pop();
}

inline void Processor::popr() {
    r[compiled_text[++pc]] = stack.top();
    stack.pop();
}

inline void Processor::jmp() {
    int pos = compiled_text[++pc];
    pc = pos;
}

inline void Processor::add() {
    int first_arg = stack.top();
    stack.pop();
    int second_arg = stack.top();
    stack.pop();
    stack.push(first_arg + second_arg);
}

inline void Processor::mul() {
    int first_arg = stack.top();
    stack.pop();
    int second_arg = stack.top();
    stack.pop();
    stack.push(first_arg * second_arg);
}

inline void Processor::sub() {
    int second_arg = stack.top();
    stack.pop();
    int first_arg = stack.top();
    stack.pop();
    stack.push(first_arg - second_arg);
}

inline void Processor::div() {
    int second_arg = stack.top();
    stack.pop();
    int first_arg = stack.top();
    stack.pop();
    stack.push(first_arg / second_arg);
}


inline void Processor::out() {
    std::cout << stack.top() << std::endl;
}

inline void Processor::call() {
    call_stack.push(pc + 1);
    int pos = compiled_text[++pc];
    pc = pos;
}

inline void Processor::ret() {
    pc = call_stack.top();
    call_stack.pop();
}

inline void Processor::end() {
    exit(0);
}

inline void Processor::in() {
    int value = 0;
    std::cin >> value;
    stack.push(value);
}

inline void Processor::je() {
    ++pc;
    if (flag & ZF) {
        pc = compiled_text[pc];
    }
}

inline void Processor::jb() {
    ++pc;
    if (flag & SF) {
        pc = compiled_text[pc];
    }
}

inline void Processor::ja() {
    ++pc;
    if (!(flag & SF) & !(flag & ZF)) {
        pc = compiled_text[pc];
    }
}

inline void Processor::jbe() {
    ++pc;
    if (flag & SF || flag & ZF) {
        pc = compiled_text[pc];
    }
}

inline void Processor::jae() {
    ++pc;
    if (!(flag & SF)) {
        pc = compiled_text[pc];
    }
}

inline void Processor::jne() {
    ++pc;
    if (!(flag & ZF)) {
        pc = compiled_text[pc];
    }
}

inline void Processor::sqrt() {
    int arg = stack.top();
    stack.pop();
    stack.push(int(std::sqrt(arg)));
}

inline void Processor::sqr() {
    int arg = stack.top();
    stack.pop();
    stack.push(arg * arg);
}

inline void Processor::less() {
    int second_arg = stack.top();
    stack.pop();
    int first_arg = stack.top();
    stack.pop();
    stack.push(int(first_arg < second_arg));
}

inline void Processor::equal() {
    int second_arg = stack.top();
    stack.pop();
    int first_arg = stack.top();
    stack.pop();
    stack.push(first_arg == second_arg);
}

void Processor::execute(char *file_name) {
    char *initial_text = nullptr;
    string_view *compiled_string_view = nullptr;
    long long SIZE = read_input(file_name, initial_text);
    long long text_size = separate_strings(initial_text, SIZE, compiled_string_view);
    compiled_text = (int *) calloc(text_size, sizeof(int));
    size = text_size;
    for (long long i = 0; i < size; ++i) {
        compiled_text[i] = strtol(compiled_string_view[i].ptr, NULL, 10);
    }
    for (pc = 0; pc < size; ++pc) {
        int current_command = compiled_text[pc];
        switch (current_command) {
#include "codegen/execute.cpp"
        }
    }
    free(initial_text);
    free(compiled_string_view);
}

int main(int argc, char **argv) {
    Processor proc;
    proc.execute(argv[1]);
    return 0;
}
