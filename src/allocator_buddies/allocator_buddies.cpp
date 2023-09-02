#include <cmath>
#include <sstream>
#include <unistd.h>
#include "allocator_buddies.h"

template <typename T>
std::string to_string(T object)
{
    std::stringstream stream;
    stream << object;

    return stream.str();
}

std::string get_bytes_buddies(void *object)
{
    std::string result;
    auto *bytes = reinterpret_cast<unsigned char*>(object);
    auto *power = reinterpret_cast<short*>(object) - 1;

    size_t size_object = (1 << *power) - 2 * sizeof(short*);

    for (int i = 0; i < size_object; i++)
    {
        result += to_string(static_cast<int>(bytes[i])) + " ";
    }

    return result;
}

size_t get_size_power_2(size_t number)
{
    return 1 << static_cast<size_t>(ceil(log2(number)));
}

short get_power(size_t size)
{
    return static_cast<short>(log2(size));
}

logger *allocator_buddies::get_logger() const
{
    auto *size_space = reinterpret_cast<size_t*>(_allocated_memory);
    auto **first_free_block = reinterpret_cast<void**>(size_space + 1);
    auto **outer_allocator = reinterpret_cast<memory**>(first_free_block + 1);

    return *reinterpret_cast<logger**>(outer_allocator + 1);
}

memory *allocator_buddies::get_memory() const
{
    auto *size_space = reinterpret_cast<size_t*>(_allocated_memory);
    auto **first_free_block = reinterpret_cast<void**>(size_space + 1);

    return *reinterpret_cast<memory**>(first_free_block + 1);
}

allocator_buddies::allocator_buddies(
        size_t const &size,
        logger *logger_allocator,
        memory *outer_allocator)
{
    size_t size_memory = get_size_power_2(size);
    size_t size_to_allocate = size_memory + sizeof(size_t) + sizeof(memory*) + sizeof(logger*) + sizeof(void*);

    if (outer_allocator != nullptr)
    {
        _allocated_memory = outer_allocator->allocate(size_to_allocate);
    }

    else
    {
        _allocated_memory = ::operator new(size_to_allocate);
    }

    auto *space_size_memory = reinterpret_cast<size_t*>(_allocated_memory);
    *space_size_memory = size_memory;

    void **first_free_block = reinterpret_cast<void**>(space_size_memory + 1);

    auto **space_outer_allocator = reinterpret_cast<memory**>(first_free_block + 1);
    *space_outer_allocator = outer_allocator;

    auto **space_logger_memory = reinterpret_cast<logger**>(space_outer_allocator + 1);
    *space_logger_memory = logger_allocator;

    *first_free_block = space_logger_memory + 1;

    auto *space_is_busy = reinterpret_cast<short*>(*first_free_block);
    *space_is_busy = 0;

    auto *space_power = reinterpret_cast<short*>(space_is_busy + 1);
    *space_power = get_power(size_memory);

    auto **prev_block = reinterpret_cast<void**>(space_power + 1);
    *prev_block = nullptr;

    auto **next_block = reinterpret_cast<void**>(prev_block + 1);
    *next_block = nullptr;

    logger *logger_space = *space_logger_memory;

    if (logger_space != nullptr)
    {
        logger_space->log("allocator_buddies is created", logger::severity::INFORMATION);
        logger_space->log("size of memory allocated for allocator_buddies: " + to_string(size_to_allocate), logger::severity::INFORMATION);
    }
}

void *allocator_buddies::get_first_free_block() const
{
    auto *space_size_memory = reinterpret_cast<size_t*>(_allocated_memory);

    return *reinterpret_cast<void**>(space_size_memory + 1);
}

short allocator_buddies::get_power_block(void *block_memory) const
{
    auto *space_is_busy = reinterpret_cast<short*>(block_memory);

    return *reinterpret_cast<short*>(space_is_busy + 1);
}

void *allocator_buddies::get_prev_free_block(void *block_memory) const
{
    auto *space_is_busy = reinterpret_cast<short*>(block_memory);
    auto *space_power = reinterpret_cast<short*>(space_is_busy + 1);

    return *reinterpret_cast<void**>(space_power + 1);
}

void *allocator_buddies::get_next_free_block(void *block_memory) const
{
    auto *space_is_busy = reinterpret_cast<short*>(block_memory);
    auto *space_power = reinterpret_cast<short*>(space_is_busy + 1);
    auto **prev_block = reinterpret_cast<void**>(space_power + 1);

    return *reinterpret_cast<void**>(prev_block + 1);
}

void *allocator_buddies::get_buddies(void *block_memory) const
{
    if (1 << get_power_block(block_memory) == *reinterpret_cast<size_t*>(_allocated_memory))
    {
        return nullptr;
    }

    auto *size_space = reinterpret_cast<size_t*>(_allocated_memory);
    auto **first_block = reinterpret_cast<void**>(size_space + 1);
    auto **space_memory = reinterpret_cast<memory**>(first_block + 1);
    auto **space_logger = reinterpret_cast<logger**>(space_memory + 1);

    void *begin_address = space_logger + 1;

    size_t size_block = 1 << get_power_block(block_memory);
    size_t relative_address = reinterpret_cast<unsigned char*>(block_memory) - reinterpret_cast<unsigned char*>(begin_address);
    size_t result_xor = relative_address ^ size_block;

    return reinterpret_cast<void*>(reinterpret_cast<char*>(begin_address) + result_xor);
}

bool allocator_buddies::is_free_block(void *block_memory) const
{
    return *reinterpret_cast<short*>(block_memory) == 0;
}

void *allocator_buddies::allocate(size_t const &size) const
{
    logger *log_memory = get_logger();
    size_t size_to_allocate = size + 2 * sizeof(short) + 2 * sizeof(void*);
    void *current_free_block = get_first_free_block();

    while (current_free_block != nullptr)
    {
        short power = get_power_block(current_free_block);

        if ((1 << power) >= size_to_allocate)
        {
            void *prev_free_block = get_prev_free_block(current_free_block);
            void *next_free_block = get_next_free_block(current_free_block);

            while (((1 << power) >> 1) > size_to_allocate)
            {
                power--;

                auto *space_is_busy_current = reinterpret_cast<short*>(current_free_block);
                auto *space_power_current = reinterpret_cast<short*>(space_is_busy_current + 1);
                auto **prev_block_current = reinterpret_cast<void**>(space_power_current + 1);
                auto **next_block_current = reinterpret_cast<void**>(prev_block_current + 1);

                void *buddie = reinterpret_cast<void*>(reinterpret_cast<char*>(current_free_block) + (1 << power));

                auto *space_is_busy_buddie = reinterpret_cast<short*>(buddie);
                auto *space_power_buddie = reinterpret_cast<short*>(space_is_busy_buddie + 1);
                auto **prev_block_buddie = reinterpret_cast<void**>(space_power_buddie + 1);
                auto **next_block_buddie = reinterpret_cast<void**>(prev_block_buddie + 1);

                if (next_free_block != nullptr)
                {
                    auto *space_is_busy_next = reinterpret_cast<short*>(next_free_block);
                    auto *space_power_next = reinterpret_cast<short*>(space_is_busy_next + 1);
                    auto **prev_block_next = reinterpret_cast<void**>(space_power_next + 1);

                    *prev_block_next = buddie;
                }

                *space_is_busy_buddie = 0;
                *space_power_buddie = power;
                *prev_block_buddie = current_free_block;
                *next_block_buddie = next_free_block;

                *space_power_current = power;
                *next_block_current = buddie;

                next_free_block = buddie;
            }

            auto *space_is_busy_current = reinterpret_cast<short*>(current_free_block);
            auto *space_power_current = reinterpret_cast<short*>(space_is_busy_current + 1);
            auto **prev_block_current = reinterpret_cast<void**>(space_power_current + 1);
            auto **next_block_current = reinterpret_cast<void**>(prev_block_current + 1);

            *space_is_busy_current = 1;

            if (prev_free_block == nullptr)
            {
                auto *size_space = reinterpret_cast<size_t*>(_allocated_memory);
                auto **first_free_block = reinterpret_cast<void**>(size_space + 1);

                *first_free_block = next_free_block;
            }
            else
            {
                auto *space_is_busy_prev = reinterpret_cast<short*>(prev_free_block);
                auto *space_power_prev = reinterpret_cast<short*>(space_is_busy_prev + 1);
                auto **prev_block_prev = reinterpret_cast<void**>(space_power_prev + 1);
                auto **next_block_prev = reinterpret_cast<void**>(prev_block_prev + 1);

                *next_block_prev = next_free_block;
            }

            if (next_free_block != nullptr)
            {
                auto *space_is_busy_next = reinterpret_cast<short*>(next_free_block);
                auto *space_power_next = reinterpret_cast<short*>(space_is_busy_next + 1);
                auto **prev_block_next = reinterpret_cast<void**>(space_power_next + 1);

                *prev_block_next = prev_free_block;
            }

            if (log_memory != nullptr)
            {
                log_memory->log("Allocate block size: " + to_string(1 << get_power_block(current_free_block)), logger::severity::INFORMATION);
            }

            return space_power_current + 1;
        }

        current_free_block = get_next_free_block(current_free_block);
    }

    if (log_memory != nullptr)
    {
        log_memory->log("Not enough memory to allocate", logger::severity::INFORMATION);
    }

    return nullptr;
}

void allocator_buddies::deallocate(void *target) const
{
    if (target == nullptr)
    {
        return;
    }

    logger *log_memory = get_logger();
    void *current_block = reinterpret_cast<short*>(target) - 2;

    if (log_memory != nullptr)
    {
        log_memory->log("deallocated block (" + to_string(target) + ") : " + get_bytes_buddies(target), logger::severity::INFORMATION);
    }

    void *prev_block = nullptr;
    void *next_block = get_first_free_block();

    while (next_block != nullptr &&
           reinterpret_cast<char*>(current_block) - reinterpret_cast<char*>(next_block) > 0)
    {
        prev_block = next_block;
        next_block = get_next_free_block(next_block);
    }

    auto *space_is_busy_current = reinterpret_cast<short*>(current_block);
    auto *space_power_current = reinterpret_cast<short*>(space_is_busy_current + 1);
    void **prev_block_current = reinterpret_cast<void**>(space_power_current + 1);
    void **next_block_current = reinterpret_cast<void**>(prev_block_current + 1);

    *space_is_busy_current = 0;
    *prev_block_current = prev_block;
    *next_block_current = next_block;

    if (prev_block == nullptr)
    {
        auto *size_space = reinterpret_cast<size_t*>(_allocated_memory);
        auto **first_free_block = reinterpret_cast<void**>(size_space + 1);

        *first_free_block = current_block;
    }

    else
    {
        auto *space_is_busy_prev = reinterpret_cast<short*>(prev_block);
        auto *space_power_prev = reinterpret_cast<short*>(space_is_busy_prev + 1);
        auto **prev_block_prev = reinterpret_cast<void**>(space_power_prev + 1);
        auto **next_block_prev = reinterpret_cast<void**>(prev_block_prev + 1);

        *next_block_prev = current_block;
    }

    if (next_block != nullptr)
    {
        auto *space_is_busy_next = reinterpret_cast<short*>(next_block);
        auto *space_power_next = reinterpret_cast<short*>(space_is_busy_next + 1);
        auto **prev_block_next = reinterpret_cast<void**>(space_power_next + 1);

        *prev_block_next = current_block;
    }

    void *buddie = get_buddies(current_block);

    while (buddie != nullptr &&
           is_free_block(buddie) &&
           get_power_block(buddie) == get_power_block(current_block))
    {
        if (reinterpret_cast<char*>(buddie) - reinterpret_cast<char*>(current_block) < 0)
        {
            void *temp = buddie;
            buddie = current_block;
            current_block = temp;
        }

        auto *space_is_busy_current = reinterpret_cast<short*>(current_block);
        auto *space_power_current = reinterpret_cast<short*>(space_is_busy_current + 1);
        void **prev_block_current = reinterpret_cast<void**>(space_power_current + 1);
        void **next_block_current = reinterpret_cast<void**>(prev_block_current + 1);

        void *next_block_buddie = get_next_free_block(buddie);

        *next_block_current = next_block_buddie;
        (*space_power_current)++;

        if (next_block_buddie != nullptr)
        {
            auto *space_is_busy_next_block_buddie = reinterpret_cast<short*>(next_block_buddie);
            auto *space_power_next_block_buddie = reinterpret_cast<short*>(space_is_busy_next_block_buddie + 1);
            auto **space_prev_block_next_block_buddie = reinterpret_cast<void**>(space_power_next_block_buddie + 1);

            *space_prev_block_next_block_buddie = current_block;
        }

        buddie = get_buddies(current_block);
    }
}

allocator_buddies::~allocator_buddies()
{
    logger *log_memory = get_logger();
    memory *allocator = get_memory();

    if (log_memory != nullptr)
    {
        log_memory->log("allocator_buddies is destroyed", logger::severity::INFORMATION);
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