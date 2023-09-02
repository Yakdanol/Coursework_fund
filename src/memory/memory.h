#ifndef MEMORY_H
#define MEMORY_H

#include <iostream>

class memory
{

public:

    enum class ALLOCATION_METOD
    {
        FIRST_SUITABLE,
        BEST_SUITABLE,
        WORSE_SUITABLE
    };

    virtual void *allocate(size_t const&) const = 0;

    virtual void deallocate(void*) const = 0;

    void *operator+=(size_t const &size) const;

    void operator-=(void*) const;

    memory(memory const&) = delete;
    
    void operator=(memory const&) = delete;

    memory() = default;
    
    virtual ~memory() = default;

};

#endif // MEMORY_H