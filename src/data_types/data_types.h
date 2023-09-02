#ifndef DATA_TYPES_H
#define DATA_TYPES_H

#include <iostream>
#include "red_black_tree.h"
#include "avl_tree.h"
#include "splay_tree.h"
#include "session_data.h"
#include "comparators.h"
#include <map>
#include <memory>


using type_value = std::pair<session_data*, std::map<long long, std::vector<std::pair<std::string, std::string>>>>; // session_data + type_states
using type_states = std::map<long long, std::vector<std::pair<std::string, std::string>>>; // время + состояния полей при изменении


using type_order_collection = red_black_tree
        <
                std::string,
                type_value*,
                compare_str_keys
        >;



using type_collection = associative_container
        <
                Key,
                type_value*
        >;

using type_scheme = associative_container
        <
                std::string,
                std::pair<type_collection*, std::map<std::string, type_order_collection*>>*
        >;

using type_pool = associative_container
        <
                std::string,
                std::pair<type_scheme*, memory*>
        >;

using type_data_base = associative_container
        <
                std::string,
                std::pair<type_pool*, memory*>
        >;



using type_collection_rb = red_black_tree
        <
                Key,
                type_value*,
                compare_data_keys
        >;

using type_scheme_rb = red_black_tree
        <
                std::string,
                std::pair<type_collection_rb*, std::map<std::string, type_order_collection*>>*,
                compare_str_keys
        >;

using type_pool_rb = red_black_tree
        <
                std::string,
                std::pair<type_scheme_rb*, memory*>,
                compare_str_keys
        >;

using type_data_base_rb = red_black_tree
        <
                std::string,
                std::pair<type_pool_rb*, memory*>,
                compare_str_keys
        >;



using type_collection_avl = avl_tree
        <
                Key,
                type_value*,
                compare_data_keys
        >;

using type_scheme_avl = avl_tree
        <
                std::string,
                std::pair<type_collection_avl*, std::map<std::string, type_order_collection*>>*,
                compare_str_keys
        >;

using type_pool_avl = avl_tree
        <
                std::string,
                std::pair<type_scheme_rb*, memory*>,
                compare_str_keys
        >;

using type_data_base_avl = avl_tree
        <
                std::string,
                std::pair<type_pool_rb*, memory*>,
                compare_str_keys
        >;



using type_collection_splay = splay_tree
        <
                Key,
                type_value*,
                compare_data_keys
        >;

using type_scheme_splay = splay_tree
        <
                std::string,
                std::pair<type_collection_splay*, std::map<std::string, type_order_collection*>>*,
                compare_str_keys
        >;

using type_pool_splay = splay_tree
        <
                std::string,
                std::pair<type_scheme_rb*, memory*>,
                compare_str_keys
        >;

using type_data_base_splay = splay_tree
        <
                std::string,
                std::pair<type_pool_rb *, memory*>,
                compare_str_keys
        >;

#endif //DATA_TYPES_H