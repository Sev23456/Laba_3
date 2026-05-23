#include <cmath>
#include <complex>
#include <iostream>
#include <stdexcept>
#include <string>

#include "include/Matrix.h"

namespace {

int passed = 0;
int failed = 0;

void expect(bool condition, const std::string &name) {
    if (condition) {
        passed++;
        std::cout << "[PASS] " << name << '\n';
    } else {
        failed++;
        std::cout << "[FAIL] " << name << '\n';
    }
}

template <class T>
bool values_equal(const T &actual, const T &expected) {
    return actual == expected;
}

bool values_equal(const double &actual, const double &expected) {
    return std::fabs(actual - expected) < 1e-9;
}

bool values_equal(const std::complex<double> &actual, const std::complex<double> &expected) {
    return std::abs(actual - expected) < 1e-9;
}

template <class T>
void expect_eq(const T &actual, const T &expected, const std::string &name) {
    expect(values_equal(actual, expected), name);
}

void expect_close(double actual, double expected, const std::string &name) {
    expect(std::fabs(actual - expected) < 1e-9, name);
}

template <class Function>
void expect_throw(Function function, const std::string &name) {
    try {
        function();
        failed++;
        std::cout << "[FAIL] " << name << " expected exception\n";
    } catch (const std::exception &) {
        passed++;
        std::cout << "[PASS] " << name << '\n';
    }
}

template <class T>
void expect_matrix_eq(const Matrix<T> &matrix, const T *expected, int rows, int columns, const std::string &name) {
    expect_eq(matrix.get_rows(), rows, name + " rows");
    expect_eq(matrix.get_columns(), columns, name + " columns");

    for (int row = 0; row < rows; row++) {
        for (int column = 0; column < columns; column++) {
            int index = row * columns + column;
            expect_eq(matrix.get_el(row, column), expected[index],
                      name + " item [" + std::to_string(row) + "][" + std::to_string(column) + "]");
        }
    }
}

void test_int_matrix_operations() {
    int raw[] = {1, 2, 3, 4};
    MutableMatrix<int> left(raw, 2, 2);
    MutableMatrix<int> right(raw, 2, 2);

    Matrix<int> *sum = left + right;
    int expected_sum[] = {2, 4, 6, 8};
    expect_matrix_eq(*sum, expected_sum, 2, 2, "int matrix sum");
    delete sum;

    Matrix<int> *scaled = left * 3;
    int expected_scaled[] = {3, 6, 9, 12};
    expect_matrix_eq(*scaled, expected_scaled, 2, 2, "int matrix scalar multiply");
    delete scaled;

    Matrix<int> *product = left * right;
    int expected_product[] = {7, 10, 15, 22};
    expect_matrix_eq(*product, expected_product, 2, 2, "int matrix product");
    delete product;

    expect_close(left.count_norm(), std::sqrt(30.0), "int matrix norm");
}

void test_mutable_matrix_updates() {
    int square_raw[] = {1, 2, 3, 4};
    MutableMatrix<int> matrix(square_raw, 2, 2);

    Matrix<int> *changed = matrix.set_el(0, 1, 20);
    int expected_changed[] = {1, 20, 3, 4};
    expect(changed == &matrix, "MutableMatrix set_el returns this");
    expect_matrix_eq(matrix, expected_changed, 2, 2, "MutableMatrix set_el changes source");

    int rows_raw[] = {1, 2, 3, 4, 5, 6};
    MutableMatrix<int> rows_matrix(rows_raw, 2, 3);

    rows_matrix.swap_rows(0, 1);
    int expected_swap_rows[] = {4, 5, 6, 1, 2, 3};
    expect_matrix_eq(rows_matrix, expected_swap_rows, 2, 3, "MutableMatrix swap_rows");

    rows_matrix.multiply_row(1, 2);
    int expected_multiply_row[] = {4, 5, 6, 2, 4, 6};
    expect_matrix_eq(rows_matrix, expected_multiply_row, 2, 3, "MutableMatrix multiply_row");

    rows_matrix.add_row(0, 1, 3);
    int expected_add_row[] = {10, 17, 24, 2, 4, 6};
    expect_matrix_eq(rows_matrix, expected_add_row, 2, 3, "MutableMatrix add_row");

    int columns_raw[] = {1, 2, 3, 4, 5, 6};
    MutableMatrix<int> columns_matrix(columns_raw, 2, 3);

    columns_matrix.swap_columns(0, 2);
    int expected_swap_columns[] = {3, 2, 1, 6, 5, 4};
    expect_matrix_eq(columns_matrix, expected_swap_columns, 2, 3, "MutableMatrix swap_columns");

    columns_matrix.multiply_column(1, 2);
    int expected_multiply_column[] = {3, 4, 1, 6, 10, 4};
    expect_matrix_eq(columns_matrix, expected_multiply_column, 2, 3, "MutableMatrix multiply_column");

    columns_matrix.add_scaled_column(2, 0, 3);
    int expected_add_column[] = {3, 4, 10, 6, 10, 22};
    expect_matrix_eq(columns_matrix, expected_add_column, 2, 3, "MutableMatrix add_scaled_column");
}

void test_immutable_matrix_updates() {
    int raw[] = {1, 2, 3, 4};
    ImmutableMatrix<int> matrix(raw, 2, 2);

    Matrix<int> *changed = matrix.set_el(0, 0, 99);
    int expected_original[] = {1, 2, 3, 4};
    int expected_changed[] = {99, 2, 3, 4};

    expect(changed != static_cast<Matrix<int> *>(&matrix), "ImmutableMatrix set_el returns new matrix");
    expect_matrix_eq(matrix, expected_original, 2, 2, "ImmutableMatrix original after set_el");
    expect_matrix_eq(*changed, expected_changed, 2, 2, "ImmutableMatrix set_el result");
    delete changed;

    Matrix<int> *swapped = matrix.swap_rows(0, 1);
    int expected_swapped[] = {3, 4, 1, 2};
    expect(swapped != static_cast<Matrix<int> *>(&matrix), "ImmutableMatrix swap_rows returns new matrix");
    expect_matrix_eq(matrix, expected_original, 2, 2, "ImmutableMatrix original after swap_rows");
    expect_matrix_eq(*swapped, expected_swapped, 2, 2, "ImmutableMatrix swap_rows result");
    delete swapped;
}

void test_rectangular_product_and_exceptions() {
    int left_raw[] = {1, 2, 3, 4, 5, 6};
    int right_raw[] = {7, 8, 9, 10, 11, 12};

    MutableMatrix<int> left(left_raw, 2, 3);
    MutableMatrix<int> right(right_raw, 3, 2);

    Matrix<int> *product = left * right;
    int expected_product[] = {58, 64, 139, 154};
    expect_matrix_eq(*product, expected_product, 2, 2, "rectangular matrix product");
    delete product;

    int square_raw[] = {1, 2, 3, 4};
    MutableMatrix<int> square(square_raw, 2, 2);

    expect_throw([&]() {
        Matrix<int> *result = left + square;
        delete result;
    }, "matrix sum size mismatch throws");

    expect_throw([&]() {
        Matrix<int> *result = left * left;
        delete result;
    }, "matrix product size mismatch throws");

    expect_throw([&]() {
        left.get_el(-1, 0);
    }, "matrix get negative row throws");

    expect_throw([&]() {
        left.set_el(0, 10, 0);
    }, "matrix set invalid column throws");

    expect_throw([&]() {
        left.swap_rows(0, 10);
    }, "matrix swap_rows invalid row throws");

    expect_throw([&]() {
        left.multiply_column(10, 2);
    }, "matrix multiply_column invalid column throws");

    expect_throw([&]() {
        MutableMatrix<int> invalid(nullptr, 2, 2);
    }, "matrix null items throws");

    expect_throw([&]() {
        MutableMatrix<int> invalid(square_raw, -1, 2);
    }, "matrix negative size throws");
}

void test_double_and_complex_matrix() {
    double double_raw[] = {1.5, -2.0, 0.5, 4.0};
    MutableMatrix<double> double_matrix(double_raw, 2, 2);

    Matrix<double> *scaled = double_matrix * 0.5;
    double expected_scaled[] = {0.75, -1.0, 0.25, 2.0};
    expect_matrix_eq(*scaled, expected_scaled, 2, 2, "double matrix scalar multiply");
    delete scaled;

    expect_close(double_matrix.count_norm(), std::sqrt(22.5), "double matrix norm");

    using Complex = std::complex<double>;
    Complex complex_raw[] = {Complex(1.0, 2.0), Complex(3.0, -1.0)};
    MutableMatrix<Complex> complex_matrix(complex_raw, 1, 2);

    Matrix<Complex> *sum = complex_matrix + complex_matrix;
    Complex expected_sum[] = {Complex(2.0, 4.0), Complex(6.0, -2.0)};
    expect_matrix_eq(*sum, expected_sum, 1, 2, "complex matrix sum");
    delete sum;

    expect_close(complex_matrix.count_norm(), std::sqrt(15.0), "complex matrix norm");
}

} // namespace

int main() {
    test_int_matrix_operations();
    test_mutable_matrix_updates();
    test_immutable_matrix_updates();
    test_rectangular_product_and_exceptions();
    test_double_and_complex_matrix();

    std::cout << "\nPassed: " << passed << "\nFailed: " << failed << '\n';
    if (failed == 0) {
        return 0;
    }
    return 1;
}
