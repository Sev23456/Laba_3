#include <stdexcept>

#include "print.h"

int passed = 0;
int failed = 0;

void test_int_vector() {
    int left_raw[] = {1, 2, 3};
    int right_raw[] = {4, 5, 6};
    Vector<int> left(left_raw, 3);
    Vector<int> right(right_raw, 3);

    Vector<int> *sum = left + right;
    int expected_sum[] = {5, 7, 9};
    expect_vector_eq(*sum, expected_sum, 3, "int vector sum");
    delete sum;

    int expected_original[] = {1, 2, 3};
    expect_vector_eq(left, expected_original, 3, "int vector sum keeps source");

    Vector<int> plus_equal(left_raw, 3);
    Vector<int> *plus_equal_result = plus_equal += right;
    expect(plus_equal_result == &plus_equal, "Vector operator+= returns this");
    expect_vector_eq(plus_equal, expected_sum, 3, "Vector operator+= modifies source");

    Vector<int> *scaled = left * 3;
    int expected_scaled[] = {3, 6, 9};
    expect_vector_eq(*scaled, expected_scaled, 3, "int vector scalar multiply");
    delete scaled;

    expect_vector_eq(left, expected_original, 3, "int vector scalar multiply keeps source");

    Vector<int> multiply_equal(left_raw, 3);
    Vector<int> *multiply_equal_result = multiply_equal *= 3;
    expect(multiply_equal_result == &multiply_equal, "Vector operator*= returns this");
    expect_vector_eq(multiply_equal, expected_scaled, 3, "Vector operator*= modifies source");

    expect_eq(left * right, 32, "int vector scalar product");

    int norm_raw[] = {1, 2, 2};
    Vector<int> norm_vector(norm_raw, 3);
    expect_eq(norm_vector.count_norm(norm_vector), 3, "int vector norm");

    Vector<int> *changed = left.set_component(1, 20);
    int expected_changed[] = {1, 20, 3};
    expect(changed == &left, "Vector set_component returns this");
    expect_vector_eq(left, expected_changed, 3, "Vector set_component changes source");

    int short_raw[] = {1, 2};
    Vector<int> short_vector(short_raw, 2);

    bool sum_thrown = false;
    try {
        Vector<int> *result = left + short_vector;
        delete result;
    } catch (const std::exception &) {
        sum_thrown = true;
    }
    expect(sum_thrown, "int vector sum dimension mismatch throws");

    bool set_thrown = false;
    try {
        left.set_component(-1, 0);
    } catch (const std::exception &) {
        set_thrown = true;
    }
    expect(set_thrown, "int vector set_component negative index throws");
}

void test_double_vector() {
    double left_raw[] = {1.5, 2.0};
    double right_raw[] = {2.5, -1.0};
    Vector<double> left(left_raw, 2);
    Vector<double> right(right_raw, 2);

    Vector<double> *sum = left + right;
    double expected_sum[] = {4.0, 1.0};
    expect_vector_eq(*sum, expected_sum, 2, "double vector sum");
    delete sum;

    Vector<double> *scaled = left * 0.5;
    double expected_scaled[] = {0.75, 1.0};
    expect_vector_eq(*scaled, expected_scaled, 2, "double vector scalar multiply");
    delete scaled;

    Vector<double> plus_equal(left_raw, 2);
    plus_equal += right;
    expect_vector_eq(plus_equal, expected_sum, 2, "double vector operator+=");

    expect_eq(left * right, 1.75, "double vector scalar product");
    expect_eq(left.count_norm(left), 2.5, "double vector norm");
}

void test_complex_vector() {
    std::complex<double> left_raw[] = {std::complex<double>(1.0, 2.0), std::complex<double>(3.0, -1.0)};
    std::complex<double> right_raw[] = {std::complex<double>(2.0, 0.0), std::complex<double>(-1.0, 4.0)};
    Vector<std::complex<double>> left(left_raw, 2);
    Vector<std::complex<double>> right(right_raw, 2);

    Vector<std::complex<double>> *sum = left + right;
    std::complex<double> expected_sum[] = {std::complex<double>(3.0, 2.0), std::complex<double>(2.0, 3.0)};
    expect_vector_eq(*sum, expected_sum, 2, "complex vector sum");
    delete sum;

    Vector<std::complex<double>> *scaled = left * std::complex<double>(0.0, 1.0);
    std::complex<double> expected_scaled[] = {std::complex<double>(-2.0, 1.0), std::complex<double>(1.0, 3.0)};
    expect_vector_eq(*scaled, expected_scaled, 2, "complex vector scalar multiply");
    delete scaled;

    expect_eq(left * right, std::complex<double>(3.0, 17.0), "complex vector scalar product");
}

void test_complex_int_vector() {
    std::complex<int> left_raw[] = {std::complex<int>(1, 2), std::complex<int>(3, -1)};
    std::complex<int> right_raw[] = {std::complex<int>(2, 0), std::complex<int>(-1, 4)};
    Vector<std::complex<int>> left(left_raw, 2);
    Vector<std::complex<int>> right(right_raw, 2);

    Vector<std::complex<int>> *sum = left + right;
    std::complex<int> expected_sum[] = {std::complex<int>(3, 2), std::complex<int>(2, 3)};
    expect_vector_eq(*sum, expected_sum, 2, "complex<int> vector sum");
    delete sum;

    Vector<std::complex<int>> *scaled = left * std::complex<int>(0, 1);
    std::complex<int> expected_scaled[] = {std::complex<int>(-2, 1), std::complex<int>(1, 3)};
    expect_vector_eq(*scaled, expected_scaled, 2, "complex<int> vector scalar multiply");
    delete scaled;

    expect_eq(left * right, std::complex<int>(3, 17), "complex<int> vector scalar product");
}

int main() {
    test_int_vector();
    test_double_vector();
    test_complex_vector();
    test_complex_int_vector();

    std::cout << "\nPassed: " << passed << "\nFailed: " << failed << '\n';
    if (failed == 0) {
        return 0;
    }
    return 1;
}
