cmake_minimum_required(VERSION 3.24)
project(coursework)

set(CMAKE_CXX_STANDARD 14)

add_compile_options(-w)

include_directories(src/associative_container)
include_directories(src/binary_search_tree)
include_directories(src/memory)
include_directories(src/logger)
include_directories(src/avl_tree)
include_directories(src/memory_holder)
include_directories(src/red_black_tree)
include_directories(src/allocator_list)
include_directories(src/allocator_descriptor)
include_directories(src/allocator_buddies)
include_directories(src/splay_tree)
include_directories(src/validation_data)
include_directories(src/handler)
include_directories(src/handler_concrete)
include_directories(src/string_flyweight)
include_directories(src/session_data)
include_directories(src/data_types)
include_directories(src/json_builder)
include_directories(src/json_builder_concrete)
include_directories(src/comparators)

add_executable(
        coursework

        src/logger/logger_builder.h
        src/logger/logger_builder.cpp

        src/logger/logger_builder_concrete.h
        src/logger/logger_builder_concrete.cpp

        src/logger/logger.h
        src/logger/logger.cpp

        src/logger/logger_concrete.h
        src/logger/logger_concrete.cpp

        src/logger/logger_holder.h
        src/logger/logger_holder.cpp

        src/memory/memory.h
        src/memory/memory.cpp

        src/allocator_list/allocator_list.h
        src/allocator_list/allocator_list.cpp

        src/allocator_descriptor/allocator_descriptor.h
        src/allocator_descriptor/allocator_descriptor.cpp

        src/allocator_buddies/allocator_buddies.h
        src/allocator_buddies/allocator_buddies.cpp

        src/memory_holder/memory_holder.h
        src/memory_holder/memory_holder.cpp

        src/associative_container/associative_container.h
        src/associative_container/associative_container.cpp

        src/binary_search_tree/binary_search_tree.h

        src/avl_tree/avl_tree.h
        src/avl_tree/avl_tree.cpp

        src/red_black_tree/red_black_tree.h

        src/splay_tree/splay_tree.h

        src/data_types/data_types.h

        src/validation_data/validation_data.h
        src/validation_data/validation_data.cpp

        src/handler/handler.h
        src/handler/handler.cpp

        src/handler_concrete/handler_concrete.h
        src/handler_concrete/handler_concrete.cpp

        src/string_flyweight/string_flyweight.h
        src/string_flyweight/string_flyweight.cpp

        src/session_data/session_data.h
        src/session_data/session_data.cpp

        src/json_builder/json_builder.h
        src/json_builder/json_builder.cpp

        src/json_builder_concrete/json_builder_concrete.cpp
        src/json_builder_concrete/json_builder_concrete.h

        src/main.cpp
        src/comparators/comparators.h)
