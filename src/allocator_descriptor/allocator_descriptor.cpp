#include "allocator_descriptor.h"
#include <sstream>

template <typename T>
std::string to_string(T object)
{
    std::stringstream stream;
    stream << object;

    return stream.str();
}

std::string get_bytes_descriptor(void *object)
{
    std::string result;
    auto *size_object = reinterpret_cast<size_t*>(object);
    auto **prev_block = reinterpret_cast<void**>(size_object + 1);
    auto **next_block = reinterpret_cast<void**>(prev_block + 1);
    auto *bytes = reinterpret_cast<unsigned char*>(next_block + 1);

    size_t size = *size_object - sizeof(size_t) - 2 * sizeof(void*);

    for (int i = 0; i < size; i++)
    {
        result += to_string(static_cast<int>(bytes[i])) + " ";
    }

    return result;
}

logger *allocator_descriptor::get_logger() const
{
    auto *size_space = reinterpret_cast<size_t*>(_allocated_memory);
    auto **first_occupied = reinterpret_cast<void**>(size_space + 1);
    auto **outer_allocator = reinterpret_cast<memory**>(first_occupied + 1);

    return *reinterpret_cast<logger**>(outer_allocator + 1);
}

memory *allocator_descriptor::get_memory() const
{
    auto *size_space = reinterpret_cast<size_t*>(_allocated_memory);
    auto **first_occupied = reinterpret_cast<void**>(size_space + 1);

    return *reinterpret_cast<memory**>(first_occupied + 1);
}

void *allocator_descriptor::get_next_occupied_block(void *memory_block) const
{
    auto *size_space = reinterpret_cast<size_t*>(memory_block);
    auto **prev_occupied_block = reinterpret_cast<void**>(size_space + 1);

    return *reinterpret_cast<void**>(prev_occupied_block + 1);
}

size_t allocator_descriptor::get_memory_size() const
{
    return *reinterpret_cast<size_t*>(_allocated_memory);
}

size_t allocator_descriptor::get_block_size(void const *memory_block) const
{
    return *reinterpret_cast<size_t const*>(memory_block);
}

void *allocator_descriptor::get_first_occupied_block() const
{
    auto *size_space = reinterpret_cast<size_t*>(_allocated_memory);

    return *reinterpret_cast<void**>(size_space + 1);
}

void *allocator_descriptor::get_first_suitable_block(
        size_t const &size,
        void **prev_block,
        void **next_block) const
{
    size_t size_to_allocate = size + sizeof(size_t) + 2 * sizeof(void*);
    void *current_occupied_block = get_first_occupied_block();
    void *prev_occupied_block = nullptr;

    *prev_block = nullptr;
    *next_block = current_occupied_block;

    while (current_occupied_block != nullptr)
    {
        auto *end_address = reinterpret_cast<unsigned char*>(current_occupied_block);
        unsigned char *begin_address = nullptr;

        if (prev_occupied_block == nullptr)
        {
            begin_address = reinterpret_cast<unsigned char*>(_allocated_memory) + sizeof(size_t) + sizeof(void*) + sizeof(memory*) + sizeof(logger*);
        }

        else
        {
            begin_address = reinterpret_cast<unsigned char*>(prev_occupied_block) + get_block_size(prev_occupied_block);
        }

        size_t distance = end_address - begin_address;

        if (distance >= size_to_allocate)
        {
            *prev_block = prev_occupied_block;
            *next_block = current_occupied_block;

            return begin_address;
        }

        prev_occupied_block = current_occupied_block;
        current_occupied_block = get_next_occupied_block(current_occupied_block);
    }

    unsigned char *end_address = reinterpret_cast<unsigned char*>(_allocated_memory) + get_memory_size();
    unsigned char *begin_address = nullptr;

    if (prev_occupied_block == nullptr)
    {
        begin_address = reinterpret_cast<unsigned char*>(_allocated_memory) + sizeof(size_t) + sizeof(void*) + sizeof(memory*) + sizeof(logger*);
    }

    else
    {
        begin_address = reinterpret_cast<unsigned char*>(prev_occupied_block) + get_block_size(prev_occupied_block);
    }

    size_t distance = end_address - begin_address;

    if (distance >= size_to_allocate)
    {
        *prev_block = prev_occupied_block;
        *next_block = current_occupied_block;

        return begin_address;
    }

    return nullptr;
}

void *allocator_descriptor::get_best_suitable_block(
        size_t const &size,
        void **prev_block,
        void **next_block) const
{
    size_t size_to_allocate = size + sizeof(size_t) + 2 * sizeof(void*);
    void *current_occupied_block = get_first_occupied_block();
    void *prev_occupied_block = nullptr;
    void *allocated_block;

    *prev_block = nullptr;
    *next_block = current_occupied_block;
    size_t max_size = 0;

    while (current_occupied_block != nullptr)
    {
        auto *end_address = reinterpret_cast<unsigned char*>(current_occupied_block);
        unsigned char *begin_address = nullptr;

        if (prev_occupied_block == nullptr)
        {
            begin_address = reinterpret_cast<unsigned char*>(_allocated_memory) + sizeof(size_t) + sizeof(void*) + sizeof(memory*) + sizeof(logger*);
        }

        else
        {
            begin_address = reinterpret_cast<unsigned char*>(prev_occupied_block) + get_block_size(prev_occupied_block);
        }

        size_t distance = end_address - begin_address;

        if (distance >= size_to_allocate)
        {
            if (distance >= max_size)
            {
                max_size = distance;
                *prev_block = prev_occupied_block;
                *next_block = current_occupied_block;

                allocated_block = begin_address;
            }
        }

        prev_occupied_block = current_occupied_block;
        current_occupied_block = get_next_occupied_block(current_occupied_block);
    }

    unsigned char *end_address = reinterpret_cast<unsigned char*>(_allocated_memory) + get_memory_size();
    unsigned char *begin_address = nullptr;

    if (prev_occupied_block == nullptr)
    {
        begin_address = reinterpret_cast<unsigned char*>(_allocated_memory) + sizeof(size_t) + sizeof(void*) + sizeof(memory*) + sizeof(logger*);
    }

    else
    {
        begin_address = reinterpret_cast<unsigned char*>(prev_occupied_block) + get_block_size(prev_occupied_block);
    }

    size_t distance = end_address - begin_address;

    if (distance >= size_to_allocate)
    {
        if (distance >= max_size)
        {
            max_size = distance;
            *prev_block = prev_occupied_block;
            *next_block = current_occupied_block;

            allocated_block = begin_address;
        }
    }

    return allocated_block;
}

void *allocator_descriptor::get_worse_suitable_block(
        size_t const &size,
        void **prev_block,
        void **next_block) const
{
    size_t size_to_allocate = size + sizeof(size_t) + 2 * sizeof(void*);
    void *current_occupied_block = get_first_occupied_block();
    void *prev_occupied_block = nullptr;
    void *allocated_block;
    size_t min_size = get_memory_size();

    *prev_block = nullptr;
    *next_block = current_occupied_block;

    while (current_occupied_block != nullptr)
    {
        auto *end_address = reinterpret_cast<unsigned char*>(current_occupied_block);
        unsigned char *begin_address = nullptr;

        if (prev_occupied_block == nullptr)
        {
            begin_address = reinterpret_cast<unsigned char*>(_allocated_memory) + sizeof(size_t) + sizeof(void*) + sizeof(memory*) + sizeof(logger*);
        }

        else
        {
            begin_address = reinterpret_cast<unsigned char*>(prev_occupied_block) + get_block_size(prev_occupied_block);
        }

        size_t distance = end_address - begin_address;

        if (distance >= size_to_allocate)
        {
            if (distance <= min_size)
            {
                *prev_block = prev_occupied_block;
                *next_block = current_occupied_block;

                allocated_block = begin_address;
            }
        }

        prev_occupied_block = current_occupied_block;
        current_occupied_block = get_next_occupied_block(current_occupied_block);
    }

    unsigned char *end_address = reinterpret_cast<unsigned char*>(_allocated_memory) + get_memory_size();
    unsigned char *begin_address = nullptr;

    if (prev_occupied_block == nullptr)
    {
        begin_address = reinterpret_cast<unsigned char*>(_allocated_memory) + sizeof(size_t) + sizeof(void*) + sizeof(memory*) + sizeof(logger*);
    }

    else
    {
        begin_address = reinterpret_cast<unsigned char*>(prev_occupied_block) + get_block_size(prev_occupied_block);
    }

    size_t distance = end_address - begin_address;

    if (distance >= size_to_allocate)
    {
        if (distance <= min_size)
        {
            min_size = distance;
            *prev_block = prev_occupied_block;
            *next_block = current_occupied_block;

            allocated_block = begin_address;
        }
    }

    return allocated_block;
}

allocator_descriptor::allocator_descriptor(
        size_t const &size,
        memory *outer_allocator,
        logger *logger_allocator,
        memory::ALLOCATION_METOD method_allocation)
{
    _method = method_allocation;

    if (outer_allocator != nullptr)
    {
        _allocated_memory = outer_allocator->allocate(size);
    }

    else
    {
        _allocated_memory = ::operator new(size + sizeof(size_t) + sizeof(void*) + sizeof(memory*) + sizeof(logger*));
    }

    auto *size_memory = reinterpret_cast<size_t*>(_allocated_memory);
    *size_memory = size;

    void **first_occupied_block = reinterpret_cast<void**>(size_memory + 1);
    *first_occupied_block = nullptr;

    auto **outer_allocator_space = reinterpret_cast<memory**>(first_occupied_block + 1);
    *outer_allocator_space = outer_allocator;

    auto **logger_space = reinterpret_cast<logger**>(outer_allocator_space + 1);
    *logger_space = logger_allocator;

    if (*logger_space != nullptr)
    {
        (*logger_space)->log("allocator_descriptor is created", logger::severity::INFORMATION);
        (*logger_space)->log("size of memory allocated for allocator_descriptor: " + to_string(size), logger::severity::INFORMATION);
    }
}

void *allocator_descriptor::allocate(
        size_t const &size) const
{
    size_t size_to_allocate = size + sizeof(size_t) + 2 * sizeof(void*);
    void *prev_block = nullptr;
    void *next_block = nullptr;
    void *allocated_block = nullptr;

    memory *allocator = get_memory();
    logger *log_memory = get_logger();

    if (allocator == nullptr)
    {
        switch (_method)
        {
            case memory::ALLOCATION_METOD::FIRST_SUITABLE:
                allocated_block = get_first_suitable_block(size, &prev_block, &next_block);
                break;

            case memory::ALLOCATION_METOD::BEST_SUITABLE:
                allocated_block = get_best_suitable_block(size, &prev_block, &next_block);
                break;

            case memory::ALLOCATION_METOD::WORSE_SUITABLE:
                allocated_block = get_worse_suitable_block(size, &prev_block, &next_block);
                break;

            default:
                break;
        }

        if (allocated_block == nullptr)
        {
            throw std::bad_alloc();
        }

        auto *size_space = reinterpret_cast<size_t*>(_allocated_memory);
        auto **first_occupied = reinterpret_cast<void**>(size_space + 1);

        if (*first_occupied == nullptr)
        {
            auto *size_block = reinterpret_cast<size_t*>(allocated_block);

            size_t distance = get_memory_size() - sizeof(size_t) - sizeof(void*) - sizeof(memory*) - sizeof(logger*);

            if (distance - size_to_allocate < sizeof(size_t) + 2 * sizeof(void*))
            {
                *size_block = distance;
            }

            else
            {
                *size_block = size_to_allocate;
            }

            auto **prev_block = reinterpret_cast<void**>(size_block + 1);
            *prev_block = nullptr;

            auto **next_block = reinterpret_cast<void**>(prev_block + 1);
            *next_block = nullptr;

            *first_occupied = allocated_block;
        }

        else
        {
            if (prev_block != nullptr &&
                next_block != nullptr)
            {
                auto *end_address = reinterpret_cast<unsigned char*>(next_block);
                auto *current_address = reinterpret_cast<unsigned char*>(prev_block) + get_block_size(prev_block);
                size_t distance = end_address - current_address;

                auto *size_current_block = reinterpret_cast<size_t*>(prev_block);
                auto **prev_current_block = reinterpret_cast<void**>(size_current_block + 1);
                auto **next_current_block = reinterpret_cast<void**>(prev_current_block + 1);

                auto *size_next_block = reinterpret_cast<size_t*>(next_block);
                auto **prev_next_occupied_block = reinterpret_cast<void**>(size_next_block + 1);

                auto *size_allocated_block = reinterpret_cast<size_t*>(allocated_block);
                auto **prev_allocated_block = reinterpret_cast<void**>(size_allocated_block + 1);
                auto **next_allocated_block = reinterpret_cast<void**>(prev_allocated_block + 1);

                if (distance - size_to_allocate < sizeof(size_t) + 2 * sizeof(void*))
                {
                    *size_allocated_block = distance;
                }
                else
                {
                    *size_allocated_block = size_to_allocate;
                }

                *next_current_block = allocated_block;
                *prev_next_occupied_block = allocated_block;

                *prev_allocated_block = prev_block;
                *next_allocated_block = next_block;
            }

            else
            {
                if (prev_block != nullptr &&
                    next_block == nullptr)
                {
                    auto *end_address = reinterpret_cast<char*>(_allocated_memory) + get_memory_size();
                    auto *begin_address = reinterpret_cast<char*>(prev_block) + get_block_size(prev_block);
                    size_t distance = end_address - begin_address;

                    auto *size_current_block = reinterpret_cast<size_t*>(prev_block);
                    auto **prev_current_block = reinterpret_cast<void**>(size_current_block + 1);
                    auto **next_current_block = reinterpret_cast<void**>(prev_current_block + 1);

                    auto *size_allocated_block = reinterpret_cast<size_t*>(allocated_block);
                    auto **prev_allocated_block = reinterpret_cast<void**>(size_allocated_block + 1);
                    auto **next_allocated_block = reinterpret_cast<void**>(prev_allocated_block + 1);

                    if (distance - size_to_allocate < sizeof(size_t) + 2 * sizeof(void*))
                    {
                        *size_allocated_block = distance;
                    }

                    else
                    {
                        *size_allocated_block = size_to_allocate;
                    }

                    *prev_allocated_block = prev_block;
                    *next_allocated_block = nullptr;

                    *next_current_block = allocated_block;
                }
            }
        }

        if (log_memory != nullptr)
        {
            log_memory->log("block (" + to_string(allocated_block) + ") size allocated from allocator_descriptor: " + to_string(size), logger::severity::INFORMATION);
        }
    }

    else
    {
        return allocator->allocate(size);
    }

    return reinterpret_cast<char*>(allocated_block) + sizeof(size_t) + 2 * sizeof(void*);
}

void allocator_descriptor::deallocate(void *target) const
{
    if (target == nullptr)
    {
        return;
    }

    auto **next_deallocated_block = reinterpret_cast<void**>(target) - 1;
    auto **prev_deallocated_block = next_deallocated_block - 1;
    void *address_deallocated_block = reinterpret_cast<size_t*>(prev_deallocated_block) - 1;

    if (target == nullptr)
    {
        return;
    }

    memory *allocator = get_memory();
    logger *log_memory = get_logger();

    if (log_memory != nullptr)
    {
        log_memory->log("deallocated block (" + to_string(target) + ") : " + get_bytes_descriptor(address_deallocated_block), logger::severity::INFORMATION);
    }

    if (allocator == nullptr)
    {
        if (*next_deallocated_block != nullptr)
        {
            auto *size_space = reinterpret_cast<size_t*>(*next_deallocated_block);
            auto **prev_block = reinterpret_cast<void**>(size_space + 1);

            *prev_block = *prev_deallocated_block;
        }

        if (*prev_deallocated_block != nullptr)
        {
            auto *size_space = reinterpret_cast<size_t*>(*prev_deallocated_block);
            auto **prev_block = reinterpret_cast<void**>(size_space + 1);
            auto **next_block = reinterpret_cast<void**>(prev_block + 1);

            *next_block = *next_deallocated_block;
        }

        else
        {
            auto *size_memory = reinterpret_cast<size_t*>(_allocated_memory);
            auto **first_occupied_block = reinterpret_cast<void**>(size_memory + 1);

            *first_occupied_block = *next_deallocated_block;
        }
    }

    else
    {
        allocator->deallocate(target);
    }
}

allocator_descriptor::~allocator_descriptor()
{
    logger *log_memory = get_logger();
    memory *allocator = get_memory();

    if (log_memory != nullptr)
    {
        log_memory->log("allocator_descriptor is destroyed", logger::severity::INFORMATION);
    }

    if (allocator != nullptr)
    {
        allocator->deallocate(_allocated_memory);
    }

    else
    {
        delete _allocated_memory;
    }
}
