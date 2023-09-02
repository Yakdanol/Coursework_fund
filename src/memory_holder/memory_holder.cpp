#include "memory_holder.h"

void *memory_holder::guard_allocate(size_t size_allocate) const
{
    memory *allocator = get_memory();

    if (allocator == nullptr)
    {
        return ::operator new(size_allocate);
    }

    else
    {
        return allocator->allocate(size_allocate);
    }
}


void memory_holder::guard_deallocate(void *object) const
{
    memory *allocator = get_memory();

    if (allocator == nullptr)
    {
        ::operator delete(object);
    }

    else
    {
        allocator->deallocate(object);;
    }
}