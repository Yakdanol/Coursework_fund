#include "allocator_list.h"
#include <sstream>

template <typename T>
std::string to_string(T object)
{
    std::stringstream stream;
    stream << object;

    return stream.str();
}

allocator_list::allocator_list(
        size_t const &size,
        memory *outer_allocator,
        logger *logger_allocator,
        memory::ALLOCATION_METOD method_allocation)
{
    _method = method_allocation;

    if (outer_allocator)
    {
        _allocated_memory = outer_allocator->allocate(size);
    }

    else
    {
        _allocated_memory = ::operator new(size);
    }

    auto *size_memory = reinterpret_cast<size_t*>(_allocated_memory);
    *size_memory = size;

    void **first_free_block = reinterpret_cast<void**>(size_memory + 1);

    auto **outer_allocator_list_space = reinterpret_cast<memory**>(first_free_block + 1);
    *outer_allocator_list_space = outer_allocator;

    auto **logger_space = reinterpret_cast<logger**>(outer_allocator_list_space + 1);
    *logger_space = logger_allocator;

    *first_free_block = logger_space + 1;

    auto *size_space_first_block = reinterpret_cast<size_t*>(*first_free_block);
    *size_space_first_block = size - sizeof(size_t) - sizeof(void*) - sizeof(memory*) - sizeof(logger*);

    void **next_free_block = reinterpret_cast<void**>(size_space_first_block + 1);
    *next_free_block = nullptr;

    if (*logger_space != nullptr)
    {
        (*logger_space)->log("allocator_list is created", logger::severity::INFORMATION);
        (*logger_space)->log("size of memory allocated for allocator_list: " + to_string(size), logger::severity::INFORMATION);
    }
}

void *allocator_list::allocate(size_t const &size) const
{
    size_t size_to_allocate = size + sizeof(size_t) + sizeof(void*);
    void *prev_block = nullptr;
    void *block_to_allocate = nullptr;

    switch (_method)
    {
        case ALLOCATION_METOD::FIRST_SUITABLE:
            block_to_allocate = get_first_suitable_block(size, &prev_block);
            break;

        case ALLOCATION_METOD::BEST_SUITABLE:
            block_to_allocate = get_best_suitable_block(size, &prev_block);
            break;

        case ALLOCATION_METOD::WORSE_SUITABLE:
            block_to_allocate = get_worse_suitable_block(size, &prev_block);
            break;

        default:
            break;
    }

    if (block_to_allocate == nullptr)
    {
        throw std::bad_alloc();
    }

    logger *log_memory = get_logger();

    if (log_memory != nullptr)
    {
        log_memory->log("block (" + to_string(block_to_allocate) + ") size allocated from allocator_list: " + to_string(size), logger::severity::INFORMATION);
    }

    size_t size_free_block = get_block_size(block_to_allocate);

    if (prev_block == nullptr)
    {
        auto *size_space_block_to_allocate = reinterpret_cast<size_t*>(block_to_allocate);
        auto **next_block = reinterpret_cast<void**>(size_space_block_to_allocate + 1);
        auto *size_space = reinterpret_cast<size_t*>(_allocated_memory);
        void **first_free_block = reinterpret_cast<void**>(size_space + 1);
        void *next_free_block = get_next_free_block(block_to_allocate);

        if (size_free_block - size_to_allocate < sizeof(size_t) + sizeof(void*))
        {
            *first_free_block = next_free_block;
        }

        else
        {
            *size_space_block_to_allocate = size_to_allocate;

            void *new_block = reinterpret_cast<char*>(block_to_allocate) + size_to_allocate;
            auto *size_space_new_block = reinterpret_cast<size_t*>(new_block);
            auto **next_free_new_block = reinterpret_cast<void**>(size_space_new_block + 1);

            *size_space_new_block = size_free_block - size_to_allocate;
            *next_free_new_block = next_free_block;

            *first_free_block = new_block;
        }

        *next_block = nullptr;
    }

    else
    {
        auto *size_space_block_to_allocate = reinterpret_cast<size_t*>(block_to_allocate);
        auto **next_block = reinterpret_cast<void**>(size_space_block_to_allocate + 1);
        auto *size_space = reinterpret_cast<size_t*>(prev_block);
        void **prev_next_block = reinterpret_cast<void**>(size_space + 1);
        void *next_free_block = get_next_free_block(block_to_allocate);

        if (size_free_block - size_to_allocate < sizeof(size_t) + sizeof(void*))
        {
            *prev_next_block = next_free_block;
        }

        else
        {
            *size_space_block_to_allocate = size_to_allocate;

            void *new_block = reinterpret_cast<char*>(block_to_allocate) + size_to_allocate;
            auto *size_space_new_block = reinterpret_cast<size_t*>(new_block);
            auto **next_free_new_block = reinterpret_cast<void**>(size_space_new_block + 1);

            *size_space_new_block = size_free_block - size_to_allocate;
            *next_free_new_block = next_free_block;

            *prev_next_block = new_block;
        }

        *next_block = nullptr;
    }

    return reinterpret_cast<size_t*>(block_to_allocate) + 1;
}


allocator_list::~allocator_list()
{
    logger *log_memory = get_logger();
    memory *allocator = get_memory();

    if (log_memory != nullptr)
    {
        log_memory->log("allocator_list is destroyed", logger::severity::INFORMATION);
    }

    if (allocator != nullptr)
    {
        allocator->deallocate(_allocated_memory);
    }
    else
    {
        ::operator delete(_allocated_memory);
    }
}


void *allocator_list::get_next_free_block(
        void *memory_block) const
{
    auto *size_space = reinterpret_cast<size_t*>(memory_block);

    return *reinterpret_cast<void**>(size_space + 1);
}

size_t allocator_list::get_block_size(
        void const *memory_block) const
{
    return *reinterpret_cast<size_t const*>(memory_block);
}

void *allocator_list::get_first_free_block() const
{
    auto *size_space = reinterpret_cast<size_t*>(_allocated_memory);

    return *reinterpret_cast<void**>(size_space + 1);
}

void *allocator_list::get_first_suitable_block(
        size_t const &size,
        void **prev_block) const
{
    size_t size_to_allocate = size + sizeof(size_t) + sizeof(void*);
    void *current_free_block = get_first_free_block();
    void *prev_free_block = nullptr;

    *prev_block = nullptr;

    while (current_free_block != nullptr)
    {
        size_t size_free_block = get_block_size(current_free_block);

        if (size_free_block >= size_to_allocate)
        {
            *prev_block = prev_free_block;

            return current_free_block;
        }

        prev_free_block = current_free_block;
        current_free_block = get_next_free_block(current_free_block);
    }

    return nullptr;
}

void *allocator_list::get_best_suitable_block(
        size_t const &size,
        void **prev_block) const
{
    size_t size_to_allocate = size + sizeof(size_t) + sizeof(void*);
    void *current_free_block = get_first_free_block();
    void *prev_free_block = nullptr;
    void *block_to_allocate = current_free_block;

    size_t max_size = 0;

    *prev_block = nullptr;

    while (current_free_block != nullptr)
    {
        if (get_block_size(current_free_block) >= size_to_allocate)
        {
            if (get_block_size(current_free_block) > max_size)
            {
                max_size = get_block_size(current_free_block);
                *prev_block = prev_free_block;
                block_to_allocate = current_free_block;
            }
        }

        prev_free_block = current_free_block;
        current_free_block = get_next_free_block(current_free_block);
    }

    return block_to_allocate;
}

void *allocator_list::get_worse_suitable_block(
        size_t const &size,
        void **prev_block) const
{
    size_t size_to_allocate = size + sizeof(size_t) + sizeof(void*);
    void *current_free_block = get_first_free_block();
    void *prev_free_block = nullptr;
    void *block_to_allocate = current_free_block;

    size_t min_size = get_memory_size();

    *prev_block = nullptr;

    while (current_free_block != nullptr)
    {
        if (get_block_size(current_free_block) >= size_to_allocate)
        {
            if (get_block_size(current_free_block) < min_size)
            {
                min_size = get_block_size(current_free_block);
                *prev_block = prev_free_block;
                block_to_allocate = current_free_block;
            }
        }

        prev_free_block = current_free_block;
        current_free_block = get_next_free_block(current_free_block);
    }

    return block_to_allocate;
}


std::string get_bytes_list(void *object)
{
    std::string result;
    auto *size_space = reinterpret_cast<size_t*>(object);
    auto *bytes = reinterpret_cast<unsigned char*>(size_space + 1);

    size_t size_object = *size_space - sizeof(size_t);

    for (int i = 0; i < size_object; i++)
    {
        result += to_string(static_cast<int>(bytes[i])) + " ";
    }

    return result;
}

void allocator_list::deallocate(void *target) const
{
    if (target == nullptr)
    {
        return;
    }

    void *current_free_block = get_first_free_block();
    void *address_deallocated_block = reinterpret_cast<size_t*>(target) - 1;
    void *prev_free_block = nullptr;
    logger *log_memory = get_logger();

    if (log_memory != nullptr)
    {
        log_memory->log("deallocated block (" + to_string(target) + ") : " + get_bytes_list(address_deallocated_block), logger::severity::INFORMATION);
    }

    while (current_free_block != nullptr &&
           reinterpret_cast<char*>(address_deallocated_block) - reinterpret_cast<char*>(current_free_block) > 0)
    {
        prev_free_block = current_free_block;
        current_free_block = get_next_free_block(current_free_block);
    }

    auto *size_space_block = reinterpret_cast<size_t*>(address_deallocated_block);
    auto **next_free_block = reinterpret_cast<void**>(size_space_block + 1);

    *next_free_block = current_free_block;

    if (prev_free_block == nullptr)
    {
        auto *size_space = reinterpret_cast<size_t*>(_allocated_memory);
        void **first_free_block = reinterpret_cast<void**>(size_space + 1);

        *first_free_block = address_deallocated_block;
    }

    else
    {
        auto *size_space_prev_block = reinterpret_cast<size_t*>(prev_free_block);
        void **next_free_block_prev = reinterpret_cast<void**>(size_space_prev_block + 1);

        *next_free_block_prev = address_deallocated_block;
    }

    free_memory();
}


size_t allocator_list::get_memory_size() const
{
    return *reinterpret_cast<size_t*>(_allocated_memory);
}

logger *allocator_list::get_logger() const
{
    auto *size_space = reinterpret_cast<size_t*>(_allocated_memory);
    void **first_free_block = reinterpret_cast<void**>(size_space + 1);
    auto **outer_allocator = reinterpret_cast<memory**>(first_free_block + 1);

    return *reinterpret_cast<logger**>(outer_allocator + 1);
}

memory *allocator_list::get_memory() const
{
    auto *size_space = reinterpret_cast<size_t*>(_allocated_memory);
    void **first_free_block = reinterpret_cast<void**>(size_space + 1);

    return *reinterpret_cast<memory**>(first_free_block + 1);
}

void allocator_list::free_memory() const
{
    void *current_free_block = get_first_free_block();
    void *next_free_block = nullptr;

    while (current_free_block != nullptr)
    {
        next_free_block = get_next_free_block(current_free_block);

        if (reinterpret_cast<char*>(next_free_block) - reinterpret_cast<char*>(current_free_block) == *reinterpret_cast<size_t*>(current_free_block))
        {
            auto *size_space = reinterpret_cast<size_t*>(next_free_block);
            auto **next_block = reinterpret_cast<void**>(size_space + 1);

            *reinterpret_cast<void**>(reinterpret_cast<size_t*>(current_free_block) + 1) = *next_block;
            *reinterpret_cast<size_t*>(current_free_block) = get_block_size(current_free_block) + get_block_size(next_free_block);

            *next_block = nullptr;
            *size_space = 0;
            size_space = nullptr;
        }
        else
        {
            current_free_block = next_free_block;
        }
    }
}