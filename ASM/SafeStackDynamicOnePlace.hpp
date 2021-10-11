#ifndef TASK2_SAFESTACKDYNAMIC_HPP
#define TASK2_SAFESTACKDYNAMIC_HPP

//#define SUPER_DEBUG

#include <iostream>
#include <cxxabi.h>
#include <cassert>


#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define PRINTF_ERR_DESCR(code, descr) case code : {printf(ANSI_COLOR_RED descr ANSI_COLOR_RESET "\n"); break;}
#define PRINTF_OK_DESCR(code, descr) case code: { printf(ANSI_COLOR_GREEN descr ANSI_COLOR_RESET "\n"); break;}
#define get_var_name(var) #var
#define END_CANARY *((int*)(this->data->buffer + this->data->MAX_SIZE))


typedef enum {
    NO_ERROR         = 0,
    CANARY_FAULT     = 1,
    HASH_FAULT       = 2,
    POISON_FAULT     = 3,
    POP_FAULT        = 4,
    PUSH_FAULT       = 5,
    TOP_FAULT        = 6,
    ALLOCATION_FAULT = 7
} ErrorCode;


#define ASSERT_OK(fake_arg1, fake_arg2)


#ifdef SUPER_DEBUG
#define ASSERT_OK(stack_ptr, func_name) \
    if (stack_ptr->data->err_code == NO_ERROR) \
    { stack_ptr->data->err_code = stack_ptr->verify(); } \
    if (stack_ptr->data->err_code != NO_ERROR) \
    { stack_ptr->DUMP(func_name); }
#endif

template <typename T>
struct Poison {
    static T VALUE() {
        return T();
    }
};


const int      CANARY_VALUE = 0x2152; // not 0xDEAD
const long int MOD = 1000000000 + 7;
const double   MINIMAL_RATIO = 0.25;


template <typename T, int SIZE>
class Stack {
public:
    explicit Stack(const char* var_name);
    explicit Stack(const char* var_name, void* data);
    ~Stack() { assert(data); free(data); }

    int         push(const T& value);
    int         pop();
    bool        empty();
    void        clear();
    size_t      size();
    T           top();
    void        FORCE_DUMP() { ASSERT_OK(this, __PRETTY_FUNCTION__); DUMP(__PRETTY_FUNCTION__); }

private:
    struct StackData {
    public:
        explicit StackData(const char* var_name);

        int         start_canary;
        const char* var_name;
        T           POISON_VALUE;
        ErrorCode   err_code;
        size_t      buffer_size;
        size_t      MAX_SIZE;
        long long   control_hash;
        double      fill_ratio;
        T           buffer[];
    };

    StackData* data;

    long long   hash();
    void        ERROR_INFO();
    void        DUMP(const char* func_name);
    void        create_new_buffer(size_t new_size);
    ErrorCode   verify();

    class __asserter {
    public:
        Stack<T, SIZE> *stack_ptr;
        const char* func_name;
        explicit __asserter(Stack<T, SIZE> *stack_ptr, const char* func_name) :
                stack_ptr(stack_ptr), func_name(func_name)
        { ASSERT_OK(stack_ptr, func_name); }

        ~__asserter() { ASSERT_OK(stack_ptr, func_name); }
    };

};



template <typename T, int SIZE>
Stack<T, SIZE>::StackData::StackData(const char* var_name) : start_canary(CANARY_VALUE), var_name(var_name),
                                                             POISON_VALUE(Poison<T>::VALUE()), err_code(NO_ERROR),
                                                             buffer_size(0), MAX_SIZE(SIZE), control_hash(0),
                                                             fill_ratio(0) {}


template <typename T, int SIZE>
Stack<T, SIZE>::Stack(const char* var_name) {
    data = (StackData*)calloc(sizeof(StackData) + sizeof(T) * SIZE + sizeof(int), 1);
    if (data == nullptr) {
        assert(!"ALLOCATION ERROR: CANNOT ALLOCATE INITIAL BUFFER");
        exit(ALLOCATION_FAULT);
    }
    new (data) StackData(var_name);
#ifdef SUPER_DEBUG
    data->control_hash = this->hash();
    for (int i = 0; i < SIZE; ++i) {
        data->buffer[i] = data->POISON_VALUE;
    }
    END_CANARY = CANARY_VALUE;
#endif
}

template <typename T, int SIZE>
Stack<T, SIZE>::Stack(const char* var_name, void* data) {
    this->data = (StackData*)data;
    if (data == nullptr) {
        assert(!"ALLOCATION ERROR: CANNOT ALLOCATE INITIAL BUFFER");
        exit(ALLOCATION_FAULT);
    }
    new (data) StackData(var_name);
#ifdef SUPER_DEBUG
    this->data->control_hash = this->hash();
    for (int i = 0; i < SIZE; ++i) {
        this->data->buffer[i] = this->data->POISON_VALUE;
    }
    END_CANARY = CANARY_VALUE;
#endif
}

template <typename T, int SIZE>
void Stack<T, SIZE>::DUMP(const char* func_name) {
    int status(0);
    auto real_type_name = abi::__cxa_demangle(typeid(*this).name(), nullptr, nullptr, &status);
    printf("DUMPED FROM: %s::%s\n", __FILE__, func_name);
    printf("TYPE NAME: %s; VARIABLE_NAME: %s; REAL_SIZE: %zu\n", real_type_name, data->var_name, data->MAX_SIZE);
    printf("CONTROL HASH = %lld; HASH() = %lld\n", data->control_hash, hash());
    printf("FIRST CANARY = %d; SECOND_CANARY = %d\n", data->start_canary, END_CANARY);
    ERROR_INFO();
    for (int i = 0; i < data->MAX_SIZE; ++i) {
        printf("%s[%d] --- ", data->var_name, i);
        std::cout << data->buffer[i];
        if (i >= data->buffer_size)
            printf("\t(POISON)");
        printf("\n");
    }
    free(real_type_name);
    exit(0);
}

template <typename T, int SIZE>
void Stack<T, SIZE>::ERROR_INFO() {
    switch (data->err_code) {
        PRINTF_OK_DESCR(NO_ERROR,          "NO ERROR")
        PRINTF_ERR_DESCR(CANARY_FAULT,     "CANARY VALUE CHANGED")
        PRINTF_ERR_DESCR(HASH_FAULT,       "HASH VALUE DOESN'T MATCH")
        PRINTF_ERR_DESCR(POISON_FAULT,     "POISON VALUE IS CORRUPTED")
        PRINTF_ERR_DESCR(POP_FAULT,        "UNABLE TO POP FROM EMPTY STACK")
        PRINTF_ERR_DESCR(PUSH_FAULT,       "UNABLE TO PUSH IN FULL STACK")
        PRINTF_ERR_DESCR(TOP_FAULT,        "UNABLE TO GET TOP OF THE EMPTY STACK")
        PRINTF_ERR_DESCR(ALLOCATION_FAULT, "UNABLE TO ALLOCATE REQUIRED SPACE")
    }
}


template <typename T, int SIZE>
long long Stack<T, SIZE>::hash() {
    char *temp_buffer = (char *)data->buffer;
    auto hash = (long long)this->data;
    for (size_t i = 0; i < data->buffer_size * sizeof(T); ++i) {
        hash = (hash * 257 + temp_buffer[i]) % MOD;
    }
    return hash;
}


template <typename T, int SIZE>
ErrorCode Stack<T, SIZE>::verify() {
#ifdef SUPER_DEBUG
    if (data->start_canary != CANARY_VALUE || END_CANARY != CANARY_VALUE) {
        return CANARY_FAULT;
    }

    if (hash() != data->control_hash) {
        return HASH_FAULT;
    }

    for (int i = data->buffer_size; i < data->MAX_SIZE; ++i) {
        if (data->buffer[i] != data->POISON_VALUE) {
            return POISON_FAULT;
        }
    }
#endif
    return NO_ERROR;
}


template <typename T, int SIZE>
size_t Stack<T, SIZE>::size() {
    __asserter checker(this, __PRETTY_FUNCTION__);
    return data->buffer_size;
}


template <typename T, int SIZE>
bool Stack<T, SIZE>::empty() {
    __asserter checker(this, __PRETTY_FUNCTION__);
    return !data->buffer_size;
}

template<typename T, int SIZE>
void Stack<T, SIZE>::create_new_buffer(size_t new_size) {
    StackData* new_data = (StackData*)calloc(sizeof(StackData) + sizeof(T) * new_size + sizeof(int), 1);
    new (new_data) StackData(data->var_name);
    new_data->buffer_size = data->buffer_size;
    new_data->MAX_SIZE = new_size;
    for (int i = 0; i < new_data->MAX_SIZE; ++i) {
        new_data->buffer[i] = data->buffer[i];
    }

#ifdef SUPER_DEBUG
    for (int i = data->MAX_SIZE + 1; i < new_data->MAX_SIZE; ++i) {
        new_data->buffer[i] = new_data->POISON_VALUE;
    }
#endif
    data = new_data;
    data->fill_ratio = (double)data->buffer_size / data->MAX_SIZE;

#ifdef SUPER_DEBUG
    END_CANARY = CANARY_VALUE;
#endif
}

template <typename T, int SIZE>
int Stack<T, SIZE>::push(const T& value) {
    __asserter checker(this, __PRETTY_FUNCTION__);
    if (data->buffer_size == data->MAX_SIZE) {
        create_new_buffer(data->MAX_SIZE * 2);
    }
    data->buffer[data->buffer_size++] = value;
#ifdef SUPER_DEBUG
    data->control_hash = hash();
#endif
    data->fill_ratio = (double) data->buffer_size / data->MAX_SIZE;
    return 0;
}


template <typename T, int SIZE>
int Stack<T, SIZE>::pop() {
    __asserter checker(this, __PRETTY_FUNCTION__);
    if (data->buffer_size == 0) {
        data->err_code = POP_FAULT;
        return 1;
    }
    data->buffer[--data->buffer_size] = data->POISON_VALUE;
    data->fill_ratio = (double)data->buffer_size / data->MAX_SIZE;
    if (data->MAX_SIZE >= SIZE && data->fill_ratio < MINIMAL_RATIO) {
        create_new_buffer(data->MAX_SIZE / 2);
    }
#ifdef SUPER_DEBUG
    data->control_hash = hash();
#endif
    return 0;
}

template <typename T, int SIZE>
void Stack<T, SIZE>::clear() {
    __asserter checker(this, __PRETTY_FUNCTION__);
    data->buffer_size = 0;
    data->fill_ratio = 0;
    for (size_t i = 0; i < data->MAX_SIZE; ++i) {
        data->buffer[i] = data->POISON_VALUE;
    }
#ifdef SUPER_DEBUG
    data->control_hash = hash();
#endif
}

template <typename T, int SIZE>
T Stack<T, SIZE>::top() {
    __asserter checker(this, __PRETTY_FUNCTION__);
    if (data->buffer_size == 0) {
        data->err_code = TOP_FAULT;
        return 1;
    }
    return data->buffer[data->buffer_size - 1];
}


#endif //TASK2_SAFESTACKDYNAMIC_HPP