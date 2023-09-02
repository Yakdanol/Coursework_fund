#ifndef HANDLE_H
#define HANDLE_H

#include "data_types.h"
#include "memory.h"
#include "allocator_buddies.h"
#include "allocator_descriptor.h"
#include "allocator_list.h"
#include "memory_holder.h"
#include "logger_holder.h"
#include "json_builder.h"
#include "json_builder_concrete.h"
#include "logger_builder.h"

class handler
{

public:

    enum class TREE
    {
        RED_BLACK_TREE,
        AVL_TREE,
        SPLAY_TREE
    };

private:

    static type_data_base *_data_base;
    static logger *_logger;

protected:

    handler *_next_handler;

public:

    class command
    {

    public:

        virtual void execute(
                const std::vector<std::string> &params,
                std::pair<type_pool*, memory*> *pool,
                std::pair<type_scheme *, memory*> *scheme,
                std::pair<type_collection*, std::map<std::string, type_order_collection*>> *collection,
                std::istream &stream,
                bool console) = 0;

        virtual ~command() = default;

    };

public:

    static std::string delete_spaces(const std::string &str);

    static std::vector<std::string> split_by_spaces(const std::string &str);

public:

    handler();

    void set_next(handler *next_handler);

    void accept_request(const std::string &request);

    virtual void handle_request(
            const std::vector<std::string> &params,
            std::istream &stream,
            bool console) = 0;

    static type_data_base *get_instance();

    static logger *get_logger();

    static type_data_base *allocate_data_base(TREE tree = TREE::RED_BLACK_TREE);

    static type_pool *allocate_pool(TREE tree = TREE::RED_BLACK_TREE);

    static type_scheme *allocate_scheme(TREE tree = TREE::RED_BLACK_TREE);

    static type_collection *allocate_collection(
            memory *memory_init,
            TREE tree = TREE::RED_BLACK_TREE);

    virtual ~handler();

};

#endif //HANDLE_H