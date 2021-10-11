#include "text_proc.h"

#include <iostream>

long long get_file_size(FILE* file) {
    assert(file);

    fseek(file, 0, SEEK_END);
    long long SIZE = ftell(file);
    fseek(file, 0, SEEK_SET);
    return SIZE;
}

//! \brief Функция считывает содержания файла и сохраняет его по данному указалетелю, возвращая размер файла.
//! \param [in] file_name Имя исходного файла.
//! \param [out] text_destination Указатель на память, выделенную под содержимое файла.
//! \return Размер считанного файла.
long long read_input(const char *file_name, char*& text_destination) {
    assert(file_name);

    FILE* input = fopen(file_name, "r"); assert(input);
    long long SIZE = get_file_size(input);

    text_destination = (char*)(calloc(SIZE, 1)); assert(text_destination);
    fread(text_destination, 1, SIZE, input); assert(ferror(input) == 0);

    fclose(input);
    return SIZE;
}


int separate_strings(char* text, long long SIZE, string_view*& string_array_destination) {
    assert(text); assert(SIZE >= 0);
    int line_number = 0;
    for (long long i = 0; i < SIZE; ++i) {
        if (text[i] == '\n' || text[i] == ' ') {
            text[i] = '\0';
            ++line_number;
        }
    }

    string_array_destination = (string_view*)(calloc(line_number, sizeof(string_view)));

    int text_ptr = 0;
    for (int i = 0; i < line_number; ++i) {
        string_array_destination[i].len = strlen(&text[text_ptr]);
        string_array_destination[i].ptr = &text[text_ptr];
        text_ptr += string_array_destination[i].len + 1;
    }
    return line_number;
}

//! \brief Функция разбивает текст на строки.
//! \details Функция разбивает текст на строки, заменяя каждый \n в исходном тексте на \0.
//! \param [in] text Указатель на исходный текст.
//! \param [in] SIZE Размер исходного текста.
//! \param [out] string_array_destination Указатель на память, в которую складываются строки.
//! \return Количество строк.
int separate_by_lines(char* text, long long SIZE, string_view*& string_array_destination) {
    assert(text); assert(SIZE >= 0);
    int line_number = 0;
    for (long long i = 0; i < SIZE; ++i) {
        if (text[i] == '\n') {
            text[i] = '\0';
            ++line_number;
        }
    }

    string_array_destination = (string_view*)(calloc(line_number, sizeof(string_view)));

    int text_ptr = 0;
    for (int i = 0; i < line_number; ++i) {
        string_array_destination[i].len = strlen(&text[text_ptr]);
        string_array_destination[i].ptr = &text[text_ptr];
        text_ptr += string_array_destination[i].len + 1;
    }
    return line_number;
}

int separate_by_words(char* text, long long SIZE, string_view*& string_array_destination) {
    assert(text); assert(SIZE >= 0);
    bool beginning = true;
    int word_number = 0;
    bool between_space = false;
    for (long long i = 0; i < SIZE; ++i) {
        while (beginning) {
            if (isspace(text[i])) {
                ++i;
            } else {
                beginning = false;
            }
        }
        between_space = true;
        if (text[i] == '\n') {
            text[i] = '\0';
            ++word_number;
            between_space = false;
            beginning = true;
        }
        if (isspace(text[i])) {
            text[i] = '\0';
            ++word_number;
        }
    }

    string_array_destination = (string_view*)(calloc(word_number, sizeof(string_view)));

    int text_ptr = 0;
    for (int i = 0; i < word_number; ++i) {
        while (isspace(text[text_ptr])) {
            ++text_ptr;
        }
        string_array_destination[i].len = strlen(&text[text_ptr]);
        string_array_destination[i].ptr = &text[text_ptr];
        text_ptr += string_array_destination[i].len + 1;
    }
    return word_number;
}

//! \brief Функция выводит в данный файл отсортированный в разном порядке массив строк.
//! \details Функция выводит в данный файл отсортированный в разном порядке массив строк. \
//!          Сначала сортировка по первому символу, затем по последнему, а в конце исходный текст. \
//!          Все символы \0 в исходном тексте заменяются на \n.
//! \param [in] FILE_NAME Имя выходного файла.
//! \param [in] string_array Указатель на массив строк.
//! \param [in] string_array_size Размер массива строк.
//! \param [in] text Указатель на исходный текст.
//! \param [in] SIZE Размер исходного текста.
void write_output(const char *FILE_NAME, string_view *&string_array,
                  int string_array_size, char *&text, long long SIZE) {

    assert(FILE_NAME); assert(string_array); assert(string_array_size >= 0); assert(text); assert(SIZE >= 0);
    FILE *output = fopen(FILE_NAME, "w");
    from_start start;
    from_end end;
    std::sort(string_array, string_array + string_array_size, start);
    for (int i = 0; i < string_array_size; ++i) {
        fputs(string_array[i].ptr, output);
        fputs("\n", output);
    }

    std::sort(string_array, string_array + string_array_size, end);
    for (int i = 0; i < string_array_size; ++i) {
        fputs(string_array[i].ptr, output);
        fputs("\n", output);
    }

    for (long long i = 0; i < SIZE; ++i) {
        if (text[i] == '\0') {
            text[i] = '\n';
        }
        fprintf(output, "%c", text[i]);
    }
    fclose(output);
}
