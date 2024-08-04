#ifndef DYNAMICARRAY_H
#define DYNAMICARRAY_H

#include <cstddef>  // For size_t

class DynamicArray {
public:
    DynamicArray();
    ~DynamicArray();

    void add(const char* item);
    size_t size() const;
    const char* operator[](size_t index) const;

private:
    void resize(size_t newSize);

    char** items;
    size_t count;
    size_t capacity;
};

#include "dynamicarray_inline.hpp"

#endif // DYNAMICARRAY_H