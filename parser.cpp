#include <cstdio>
#include <cstdlib>
#include <cstring>

/// \brief Parser to calculate simple expressions with integers.
/// \details Allowed operations: +, -, *, /, (). Only for positive numbers.
///          Grammar:
///                 (next syntax is similiar to regex and $ is a terminal symbol that equals to \0 in C++)
///                 G := E$
///                 E := T {[+-] T}*
///                 T := P {[*/] P}*
///                 P := -?{(E) | N}
///                 N := [0-9]+

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define PRINTF_ERR_DESCR(code, descr) case code : {printf(ANSI_COLOR_RED descr ANSI_COLOR_RESET "\n"); break;}

class Parser {
public:
    typedef enum {
        NO_ERROR = 0,
        G_ERROR  = 1,
        N_ERROR  = 2,
        E_ERROR  = 3,
        P_ERROR = 4
    } ErrorCode;

    Parser() : ptr(nullptr), start(nullptr), err_code(NO_ERROR) {}
    char * ptr;
    char * start;
    ErrorCode err_code;


    void raise_syntax_error();
    void ERROR_INFO();

    /// \brief General rule
    int evaluate(char expression[]);

    /// \brief N rule
    int get_N();

    /// \brief E rule
    int get_E();

    /// \brief T rule
    int get_T();

    /// \brief P rule
    int get_P();
};

void Parser::ERROR_INFO() {
    switch (err_code) {
        PRINTF_ERR_DESCR(NO_ERROR, "No error.")
        PRINTF_ERR_DESCR(G_ERROR,  "G failed to parse.")
        PRINTF_ERR_DESCR(E_ERROR,  "E failed to parse.")
        PRINTF_ERR_DESCR(N_ERROR,  "N failed to parse.")
        PRINTF_ERR_DESCR(P_ERROR,  "P failed to parse.")
    }
}

void Parser::raise_syntax_error() {
    printf(ANSI_COLOR_RED "Syntax Error at position %ld, symbol \'%c\'.\n" ANSI_COLOR_RESET, ptr - start, *ptr);
    ERROR_INFO();
    exit(0);
}

int Parser::evaluate(char *expression) {
    ptr = start = expression;
    int val = get_E();
    if (*ptr == '\0') {
        return val;
    }
    err_code = G_ERROR;
    raise_syntax_error();
}

int Parser::get_E() {
    int val1 = get_T();
    while (*ptr == '+' || *ptr == '-') {
        char * op = ptr;
        ++ptr;
        int val2 = get_T();
        *op == '+'? val1 += val2 : val1 -= val2;
    }
    return val1;
}

int Parser::get_N() {
    int val = 0;

    if (!('0' <= *ptr && *ptr <= '9')) {
        err_code = N_ERROR;
        raise_syntax_error();
    }

    do  {
        val = val * 10 + (*ptr - '0');
        ++ptr;
    } while ('0' <= *ptr && *ptr <= '9');
    return val;
}

int Parser::get_T() {
    int val1 = get_P();
    while (*ptr == '*' || *ptr == '/') {
        char * op = ptr;
        ++ptr;
        int val2 = get_P();
        *op == '*'? val1 *= val2 : val1 /= val2;
    }
    return val1;
}

int Parser::get_P() {
    int sign = 1;
    if (*ptr == '-') {
        sign = -1;
        ++ptr;
    }
    if (*ptr == '(') {
        ++ptr;
        int val = get_E();
        if (*ptr != ')') {
            err_code = P_ERROR;
            raise_syntax_error();
        }
        ++ptr;
        return sign * val;
    } else {
        return sign * get_N();
    }
}

int main() {
    char expression[256];
    char * end = nullptr;
    Parser parser;
    fgets(expression, sizeof(expression), stdin);
    if (end = strchr(expression, '\n')) {
        *end = '\0';
    }
    printf("%d", parser.evaluate(expression));
    return 0;
}