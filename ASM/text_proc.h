#ifndef TASK3_TEXT_PROC_H
#define TASK3_TEXT_PROC_H

#include <cstring>
#include <algorithm>
#include <cctype>
#include <cassert>
#include <cstdio>


struct string_view {
    char* ptr;
    int   len;
    string_view(char* ptr, int len) : ptr(ptr), len(len) {}
    string_view() : ptr(nullptr), len(0) {}
};

//! \details Компаратор для строк. Сравнение с последнего символа.
struct from_end {
    bool operator()(const string_view& left, const string_view& right) {
        int left_cnt  = left.len - 1;
        int right_cnt = right.len - 1;
        for (; left_cnt >= 0 && right_cnt >= 0;) {
            while (left_cnt >= 0 && !isalpha(left.ptr[left_cnt])) {
                --left_cnt;
            }
            while (right_cnt >= 0 && !isalpha(right.ptr[right_cnt])) {
                --right_cnt;
            }
            if (left_cnt < 0 || right_cnt < 0) {
                break;
            }
            if (tolower(left.ptr[left_cnt]) != tolower(right.ptr[right_cnt])) {
                return tolower(left.ptr[left_cnt]) < tolower(right.ptr[right_cnt]);
            }
            --left_cnt;
            --right_cnt;
        }
        return left.len < right.len;
    }
};
//! \details Компаратор для строк. Сравнение с первого символа.
struct from_start {
    bool operator()(const string_view& left, const string_view& right) {
        int left_cnt  = 0;
        int right_cnt = 0;
        for (; left_cnt < left.len && right_cnt < right.len;) {
            while (left_cnt < left.len && !isalpha(left.ptr[left_cnt])) {
                ++left_cnt;
            }
            while (right_cnt < right.len && !isalpha(right.ptr[right_cnt])) {
                ++right_cnt;
            }
            if (left_cnt >= left.len || right_cnt >= right.len) {
                break;
            }
            if (tolower(left.ptr[left_cnt]) != tolower(right.ptr[right_cnt])) {
                return tolower(left.ptr[left_cnt]) < tolower(right.ptr[right_cnt]);
            }
            ++left_cnt;
            ++right_cnt;
        }
        return left.len < right.len;
    }
};


long long get_file_size(FILE* file);

//! \brief Функция считывает содержания файла и сохраняет его по данному указалетелю, возвращая размер файла.
//! \param [in] file_name Имя исходного файла.
//! \param [out] text_destination Указатель на память, выделенную под содержимое файла.
//! \return Размер считанного файла.
long long read_input(const char *file_name, char*& text_destination);

//! \brief Функция разбивает текст на строки.
//! \details Функция разбивает текст на строки, заменяя каждый \n в исходном тексте на \0.
//! \param [in] text Указатель на исходный текст.
//! \param [in] SIZE Размер исходного текста.
//! \param [out] string_array_destination Указатель на память, в которую складываются строки.
//! \return Количество строк.
int separate_by_lines(char* text, long long SIZE, string_view*& string_array_destination);

int separate_strings(char* text, long long SIZE, string_view*& string_array_destination);

//! \brief Функция разбивает текст на слова.
//! \details Функция разбивает текст на слова, заменяя каждый \n в исходном тексте на \0 и игнорируя начальные пробелы.
//! \param [in] text Указатель на исходный текст.
//! \param [in] SIZE Размер исходного текста.
//! \param [out] string_array_destination Указатель на память, в которую складываются слова.
//! \return Количество строк.
int separate_by_words(char* text, long long SIZE, string_view*& string_array_destination);
//! \brief Функция выводит в данный файл отсортированный в разном порядке массив строк.
//! \details Функция выводит в данный файл отсортированный в разном порядке массив строк. \
//!          Сначала сортировка по первому символу, затем по последнему, а в конце исходный текст. \
//!          Все символы \0 в исходном тексте заменяются на \n.
//! \param [in] FILE_NAME Имя выходного файла.
//! \param [in] string_array Указатель на массив строк.
//! \param [in] string_array_size Размер массива строк.
//! \param [in] text Указатель на исходный текст.
//! \param [in] SIZE Размер исходного текста.
void write_output(const char *FILE_NAME, string_view *&string_array, int string_array_size, char *&text, long long SIZE);
#endif //DED_TASK1_TEXT_PROC_H

