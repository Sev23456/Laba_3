#include "util.h"

#include "print.h"

#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>

int square(const int &value) {
    return value * value;
}

bool is_even(const int &value) {
    return value % 2 == 0;
}

int sum_values(const int &left, const int &right) {
    return left + right;
}

int sum(const int &left, const int &right) {
    return sum_values(left, right);
}

bool equal_value(const int &actual, const int &expected) {
    return actual == expected;
}

bool equal_value(const double &actual, const double &expected) {
    return std::fabs(actual - expected) < 1e-9;
}

bool equal_value(const std::complex<double> &actual, const std::complex<double> &expected) {
    return std::abs(actual - expected) < 1e-9;
}

bool equal_value(const std::complex<int> &actual, const std::complex<int> &expected) {
    return actual == expected;
}

int read_int(const std::string &prompt) {
    int value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }

        std::cout << "Invalid integer. Try again.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

int read_non_negative_int(const std::string &prompt) {
    int value = read_int(prompt);
    while (value < 0) {
        std::cout << "Value must be non-negative.\n";
        value = read_int(prompt);
    }
    return value;
}

int read_sequence_kind() {
    print_sequence_kind_menu();
    int kind = read_int("Type: ");
    while (kind < MUTABLE_ARRAY_SEQUENCE_KIND || kind > IMMUTABLE_LIST_SEQUENCE_KIND) {
        std::cout << "Unknown type. Choose 1..4.\n";
        kind = read_int("Type: ");
    }
    return kind;
}

DynamicArray<int> read_values() {
    int count = read_non_negative_int("Element count: ");
    DynamicArray<int> values(count);

    for (int index = 0; index < count; index++) {
        values.set(index, read_int("Element " + std::to_string(index) + ": "));
    }

    return values;
}

Sequence<int> *create_sequence(int kind, const DynamicArray<int> &values) {
    Sequence<int> *sequence = nullptr;

    switch (kind) {
        case MUTABLE_ARRAY_SEQUENCE_KIND:
            sequence = new MutableArraySequence<int>();
            break;
        case IMMUTABLE_ARRAY_SEQUENCE_KIND:
            sequence = new ImmutableArraySequence<int>();
            break;
        case MUTABLE_LIST_SEQUENCE_KIND:
            sequence = new MutableListSequence<int>();
            break;
        case IMMUTABLE_LIST_SEQUENCE_KIND:
            sequence = new ImmutableListSequence<int>();
            break;
        default:
            throw std::invalid_argument("Unknown sequence type");
    }

    for (int index = 0; index < values.get_size(); index++) {
        Sequence<int> *next = sequence->append(values.get(index));
        if (next != sequence) {
            delete sequence;
            sequence = next;
        }
    }

    return sequence;
}

void reset_sequence(Sequence<int> *&sequence, int &kind) {
    int new_kind = read_sequence_kind();
    DynamicArray<int> values = read_values();
    Sequence<int> *replacement = create_sequence(new_kind, values);

    delete sequence;
    sequence = replacement;
    kind = new_kind;
}

void replace_sequence(Sequence<int> *&sequence, Sequence<int> *result) {
    if (result == nullptr) {
        throw std::runtime_error("Operation returned nullptr");
    }

    if (result != sequence) {
        delete sequence;
        sequence = result;
    }
}

void append_value(Sequence<int> *&sequence) {
    int value = read_int("Value: ");
    replace_sequence(sequence, sequence->append(value));
}

void prepend_value(Sequence<int> *&sequence) {
    int value = read_int("Value: ");
    replace_sequence(sequence, sequence->prepend(value));
}

void insert_value(Sequence<int> *&sequence) {
    int value = read_int("Value: ");
    int index = read_int("Index: ");
    replace_sequence(sequence, sequence->insert(value, index));
}

void remove_value(Sequence<int> *&sequence) {
    int index = read_int("Index: ");
    replace_sequence(sequence, sequence->remove(index));
}

void concat_sequence(Sequence<int> *&sequence) {
    std::cout << "Enter sequence to append.\n";
    DynamicArray<int> values = read_values();
    Sequence<int> *other = create_sequence(1, values);

    try {
        Sequence<int> *result = sequence->concat(*other);
        replace_sequence(sequence, result);
        delete other;
    } catch (...) {
        delete other;
        throw;
    }
}
