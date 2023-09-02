#ifndef ALLOCATOR_DESCRIPTOR_H
#define ALLOCATOR_DESCRIPTOR_H

#include "logger.h"
#include "logger_concrete.h"
#include "logger_builder.h"
#include "logger_builder_concrete.h"
#include "memory.h"
#include <iostream>

class allocator_descriptor final : public memory
{

public:

    explicit allocator_descriptor(
            size_t const &size,
            memory *outer_allocator = nullptr,
            logger *logger_allocator = nullptr,
            memory::ALLOCATION_METOD method_allocation = memory::ALLOCATION_METOD::FIRST_SUITABLE);

    void *allocate(size_t const &size) const override;

    void deallocate(void *target) const override;

    allocator_descriptor(allocator_descriptor const&) = delete;

    void operator=(allocator_descriptor const&) = delete;

    ~allocator_descriptor() override;

private:

    void *_allocated_memory;

    memory::ALLOCATION_METOD _method;

    logger *get_logger() const;

    memory *get_memory() const;

    void *get_next_occupied_block(void *memory_block) const;

    size_t get_memory_size() const;

    size_t get_block_size(void const *memory_block) const;

    void *get_first_occupied_block() const;

    void *get_first_suitable_block(
            size_t const &size,
            void **prev_block,
            void **next_block) const;

    void *get_best_suitable_block(
            size_t const &size,
            void **prev_block,
            void **next_block) const;

    void *get_worse_suitable_block(
            size_t const &size,
            void **prev_block,
            void **next_block) const;

};

#endif //ALLOCATOR_DESCRIPTOR_H