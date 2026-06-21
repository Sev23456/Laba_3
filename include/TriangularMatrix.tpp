#pragma once

#include "TriangularMatrix.h"

#include <stdexcept>

template <typename T> TriangularMatrix<T>::TriangularMatrix(bool is_lower)
    : Matrix<T>(), lower(is_lower) {}

template <typename T> TriangularMatrix<T>::TriangularMatrix(const T *source, int matrix_size, bool is_lower)
    : Matrix<T>(), lower(is_lower) { // тут мы "укладываем" данные 
    if (matrix_size < 0) {
        throw std::invalid_argument("Triangular matrix size cannot be negative");
    }

    int count = matrix_size * matrix_size;
    if (count > 0 && source == nullptr) {
        throw std::invalid_argument("Items cannot be null");
    }

    this->rows = matrix_size;
    this->columns = matrix_size;

    for (int row = 0; row < matrix_size; row++) {
        for (int column = 0; column < matrix_size; column++) {
            int index = row * matrix_size + column;
            if (is_allowed_cell(row, column)) {
                this->append_to_current(source[index]);
            } else {
                this->append_to_current(T{}); // Добавление 0, но в типе T
                // Краткая справка: В данной реализации треугольная матрица -
                // надстройка над обычной, где мы тупо ограничили добавление данных
                // по правилам треугольной матрицы. Там где данных быть не должно
                // Мы ставим 0. Это не самая экономная реализация, но она понятная.
                // плюс в линале когда мы строим треугольную матрицу, то в местах
                // Где нет элементов мы ставим 0. То есть мы храним её "как есть"
                // В данный момент а не компактно. Но если хранить её копактно, то
                // Для выводов придётся дополнительно это учитывать
            }
        }
    }
}

// Конструктор на основе обычной матрицы (Если передать не треугольный Matrix, то лишние элементы обрежутся)

template <typename T> TriangularMatrix<T>::TriangularMatrix(const Matrix<T> &source, bool is_lower)
    : Matrix<T>(), lower(is_lower) {
    if (source.get_rows() != source.get_columns()) {
        throw std::invalid_argument("Triangular matrix must be square");
    }

    this->rows = source.get_rows();
    this->columns = source.get_columns();

    for (int row = 0; row < this->rows; row++) {
        for (int column = 0; column < this->columns; column++) {
            if (is_allowed_cell(row, column)) {
                this->append_to_current(source.get_el(row, column));
            } else {
                this->append_to_current(T{});
            }
        }
    }
}

// Конструктор

template <typename T> TriangularMatrix<T>::TriangularMatrix(const TriangularMatrix<T> &other)
    : Matrix<T>(other), lower(other.lower) {}

// =

template <typename T>
TriangularMatrix<T> &TriangularMatrix<T>::operator=(const TriangularMatrix<T> &other) {
    if (this == &other) {
        return *this;
    }

    this->items = other.items;
    this->rows = other.rows;
    this->columns = other.columns;
    lower = other.lower;
    return *this;
}

// получить размер (можем взять и не rows, а columns, так как матрица квадратная)
// Я выбрал rows

template <typename T> int TriangularMatrix<T>::get_size() const {
    return this->rows;
}

template <typename T> bool TriangularMatrix<T>::is_lower() const {
    return lower;
}

// Функция проверки допустимости элемента

template <typename T> bool TriangularMatrix<T>::is_allowed_cell(int row, int column) const {
    if (lower) {
        return row >= column;
    }

    return column >= row;
}

template <typename T> TriangularMatrix<T> *TriangularMatrix<T>::set_el(int row, int column, const T &value) {
    if (row < 0 || row >= this->rows) {
        throw std::out_of_range("Row index out of range");
    }
    if (column < 0 || column >= this->columns) {
        throw std::out_of_range("Column index out of range");
    }
    if (!is_allowed_cell(row, column)) {
        throw std::invalid_argument("Cannot set value outside triangle");
    }

    TriangularMatrix<T> *result = this->Instance();
    result->set_to_current(row, column, value);
    return result;
}

// +=

template <typename T> TriangularMatrix<T> *TriangularMatrix<T>::add_to_self(const TriangularMatrix<T> &other) {
    if (this->rows != other.rows || this->columns != other.columns || lower != other.lower) {
        throw std::invalid_argument("Triangular matrices have incompatible shapes");
    }

    TriangularMatrix<T> *result = this->Instance();

    for (int row = 0; row < this->rows; row++) {
        for (int column = 0; column < this->columns; column++) {
            if (is_allowed_cell(row, column)) {
                T value = result->get_el(row, column) + other.get_el(row, column);
                result->set_to_current(row, column, value);
            } else {
                result->set_to_current(row, column, T{});
            }
        }
    }

    return result;
}

template <typename T> TriangularMatrix<T> *TriangularMatrix<T>::scalar_product(const T &scalar) {
    TriangularMatrix<T> *result = this->Instance();

    for (int row = 0; row < this->rows; row++) {
        for (int column = 0; column < this->columns; column++) {
            if (is_allowed_cell(row, column)) {
                T value = result->get_el(row, column) * scalar;
                result->set_to_current(row, column, value);
            } else {
                result->set_to_current(row, column, T{});
            }
        }
    }

    return result;
}

// Перегрузки операторов-обёртки

template <typename T> TriangularMatrix<T> *TriangularMatrix<T>::operator+=(const TriangularMatrix<T> &other) {
    return add_to_self(other);
}

template <typename T> TriangularMatrix<T> *TriangularMatrix<T>::operator*=(const T &scalar) {
    return scalar_product(scalar);
}

template <typename T> TriangularMatrix<T> *TriangularMatrix<T>::operator+(const TriangularMatrix<T> &other) const {
    TriangularMatrix<T> *result = this->Clone();
    TriangularMatrix<T> *updated = nullptr;

    try {
        updated = (*result += other);
    } catch (...) { // Ловим любое исключение
        delete result;
        throw;
    }

    if (updated != result) {
        delete result;
    }

    return updated;
}

// Работает аналогично верхнему

template <typename T> TriangularMatrix<T> *TriangularMatrix<T>::operator*(const T &scalar) const {
    TriangularMatrix<T> *result = this->Clone();
    TriangularMatrix<T> *updated = nullptr;

    try {
        updated = (*result *= scalar);
    } catch (...) {
        delete result;
        throw;
    }

    if (updated != result) {
        delete result;
    }

    return updated;
}

// Функции заглушки, чтобы не вызывать deprecated методы

template <typename T> Matrix<T> *TriangularMatrix<T>::operator+=(const Matrix<T> &) {
    throw std::logic_error("Use operator+= with TriangularMatrix");
}

template <typename T> Matrix<T> *TriangularMatrix<T>::operator*=(const Matrix<T> &) {
    throw std::logic_error("Matrix product is disabled for TriangularMatrix");
}

template <typename T> Matrix<T> *TriangularMatrix<T>::operator+(const Matrix<T> &) const {
    throw std::logic_error("Use operator+ with TriangularMatrix");
}

template <typename T> Matrix<T> *TriangularMatrix<T>::operator*(const Matrix<T> &) const {
    throw std::logic_error("Matrix product is disabled for TriangularMatrix");
}

template <typename T> Matrix<T> *TriangularMatrix<T>::swap_rows(int, int) {
    throw std::logic_error("Row operations are disabled for TriangularMatrix");
}

template <typename T> Matrix<T> *TriangularMatrix<T>::multiply_row(int, const T &) {
    throw std::logic_error("Row operations are disabled for TriangularMatrix");
}

template <typename T> Matrix<T> *TriangularMatrix<T>::add_row(int, int, const T &) {
    throw std::logic_error("Row operations are disabled for TriangularMatrix");
}

template <typename T> Matrix<T> *TriangularMatrix<T>::swap_columns(int, int) {
    throw std::logic_error("Column operations are disabled for TriangularMatrix");
}

template <typename T> Matrix<T> *TriangularMatrix<T>::multiply_column(int, const T &) {
    throw std::logic_error("Column operations are disabled for TriangularMatrix");
}

template <typename T> Matrix<T> *TriangularMatrix<T>::add_scaled_column(int, int, const T &) {
    throw std::logic_error("Column operations are disabled for TriangularMatrix");
}

template <typename T> Matrix<T> *TriangularMatrix<T>::add_to_self(const Matrix<T> &) {
    throw std::logic_error("Use add_to_self with TriangularMatrix");
}

template <typename T> Matrix<T> *TriangularMatrix<T>::matrix_product(const Matrix<T> &) {
    throw std::logic_error("Matrix product is disabled for TriangularMatrix");
}
