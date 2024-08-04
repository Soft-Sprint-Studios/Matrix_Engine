#ifndef DYNAMICARRAY_INLINE_HPP
#define DYNAMICARRAY_INLINE_HPP

#include "dynamicarray.h"
#include <cstdlib>
#include <cstring>

inline DynamicArray::DynamicArray()
    : items(nullptr), count(0), capacity(10) {
    items = (char**)malloc(capacity * sizeof(char*));
}

inline DynamicArray::~DynamicArray() {
    for (size_t i = 0; i < count; ++i) {
        free(items[i]);
    }
    free(items);
}

inline void DynamicArray::add(const char* item) {
    if (count >= capacity) {
        resize(capacity * 2);
    }
    size_t len = strlen(item);
    items[count] = (char*)malloc(len + 1);
    strcpy(items[count], item);
    ++count;
}

inline size_t DynamicArray::size() const {
    return count;
}

inline const char* DynamicArray::operator[](size_t index) const {
    return index < count ? items[index] : nullptr;
}

inline void DynamicArray::resize(size_t newSize) {
    items = (char**)realloc(items, newSize * sizeof(char*));
    capacity = newSize;
}

#endif // DYNAMICARRAY_INLINE_HPP
