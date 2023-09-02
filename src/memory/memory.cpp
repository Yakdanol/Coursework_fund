#include "memory.h"

void *memory::operator+=(size_t const &size) const
{
    return allocate(size);   
}

void memory::operator-=(void *memory_block) const
{
    deallocate(memory_block);   
}
