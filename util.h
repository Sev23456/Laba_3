#pragma once

#include "include/ArraySequence.h"
#include "include/DynamicArray.h"
#include "include/ListSequence.h"
#include "include/Sequence.h"

#include <complex>
#include <string>

void print_sequence_kind_menu();
void expect(bool condition, const std::string &name);

const int MUTABLE_ARRAY_SEQUENCE_KIND = 1;
const int IMMUTABLE_ARRAY_SEQUENCE_KIND = 2;
const int MUTABLE_LIST_SEQUENCE_KIND = 3;
const int IMMUTABLE_LIST_SEQUENCE_KIND = 4;
const int VECTOR_KIND = 5;
const int MUTABLE_MATRIX_KIND = 6;
const int IMMUTABLE_MATRIX_KIND = 7;
const int MUTABLE_TRIANGULAR_MATRIX_KIND = 8;
const int IMMUTABLE_TRIANGULAR_MATRIX_KIND = 9;

int square(const int &value);
bool is_even(const int &value);
int sum_values(const int &left, const int &right);
int sum(const int &left, const int &right);

bool equal_value(const int &actual, const int &expected);
bool equal_value(const double &actual, const double &expected);
bool equal_value(const std::complex<double> &actual, const std::complex<double> &expected);
bool equal_value(const std::complex<int> &actual, const std::complex<int> &expected);

template <class T>
void expect_eq(const T &actual, const T &expected, const std::string &name) {
    expect(equal_value(actual, expected), name);
}

int read_int(const std::string &prompt);
int read_non_negative_int(const std::string &prompt);
int read_sequence_kind();
DynamicArray<int> read_values();

Sequence<int> *create_sequence(int kind, const DynamicArray<int> &values);
void reset_sequence(Sequence<int> *&sequence, int &kind);
void replace_sequence(Sequence<int> *&sequence, Sequence<int> *result);
void append_value(Sequence<int> *&sequence);
void prepend_value(Sequence<int> *&sequence);
void insert_value(Sequence<int> *&sequence);
void remove_value(Sequence<int> *&sequence);
void concat_sequence(Sequence<int> *&sequence);
