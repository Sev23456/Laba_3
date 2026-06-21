#pragma once

#include "Matrix.h"

template <typename T> class TriangularMatrix : public Matrix<T> {
    protected:
        virtual TriangularMatrix<T> *Instance() override = 0; // Изменяем значение с Matrix на TriangularMatrix
        virtual TriangularMatrix<T> *Clone() const override = 0;

        bool lower; // Отвечает за то, будет ли у нас нижнетреугольная или верхнетреугольная матрица

    public:
        TriangularMatrix(bool is_lower = true);
        TriangularMatrix(const T *source, int matrix_size, bool is_lower = true);
        TriangularMatrix(const Matrix<T> &source, bool is_lower = true);
        TriangularMatrix(const TriangularMatrix<T> &other);

        TriangularMatrix<T> &operator=(const TriangularMatrix<T> &other);
        virtual TriangularMatrix<T> *Empty() const override = 0;

        int get_size() const;
        bool is_lower() const; // нужно чтобы снаружи узнать тип матрицы
        // Почему не сделать lower публиным - защита от дурака, нельзя просто поменять этот параметр
        // Не переставив элементов - ручное изменение lower в текущем сценарии приведёт
        // К непредсказуемому и неправильному поведению программы

        bool is_allowed_cell(int row, int column) const; // Проверяем, можно ли вставить элемент
        // в клетку. Почему в public? пользователь может сам захотеть узнать эту информацию
        // поэтому она не должна быть закрытой

        TriangularMatrix<T> *set_el(int row, int column, const T &value);
        TriangularMatrix<T> *add_to_self(const TriangularMatrix<T> &other); // += тупо
        TriangularMatrix<T> *scalar_product(const T &scalar);

        TriangularMatrix<T> *operator+=(const TriangularMatrix<T> &other);
        TriangularMatrix<T> *operator*=(const T &scalar);
        TriangularMatrix<T> *operator+(const TriangularMatrix<T> &other) const;
        TriangularMatrix<T> *operator*(const T &scalar) const;

        // deprecated

        Matrix<T> *operator+=(const Matrix<T> &other);
        Matrix<T> *operator*=(const Matrix<T> &other);
        Matrix<T> *operator+(const Matrix<T> &other) const;
        Matrix<T> *operator*(const Matrix<T> &other) const;

        Matrix<T> *swap_rows(int first, int second);
        Matrix<T> *multiply_row(int row, const T &scalar);
        Matrix<T> *add_row(int target, int source, const T &scalar);

        Matrix<T> *swap_columns(int first, int second);
        Matrix<T> *multiply_column(int column, const T &scalar);
        Matrix<T> *add_scaled_column(int target, int source, const T &scalar);

        Matrix<T> *add_to_self(const Matrix<T> &other);
        Matrix<T> *matrix_product(const Matrix<T> &other);
};

template <typename T> class MutableTriangularMatrix : public TriangularMatrix<T> {
    protected:
        TriangularMatrix<T> *Instance() override {
            return this;
        }

        TriangularMatrix<T> *Clone() const override {
            return new MutableTriangularMatrix<T>(*this);
        }

    public:
        TriangularMatrix<T> *Empty() const override {
            return new MutableTriangularMatrix<T>(this->lower);
        }

        MutableTriangularMatrix(bool is_lower = true) : TriangularMatrix<T>(is_lower) {};
        MutableTriangularMatrix(const T *items, int size, bool is_lower = true)
            : TriangularMatrix<T>(items, size, is_lower) {};
        MutableTriangularMatrix(const Matrix<T> &other, bool is_lower = true)
            : TriangularMatrix<T>(other, is_lower) {};
        MutableTriangularMatrix(const TriangularMatrix<T> &other) : TriangularMatrix<T>(other) {};
};

template <typename T> class ImmutableTriangularMatrix : public TriangularMatrix<T> {
    protected:
        TriangularMatrix<T> *Instance() override {
            return Clone();
        }

        TriangularMatrix<T> *Clone() const override {
            return new ImmutableTriangularMatrix<T>(*this);
        }

    public:
        TriangularMatrix<T> *Empty() const override {
            return new ImmutableTriangularMatrix<T>(this->lower);
        }

        ImmutableTriangularMatrix(bool is_lower = true) : TriangularMatrix<T>(is_lower) {};
        ImmutableTriangularMatrix(const T *items, int size, bool is_lower = true)
            : TriangularMatrix<T>(items, size, is_lower) {};
        ImmutableTriangularMatrix(const Matrix<T> &other, bool is_lower = true)
            : TriangularMatrix<T>(other, is_lower) {};
        ImmutableTriangularMatrix(const TriangularMatrix<T> &other) : TriangularMatrix<T>(other) {};
};

#include "TriangularMatrix.tpp"
