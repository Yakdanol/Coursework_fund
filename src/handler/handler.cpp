#include "handler.h"

type_data_base *handler::_data_base = allocate_data_base(handler::TREE::RED_BLACK_TREE);
logger *handler::_logger = nullptr;

type_data_base *handler::allocate_data_base(TREE tree)
{
    switch (tree)
    {
        case TREE::RED_BLACK_TREE:
            return reinterpret_cast<type_data_base*>(new type_data_base_rb);
        case TREE::AVL_TREE:
            return reinterpret_cast<type_data_base*>(new type_data_base_avl);
        case TREE::SPLAY_TREE:
            return reinterpret_cast<type_data_base*>(new type_data_base_splay);
        default:
            throw std::invalid_argument("ERROR: Invalid number of type tree");
    }
}

type_pool *handler::allocate_pool(TREE tree)
{
    switch (tree)
    {
        case TREE::RED_BLACK_TREE:
            return reinterpret_cast<type_pool*>(new type_pool_rb);
        case TREE::AVL_TREE:
            return reinterpret_cast<type_pool*>(new type_pool_avl);
        case TREE::SPLAY_TREE:
            return reinterpret_cast<type_pool*>(new type_pool_splay);
        default:
            throw std::invalid_argument("ERROR: Invalid number of type tree");
    }
}

type_scheme *handler::allocate_scheme(TREE tree)
{
    switch (tree)
    {
        case TREE::RED_BLACK_TREE:
            return reinterpret_cast<type_scheme*>(new type_scheme_rb);
        case TREE::AVL_TREE:
            return reinterpret_cast<type_scheme*>(new type_scheme_avl);
        case TREE::SPLAY_TREE:
            return reinterpret_cast<type_scheme*>(new type_scheme_splay);
        default:
            throw std::invalid_argument("ERROR: Invalid number of type tree");
    }
}

type_collection *handler::allocate_collection(
        memory *memory_init,
        TREE tree)
{
    switch (tree)
    {
        case TREE::RED_BLACK_TREE:
            return reinterpret_cast<type_collection*>(new type_collection_rb(memory_init));
        case TREE::AVL_TREE:
            return reinterpret_cast<type_collection*>(new type_collection_avl(memory_init));
        case TREE::SPLAY_TREE:
            return reinterpret_cast<type_collection*>(new type_collection_splay(memory_init));
        default:
            throw std::invalid_argument("ERROR: Invalid number of type tree");
    }
}

// разобрать
handler::handler() :
        _next_handler(nullptr)
{
    json_builder *builder = new json_builder_concrete();
    _logger = builder->build("json/config.json");
}

handler::~handler()
{
    delete _next_handler;
}

void handler::set_next(handler *next_handler)
{
    _next_handler = next_handler;
}

void handler::accept_request(const std::string &request)
{
    std::vector<std::string> params = split_by_spaces(delete_spaces(request));

    try
    {
        handle_request(params, std::cin, true);
    }

    catch (const std::exception &ex)
    {
        throw ex;
    }
}

type_data_base *handler::get_instance()
{
    return _data_base;
}

logger *handler::get_logger()
{
    return _logger;
}


// удалить пробелы в начале и конце строки
std::string handler::delete_spaces(const std::string &str)
{
    std::string copy_str = str;

    size_t str_begin = copy_str.find_first_not_of(' ');
    size_t str_end = copy_str.find_last_not_of(' ');
    copy_str.erase(str_end + 1, copy_str.size() - str_end);
    copy_str.erase(0, str_begin);

    return copy_str;
}

// vector отдельных слов (строку делим по словам)
std::vector<std::string> handler::split_by_spaces(const std::string &str)
{
    std::vector<std::string> result;
    std::stringstream stream(str); // создание потока из входной строки
    std::string word;

    while (stream >> word)
    {
        result.push_back(word);
    }

    return result;
}

