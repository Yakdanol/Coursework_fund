#ifndef MEMORY_HOLDER_H
#define MEMORY_HOLDER_H

#include "memory.h"


class memory_holder
{

public:

    void *guard_allocate(size_t size_allocate) const;

    void guard_deallocate(void *object) const;

    virtual memory *get_memory() const = 0;

    virtual ~memory_holder() = default;

};

#endif //MEMORY_HOLDER_H