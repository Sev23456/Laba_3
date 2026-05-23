#include "print.h"
#include "util.h"
#include "include/Matrix.h"
#include "include/Vector.h"

#include <exception>
#include <iostream>

int read_vector_kind() {
    print_vector_kind_menu();
    int kind = read_int("Type: ");
    while (kind != VECTOR_KIND) {
        std::cout << "Unknown vector type. Choose " << VECTOR_KIND << ".\n";
        kind = read_int("Type: ");
    }
    return kind;
}

int read_matrix_kind() {
    print_matrix_kind_menu();
    int kind = read_int("Type: ");
    while (kind < MUTABLE_MATRIX_KIND || kind > IMMUTABLE_MATRIX_KIND) {
        std::cout << "Unknown matrix type. Choose "
                  << MUTABLE_MATRIX_KIND << ".." << IMMUTABLE_MATRIX_KIND << ".\n";
        kind = read_int("Type: ");
    }
    return kind;
}

DynamicArray<int> read_matrix_values(int &rows, int &columns) {
    rows = read_non_negative_int("Rows: ");
    columns = read_non_negative_int("Columns: ");

    DynamicArray<int> values(rows * columns);
    for (int row = 0; row < rows; row++) {
        for (int column = 0; column < columns; column++) {
            values.set(row * columns + column,
                       read_int("Element [" + std::to_string(row) + "][" + std::to_string(column) + "]: "));
        }
    }

    return values;
}

Vector<int> *create_vector(int kind, const DynamicArray<int> &values) {
    int count = values.get_size();
    int *items = nullptr;
    if (count > 0) {
        items = new int[count];
    }

    for (int index = 0; index < count; index++) {
        items[index] = values.get(index);
    }

    try {
        if (kind == VECTOR_KIND) {
            Vector<int> *vector = new Vector<int>(items, count);
            delete[] items;
            return vector;
        }

        throw std::invalid_argument("Unknown vector type");
    } catch (...) {
        delete[] items;
        throw;
    }
}

Matrix<int> *create_matrix(int kind, int rows, int columns, const DynamicArray<int> &values) {
    int count = rows * columns;
    if (values.get_size() != count) {
        throw std::invalid_argument("Matrix value count does not match matrix size");
    }

    int *items = nullptr;
    if (count > 0) {
        items = new int[count];
    }

    for (int index = 0; index < count; index++) {
        items[index] = values.get(index);
    }

    try {
        if (kind == MUTABLE_MATRIX_KIND) {
            Matrix<int> *matrix = new MutableMatrix<int>(items, rows, columns);
            delete[] items;
            return matrix;
        }

        if (kind == IMMUTABLE_MATRIX_KIND) {
            Matrix<int> *matrix = new ImmutableMatrix<int>(items, rows, columns);
            delete[] items;
            return matrix;
        }

        throw std::invalid_argument("Unknown matrix type");
    } catch (...) {
        delete[] items;
        throw;
    }
}

Matrix<int> *read_mutable_matrix() {
    int rows = 0;
    int columns = 0;
    DynamicArray<int> values = read_matrix_values(rows, columns);
    return create_matrix(MUTABLE_MATRIX_KIND, rows, columns, values);
}

void reset_vector(Vector<int> *&vector, int &kind) {
    int new_kind = read_vector_kind();
    DynamicArray<int> values = read_values();
    Vector<int> *replacement = create_vector(new_kind, values);

    delete vector;
    vector = replacement;
    kind = new_kind;
}

void reset_matrix(Matrix<int> *&matrix, int &kind) {
    int new_kind = read_matrix_kind();
    int rows = 0;
    int columns = 0;
    DynamicArray<int> values = read_matrix_values(rows, columns);
    Matrix<int> *replacement = create_matrix(new_kind, rows, columns, values);

    delete matrix;
    matrix = replacement;
    kind = new_kind;
}

void replace_vector(Vector<int> *&vector, Vector<int> *result) {
    if (result == nullptr) {
        throw std::runtime_error("Operation returned nullptr");
    }

    if (result != vector) {
        delete vector;
        vector = result;
    }
}

void replace_matrix(Matrix<int> *&matrix, Matrix<int> *result) {
    if (result == nullptr) {
        throw std::runtime_error("Operation returned nullptr");
    }

    if (result != matrix) {
        delete matrix;
        matrix = result;
    }
}

void execute_sequence_command(Sequence<int> *&sequence, int &kind, int command, bool &running) {
    switch (command) {
        case 0:
            running = false;
            return;
        case 1:
            print_current_sequence(*sequence, kind);
            return;
        case 2:
            std::cout << "First: " << sequence->get_first() << "\n";
            return;
        case 3:
            std::cout << "Last: " << sequence->get_last() << "\n";
            return;
        case 4: {
            int index = read_int("Index: ");
            std::cout << "Value: " << sequence->get(index) << "\n";
            return;
        }
        case 5: {
            int index = read_int("Index: ");
            print_option(sequence->try_get(index));
            return;
        }
        case 6:
            append_value(sequence);
            return;
        case 7:
            prepend_value(sequence);
            return;
        case 8:
            insert_value(sequence);
            return;
        case 9:
            remove_value(sequence);
            return;
        case 10:
            print_subsequence(*sequence);
            return;
        case 11:
            concat_sequence(sequence);
            return;
        case 12:
            print_mapped(*sequence);
            return;
        case 13:
            print_filtered(*sequence);
            return;
        case 14:
            std::cout << "Sum: " << sequence->reduce(sum_values, 0) << "\n";
            return;
        case 15:
            print_with_enumerator(*sequence);
            return;
        case 16:
            reset_sequence(sequence, kind);
            return;
        default:
            std::cout << "Unknown command\n";
            return;
    }
}

void execute_vector_command(Vector<int> *&vector, int &kind, int command, bool &running) {
    switch (command) {
        case 0:
            running = false;
            return;
        case 1:
            print_current_vector(*vector, kind);
            return;
        case 2: {
            int index = read_int("Index: ");
            int value = read_int("Value: ");
            replace_vector(vector, vector->set_component(index, value));
            return;
        }
        case 3: {
            std::cout << "Enter vector to add.\n";
            DynamicArray<int> values = read_values();
            Vector<int> *other = create_vector(VECTOR_KIND, values);
            Vector<int> *result = nullptr;
            try {
                result = *vector + *other;
                std::cout << "Result: ";
                print_vector(*result);
                std::cout << "\n";
                delete result;
                delete other;
            } catch (...) {
                delete result;
                delete other;
                throw;
            }
            return;
        }
        case 4: {
            int scalar = read_int("Scalar: ");
            Vector<int> *result = (*vector) * scalar;
            std::cout << "Result: ";
            print_vector(*result);
            std::cout << "\n";
            delete result;
            return;
        }
        case 5: {
            std::cout << "Enter second vector.\n";
            DynamicArray<int> values = read_values();
            Vector<int> *other = create_vector(VECTOR_KIND, values);
            try {
                std::cout << "Scalar product: " << ((*vector) * (*other)) << "\n";
                delete other;
            } catch (...) {
                delete other;
                throw;
            }
            return;
        }
        case 6:
            std::cout << "Norm: " << vector->count_norm(*vector) << "\n";
            return;
        case 7:
            std::cout << "Enumerator = ";
            print_vector(*vector);
            std::cout << "\n";
            return;
        case 8:
            reset_vector(vector, kind);
            return;
        default:
            std::cout << "Unknown command\n";
            return;
    }
}

void execute_matrix_command(Matrix<int> *&matrix, int &kind, int command, bool &running) {
    switch (command) {
        case 0:
            running = false;
            return;
        case 1:
            print_current_matrix(*matrix, kind);
            return;
        case 2: {
            int row = read_int("Row: ");
            int column = read_int("Column: ");
            std::cout << "Value: " << matrix->get_el(row, column) << "\n";
            return;
        }
        case 3: {
            int row = read_int("Row: ");
            int column = read_int("Column: ");
            int value = read_int("Value: ");
            replace_matrix(matrix, matrix->set_el(row, column, value));
            return;
        }
        case 4: {
            std::cout << "Enter matrix to add.\n";
            Matrix<int> *other = read_mutable_matrix();
            Matrix<int> *result = nullptr;
            try {
                result = *matrix + *other;
                std::cout << "Result: ";
                print_matrix(*result);
                std::cout << "\n";
                delete result;
                delete other;
            } catch (...) {
                delete result;
                delete other;
                throw;
            }
            return;
        }
        case 5: {
            int scalar = read_int("Scalar: ");
            Matrix<int> *result = (*matrix) * scalar;
            std::cout << "Result: ";
            print_matrix(*result);
            std::cout << "\n";
            delete result;
            return;
        }
        case 6: {
            std::cout << "Enter second matrix.\n";
            Matrix<int> *other = read_mutable_matrix();
            Matrix<int> *result = nullptr;
            try {
                result = (*matrix) * (*other);
                std::cout << "Result: ";
                print_matrix(*result);
                std::cout << "\n";
                delete result;
                delete other;
            } catch (...) {
                delete result;
                delete other;
                throw;
            }
            return;
        }
        case 7:
            std::cout << "Norm: " << matrix->count_norm() << "\n";
            return;
        case 8: {
            int first = read_int("First row: ");
            int second = read_int("Second row: ");
            replace_matrix(matrix, matrix->swap_rows(first, second));
            return;
        }
        case 9: {
            int row = read_int("Row: ");
            int scalar = read_int("Scalar: ");
            replace_matrix(matrix, matrix->multiply_row(row, scalar));
            return;
        }
        case 10: {
            int target = read_int("Target row: ");
            int source = read_int("Source row: ");
            int scalar = read_int("Scalar: ");
            replace_matrix(matrix, matrix->add_row(target, source, scalar));
            return;
        }
        case 11: {
            int first = read_int("First column: ");
            int second = read_int("Second column: ");
            replace_matrix(matrix, matrix->swap_columns(first, second));
            return;
        }
        case 12: {
            int column = read_int("Column: ");
            int scalar = read_int("Scalar: ");
            replace_matrix(matrix, matrix->multiply_column(column, scalar));
            return;
        }
        case 13: {
            int target = read_int("Target column: ");
            int source = read_int("Source column: ");
            int scalar = read_int("Scalar: ");
            replace_matrix(matrix, matrix->add_scaled_column(target, source, scalar));
            return;
        }
        case 14:
            reset_matrix(matrix, kind);
            return;
        default:
            std::cout << "Unknown command\n";
            return;
    }
}

void run_sequence_interface(int kind) {
    DynamicArray<int> values = read_values();
    Sequence<int> *sequence = create_sequence(kind, values);

    bool running = true;
    while (running) {
        print_current_sequence(*sequence, kind);
        print_operation_menu();
        int command = read_int("Command: ");

        try {
            execute_sequence_command(sequence, kind, command, running);
        } catch (const std::exception &error) {
            std::cout << "Error: " << error.what() << "\n";
        }
    }

    delete sequence;
}

void run_vector_interface(int kind) {
    DynamicArray<int> values = read_values();
    Vector<int> *vector = create_vector(kind, values);

    bool running = true;
    while (running) {
        print_current_vector(*vector, kind);
        print_vector_operation_menu();
        int command = read_int("Command: ");

        try {
            execute_vector_command(vector, kind, command, running);
        } catch (const std::exception &error) {
            std::cout << "Error: " << error.what() << "\n";
        }
    }

    delete vector;
}

void run_matrix_interface(int kind) {
    int rows = 0;
    int columns = 0;
    DynamicArray<int> values = read_matrix_values(rows, columns);
    Matrix<int> *matrix = create_matrix(kind, rows, columns, values);

    bool running = true;
    while (running) {
        print_current_matrix(*matrix, kind);
        print_matrix_operation_menu();
        int command = read_int("Command: ");

        try {
            execute_matrix_command(matrix, kind, command, running);
        } catch (const std::exception &error) {
            std::cout << "Error: " << error.what() << "\n";
        }
    }

    delete matrix;
}

int main() {
    std::cout << "Sequence, vector and matrix console UI\n";

    bool running = true;
    while (running) {
        print_object_kind_menu();
        int kind = read_int("Type: ");

        try {
            switch (kind) {
                case 0:
                    running = false;
                    break;
                case 1:
                    run_sequence_interface(kind);
                    break;
                case 2:
                    run_sequence_interface(kind);
                    break;
                case 3:
                    run_sequence_interface(kind);
                    break;
                case 4:
                    run_sequence_interface(kind);
                    break;
                case 5:
                    run_vector_interface(kind);
                    break;
                case 6:
                    run_matrix_interface(kind);
                    break;
                case 7:
                    run_matrix_interface(kind);
                    break;
                default:
                    std::cout << "Unknown object type\n";
                    break;
            }
        } catch (const std::exception &error) {
            std::cout << "Error: " << error.what() << "\n";
        }
    }

    std::cout << "Bye\n";
    return 0;
}
