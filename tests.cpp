#include <iostream>
#include <stdexcept>
#include <string>

#include "include/ArraySequence.h"
#include "include/DynamicArray.h"
#include "include/LinkedList.h"
#include "include/ListSequence.h"

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

template <class T, class U>
void expect_eq(const T &actual, const U &expected, const std::string &name) {
    if (actual == expected) {
        passed++;
        std::cout << "[PASS] " << name << '\n';
    } else {
        failed++;
        std::cout << "[FAIL] " << name << " expected=" << expected << " actual=" << actual << '\n';
    }
}

template <class Object>
void expect_throw(void (*function)(Object &), Object &object, const std::string &name) {
    try {
        function(object);
        failed++;
        std::cout << "[FAIL] " << name << " expected exception\n";
    } catch (const std::exception &) {
        passed++;
        std::cout << "[PASS] " << name << '\n';
    }
}

int square(const int &value) {
    return value * value;
}

bool is_even(const int &value) {
    return value % 2 == 0;
}

int sum_values(const int &left, const int &right) {
    return left + right;
}

void dynamic_array_get_negative(DynamicArray<int> &array) {
    array.get(-1);
}

void dynamic_array_set_past_end(DynamicArray<int> &array) {
    array.set(5, 0);
}

void linked_list_get_negative(LinkedList<int> &list) {
    list.get(-1);
}

void linked_list_get_sublist_invalid(LinkedList<int> &list) {
    list.get_sublist(3, 1);
}

void expect_sequence(Sequence<int> *sequence, const int *expected, int count, const std::string &name) {
    expect_eq(sequence->get_length(), count, name + " length");
    for (int i = 0; i < count; i++) {
        expect_eq(sequence->get(i), expected[i], name + " item " + std::to_string(i));
    }
}

void test_dynamic_array() {
    int raw[] = {1, 2, 3};
    DynamicArray<int> array(raw, 3);

    expect_eq(array.get_size(), 3, "DynamicArray constructor length");
    expect_eq(array.get(1), 2, "DynamicArray get");

    array.set(1, 20);
    expect_eq(array.get(1), 20, "DynamicArray set");

    array.resize(5);
    array.set(4, 50);
    expect_eq(array.get_size(), 5, "DynamicArray resize grow");
    expect_eq(array.get(4), 50, "DynamicArray resized value");

    DynamicArray<int> copied(array);
    copied.set(0, 100);
    expect_eq(array.get(0), 1, "DynamicArray copy constructor is deep");

    DynamicArray<int> assigned;
    assigned = array;
    assigned.set(0, 200);
    expect_eq(array.get(0), 1, "DynamicArray operator= is deep");

    expect_throw(dynamic_array_get_negative, array, "DynamicArray get negative index throws");
    expect_throw(dynamic_array_set_past_end, array, "DynamicArray set past end throws");
}

void test_linked_list() {
    int raw[] = {2, 3, 4};
    LinkedList<int> list(raw, 3);

    expect_eq(list.get_length(), 3, "LinkedList constructor length");
    expect_eq(list.get(0), 2, "LinkedList first by index");
    expect_eq(list.get(list.get_length() - 1), 4, "LinkedList last by index");
    expect_eq(list.get(1), 3, "LinkedList get");

    list.prepend(1);
    list.append(5);
    list.insert(10, 2);
    list.remove(2);

    expect_eq(list.get_length(), 5, "LinkedList mutation length");
    expect_eq(list.get(0), 1, "LinkedList prepend");
    expect_eq(list.get(4), 5, "LinkedList append");
    expect_eq(list[2], 3, "LinkedList operator[]");

    LinkedList<int> sublist = list.get_sublist(1, 3);
    expect_eq(sublist.get_length(), 3, "LinkedList get_sublist length");
    expect_eq(sublist.get(0), 2, "LinkedList get_sublist first item");
    expect_eq(sublist.get(2), 4, "LinkedList get_sublist last item");

    LinkedList<int> copied(list);
    copied.remove(0);
    expect_eq(list.get(0), 1, "LinkedList copy constructor is deep");

    LinkedList<int> assigned;
    assigned = list;
    assigned.remove(0);
    expect_eq(list.get(0), 1, "LinkedList operator= is deep");

    expect_throw(linked_list_get_negative, list, "LinkedList get negative index throws");
    expect_throw(linked_list_get_sublist_invalid, list, "LinkedList get_sublist invalid range throws");
}

void test_mutable_array_sequence() {
    int raw[] = {1, 2, 3};
    MutableArraySequence<int> sequence(raw, 3);

    expect(sequence.append(4) == &sequence, "MutableArraySequence append returns this");
    expect(sequence.prepend(0) == &sequence, "MutableArraySequence prepend returns this");
    expect(sequence.insert(10, 2) == &sequence, "MutableArraySequence insert returns this");
    expect(sequence.remove(2) == &sequence, "MutableArraySequence remove returns this");

    int expected_after_mutations[] = {0, 1, 2, 3, 4};
    expect_sequence(&sequence, expected_after_mutations, 5, "MutableArraySequence mutations");

    Sequence<int> *subsequence = sequence.get_subsequence(1, 3);
    int expected_subsequence[] = {1, 2, 3};
    expect_sequence(subsequence, expected_subsequence, 3, "MutableArraySequence get_subsequence");
    delete subsequence;

    Sequence<int> *mapped = sequence.map(square);
    int expected_mapped[] = {0, 1, 4, 9, 16};
    expect_sequence(mapped, expected_mapped, 5, "MutableArraySequence map");

    Sequence<int> *filtered = sequence.where(is_even);
    int expected_filtered[] = {0, 2, 4};
    expect_sequence(filtered, expected_filtered, 3, "MutableArraySequence where");

    expect_eq(sequence.reduce(sum_values, 0), 10, "MutableArraySequence reduce");
    expect(sequence.try_get(2).has_value(), "MutableArraySequence try_get existing has value");
    expect_eq(sequence.try_get(2).get_value(), 2, "MutableArraySequence try_get existing value");
    expect(!sequence.try_get(10).has_value(), "MutableArraySequence try_get invalid is empty");

    IEnumerator<int> *enumerator = sequence.get_enumerator();
    int index = 0;
    while (enumerator->move_next()) {
        expect_eq(enumerator->get_curr(), expected_after_mutations[index], "MutableArraySequence enumerator item " + std::to_string(index));
        index++;
    }
    expect_eq(index, 5, "MutableArraySequence enumerator count");
    enumerator->reset();
    expect(enumerator->move_next(), "MutableArraySequence enumerator reset");
    expect_eq(enumerator->get_curr(), 0, "MutableArraySequence enumerator first after reset");
    delete enumerator;

    delete mapped;
    delete filtered;
}

void test_immutable_array_sequence() {
    int raw[] = {1, 2, 3};
    ImmutableArraySequence<int> sequence(raw, 3);

    Sequence<int> *appended = sequence.append(4);
    int expected_original[] = {1, 2, 3};
    int expected_appended[] = {1, 2, 3, 4};

    expect(appended != &sequence, "ImmutableArraySequence append returns new sequence");
    expect_sequence(&sequence, expected_original, 3, "ImmutableArraySequence original after append");
    expect_sequence(appended, expected_appended, 4, "ImmutableArraySequence appended result");
    delete appended;

    Sequence<int> *removed = sequence.remove(1);
    int expected_removed[] = {1, 3};
    expect(removed != &sequence, "ImmutableArraySequence remove returns new sequence");
    expect_sequence(&sequence, expected_original, 3, "ImmutableArraySequence original after remove");
    expect_sequence(removed, expected_removed, 2, "ImmutableArraySequence remove result");
    delete removed;
}

void test_mutable_list_sequence() {
    int raw[] = {1, 2, 3};
    MutableListSequence<int> sequence(raw, 3);

    expect(sequence.append(4) == &sequence, "MutableListSequence append returns this");
    expect(sequence.prepend(0) == &sequence, "MutableListSequence prepend returns this");
    expect(sequence.insert(10, 2) == &sequence, "MutableListSequence insert returns this");
    expect(sequence.remove(2) == &sequence, "MutableListSequence remove returns this");

    int expected_after_mutations[] = {0, 1, 2, 3, 4};
    expect_sequence(&sequence, expected_after_mutations, 5, "MutableListSequence mutations");

    Sequence<int> *subsequence = sequence.get_subsequence(1, 3);
    int expected_subsequence[] = {1, 2, 3};
    expect_sequence(subsequence, expected_subsequence, 3, "MutableListSequence get_subsequence");
    delete subsequence;

    Sequence<int> *mapped = sequence.map(square);
    int expected_mapped[] = {0, 1, 4, 9, 16};
    expect_sequence(mapped, expected_mapped, 5, "MutableListSequence map");

    Sequence<int> *filtered = sequence.where(is_even);
    int expected_filtered[] = {0, 2, 4};
    expect_sequence(filtered, expected_filtered, 3, "MutableListSequence where");

    expect_eq(sequence.reduce(sum_values, 0), 10, "MutableListSequence reduce");
    expect(sequence.try_get(2).has_value(), "MutableListSequence try_get existing has value");
    expect_eq(sequence.try_get(2).get_value(), 2, "MutableListSequence try_get existing value");
    expect(!sequence.try_get(10).has_value(), "MutableListSequence try_get invalid is empty");

    IEnumerator<int> *enumerator = sequence.get_enumerator();
    int index = 0;
    while (enumerator->move_next()) {
        expect_eq(enumerator->get_curr(), expected_after_mutations[index], "MutableListSequence enumerator item " + std::to_string(index));
        index++;
    }
    expect_eq(index, 5, "MutableListSequence enumerator count");
    enumerator->reset();
    expect(enumerator->move_next(), "MutableListSequence enumerator reset");
    expect_eq(enumerator->get_curr(), 0, "MutableListSequence enumerator first after reset");
    delete enumerator;

    delete mapped;
    delete filtered;
}

void test_immutable_list_sequence() {
    int raw[] = {1, 2, 3};
    ImmutableListSequence<int> sequence(raw, 3);

    Sequence<int> *appended = sequence.append(4);
    int expected_original[] = {1, 2, 3};
    int expected_appended[] = {1, 2, 3, 4};

    expect(appended != &sequence, "ImmutableListSequence append returns new sequence");
    expect_sequence(&sequence, expected_original, 3, "ImmutableListSequence original after append");
    expect_sequence(appended, expected_appended, 4, "ImmutableListSequence appended result");
    delete appended;

    Sequence<int> *removed = sequence.remove(1);
    int expected_removed[] = {1, 3};
    expect(removed != &sequence, "ImmutableListSequence remove returns new sequence");
    expect_sequence(&sequence, expected_original, 3, "ImmutableListSequence original after remove");
    expect_sequence(removed, expected_removed, 2, "ImmutableListSequence remove result");
    delete removed;
}

void test_concat() {
    int left_raw[] = {1, 2};
    int right_raw[] = {3, 4};
    int expected[] = {1, 2, 3, 4};

    MutableArraySequence<int> array_left(left_raw, 2);
    MutableArraySequence<int> array_right(right_raw, 2);
    Sequence<int> *array_concat = array_left.concat(array_right);
    expect_sequence(array_concat, expected, 4, "MutableArraySequence concat");

    MutableListSequence<int> list_left(left_raw, 2);
    MutableArraySequence<int> list_right(right_raw, 2);
    Sequence<int> *list_concat = list_left.concat(list_right);
    expect_sequence(list_concat, expected, 4, "MutableListSequence concat");
    if (list_concat != &list_left) {
        delete list_concat;
    }
}

} // namespace

int main() {
    test_dynamic_array();
    test_linked_list();
    test_mutable_array_sequence();
    test_immutable_array_sequence();
    test_mutable_list_sequence();
    test_immutable_list_sequence();
    test_concat();

    std::cout << "\nPassed: " << passed << "\nFailed: " << failed << '\n';
    if (failed == 0) {
        return 0;
    }
    return 1;
}
