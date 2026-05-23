#include "include/ArraySequence.h"
#include "include/ListSequence.h"
#include "print.h"
#include "util.h"

#include <iostream>

int main() {
    int items[] = {1, 2, 3};

    std::cout << "MutableArraySequence\n";
    MutableArraySequence<int> array_sequence(items, 3);
    print_sequence("start", array_sequence);

    array_sequence.append(4);
    array_sequence.prepend(0);
    array_sequence.insert(99, 2);
    print_sequence("after append/prepend/insert", array_sequence);

    array_sequence.remove(2);
    print_sequence("after remove", array_sequence);

    Sequence<int> *array_subsequence = array_sequence.get_subsequence(1, 3);
    Sequence<int> *array_mapped = array_sequence.map(square);
    Sequence<int> *array_filtered = array_sequence.where(is_even);

    print_sequence("subsequence(1, 3)", *array_subsequence);
    print_sequence("map(square)", *array_mapped);
    print_sequence("where(is_even)", *array_filtered);
    std::cout << "reduce(sum, 0) = " << array_sequence.reduce(sum, 0) << "\n";
    print_option("try_get(2)", array_sequence.try_get(2));
    print_option("try_get(100)", array_sequence.try_get(100));
    print_with_enumerator("enumerator", array_sequence);

    delete array_subsequence;
    delete array_mapped;
    delete array_filtered;

    std::cout << "\nImmutableArraySequence\n";
    ImmutableArraySequence<int> immutable_array(items, 3);
    Sequence<int> *array_added = immutable_array.append(4);

    print_sequence("original", immutable_array);
    print_sequence("append result", *array_added);
    delete array_added;

    std::cout << "\nMutableListSequence\n";
    MutableListSequence<int> list_sequence(items, 3);
    print_sequence("start", list_sequence);

    list_sequence.append(4);
    list_sequence.prepend(0);
    list_sequence.insert(99, 2);
    print_sequence("after append/prepend/insert", list_sequence);

    list_sequence.remove(2);
    print_sequence("after remove", list_sequence);

    Sequence<int> *list_subsequence = list_sequence.get_subsequence(1, 3);
    Sequence<int> *list_mapped = list_sequence.map(square);
    Sequence<int> *list_filtered = list_sequence.where(is_even);

    print_sequence("subsequence(1, 3)", *list_subsequence);
    print_sequence("map(square)", *list_mapped);
    print_sequence("where(is_even)", *list_filtered);
    std::cout << "reduce(sum, 0) = " << list_sequence.reduce(sum, 0) << "\n";
    print_option("try_get(2)", list_sequence.try_get(2));
    print_option("try_get(100)", list_sequence.try_get(100));
    print_with_enumerator("enumerator", list_sequence);

    delete list_subsequence;
    delete list_mapped;
    delete list_filtered;

    std::cout << "\nImmutableListSequence\n";
    ImmutableListSequence<int> immutable_list(items, 3);
    Sequence<int> *list_added = immutable_list.append(4);

    print_sequence("original", immutable_list);
    print_sequence("append result", *list_added);
    delete list_added;

    return 0;
}
