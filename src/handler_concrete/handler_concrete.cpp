#include "handler_concrete.h"
#include <functional>


handler_concrete::handler_concrete()
{
    // создаем указатели на объекты всех классов обработчиков
    handler *_run = new handler_run;
    handler *_add_pool = new handler_add_pool;
    handler *_remove_pool = new handler_remove_pool;
    handler *_add_scheme = new handler_add_scheme;
    handler *_remove_scheme = new handler_remove_scheme;
    handler *_add_collection = new handler_add_collection;
    handler *_remove_collection = new handler_remove_collection;
    handler *_add_data = new handler_add_data;
    handler *_get_data = new handler_get_data;
    handler *_get_range = new handler_get_range;
    handler *_update = new handler_update;
    handler *_remove = new handler_remove;

    // устанавливаем связь, формируя цепочку
    set_next(_run);
    _run->set_next(_add_pool);
    _add_pool->set_next(_remove_pool);
    _remove_pool->set_next(_add_scheme);
    _add_scheme->set_next(_remove_scheme);
    _remove_scheme->set_next(_add_collection);
    _add_collection->set_next(_remove_collection);
    _remove_collection->set_next(_add_data);
    _add_data->set_next(_get_data);
    _get_data->set_next(_get_range);
    _get_range->set_next(_update);
    _update->set_next(_remove);
    _remove->set_next(nullptr);
}

void handler_concrete::handle_request(
        const std::vector<std::string> &params,
        std::istream &stream,
        bool console)
{
    try
    {
        _next_handler->handle_request(params, stream, console);
    }

    catch (const std::exception &ex)
    {
        std::cout << ex.what() << std::endl;
    }
}


handler_concrete::~handler_concrete()
{
    handler::get_logger()->log("Deleting handler", logger::severity::INFORMATION);

    auto *data_base = reinterpret_cast<type_data_base_rb*>(handler::get_instance());
    auto iterator_base = data_base->begin_prefix();
    auto data_base_end = data_base->end_prefix();

    // проходимся по базе
    for (iterator_base; iterator_base != data_base_end; ++iterator_base)
    {
        auto end_pool= std::get<2>(*iterator_base).first->end_prefix();
        auto begin_pool = std::get<2>(*iterator_base).first->begin_prefix();

        // проходимся по пулам
        for (auto iterator_pool = begin_pool; iterator_pool != end_pool; ++iterator_pool)
        {
            auto end_scheme = std::get<2>(*iterator_pool).first->end_prefix();
            auto begin_scheme = std::get<2>(*iterator_pool).first->begin_prefix();

            // проходимся по схемам
            for (auto iterator_scheme = begin_scheme; iterator_scheme != end_scheme; ++iterator_scheme)
            {
                auto end_collection = std::get<2>(*iterator_scheme)->first->end_prefix();
                auto begin_collection = std::get<2>(*iterator_scheme)->first->begin_prefix();

                // проходимся по коллекциям
                for (auto iterator_collection = begin_collection; iterator_collection != end_collection; ++iterator_collection)
                {
                    // удаляем память для коллекции и коллекцию
                    delete std::get<2>(*iterator_collection)->first;
                    delete std::get<2>(*iterator_collection);
                }

                // освобождаем ресурсы
                delete std::get<2>(*iterator_scheme)->first;
                delete std::get<2>(*iterator_scheme)->second["SURNAME_STUDENT"];
                delete std::get<2>(*iterator_scheme)->second["NAME_STUDENT"];
                delete std::get<2>(*iterator_scheme)->second["PATRONYMIC_STUDENT"];
                delete std::get<2>(*iterator_scheme)->second["DATE"];
                delete std::get<2>(*iterator_scheme)->second["TIME"];
                delete std::get<2>(*iterator_scheme)->second["MARK"];
                delete std::get<2>(*iterator_scheme)->second["SURNAME_TEACHER"];
                delete std::get<2>(*iterator_scheme)->second["NAME_TEACHER"];
                delete std::get<2>(*iterator_scheme)->second["PATRONYMIC_TEACHER"];
                delete std::get<2>(*iterator_scheme);
                std::get<2>(*iterator_pool).first->remove(std::get<1>(*iterator_scheme));
            }

            // удаляем память и пул
            delete std::get<2>(*iterator_pool).first;
            std::get<2>(*iterator_base).first->remove(std::get<1>(*iterator_pool));
        }

        // удаляем память и базу данных
        delete std::get<2>(*iterator_base).second;
        delete std::get<2>(*iterator_base).first;
        data_base->remove(std::get<1>(*iterator_base));
    }

    delete handler::get_instance();

    handler::get_logger()->log("Handler is deleted", logger::severity::INFORMATION);
}



void handler_run::handle_request(
        const std::vector<std::string> &params,
        std::istream &stream,
        bool console)
{
    if (!params.empty() && params[0] == "START")
    {
        handler::get_logger()->log("START command called", logger::severity::INFORMATION);
        command *execute_run = new run_command(this);

        try
        {
            execute_run->execute(params, nullptr, nullptr, nullptr, stream, console);
        }

        catch (const std::exception &ex)
        {
            handler::get_logger()->log("Exception caught", logger::severity::INFORMATION);
            delete execute_run;
            throw std::invalid_argument(ex.what());
        }

        delete execute_run;

        handler::get_logger()->log("Successful command execution", logger::severity::INFORMATION);
    }

    else
    {
        if (_next_handler != nullptr)
        {
            _next_handler->handle_request(params, stream, console);
        }
    }
}

// инициализация _handler
handler_run::run_command::run_command(
        handler *handler_init) :
        _handler(handler_init)
{

}

// выполнение команд из файла
void handler_run::run_command::execute(
        const std::vector<std::string> &params,
        std::pair<type_pool*, memory*> *pool,
        std::pair<type_scheme*, memory*> *scheme,
        std::pair<type_collection*, std::map<std::string, type_order_collection*>> *collection,
        std::istream &stream,
        bool console)
{
    std::fstream stream_file(params[1]);
    std::string line;

    if (!stream_file.is_open())
    {
        throw std::invalid_argument("ERROR: Invalid file to run commands");
    }

    while (std::getline(stream_file, line))
    {
        std::vector<std::string> params_file_command = split_by_spaces(delete_spaces(line));

        try
        {
            _handler->handle_request(params_file_command, stream_file, false);
        }

        catch (const std::exception &ex)
        {
            throw std::invalid_argument(ex.what());
        }
    }

    stream_file.close();
}


void handler_add_pool::handle_request(
        const std::vector<std::string> &params,
        std::istream &stream,
        bool console)
{
    if (!params.empty() && params[0] == "ADD_POOL")
    {
        handler::get_logger()->log("ADD_POOL command called", logger::severity::INFORMATION);

        type_data_base *data_base = handler::get_instance();

        if (params.size() != 2)
        {
            handler::get_logger()->log("Exception caught", logger::severity::INFORMATION);
            throw std::invalid_argument("ERROR: Invalid arguments to add pool");
        }

        if (data_base->in(params[1]))
        {
            handler::get_logger()->log("Exception caught", logger::severity::INFORMATION);
            throw std::invalid_argument("ERROR: Pool with the same name already exists");
        }

        command *execute_add_pool = new add_pool_command;

        try
        {
            execute_add_pool->execute(params, nullptr, nullptr, nullptr, stream, console);
        }

        catch (const std::exception &ex)
        {
            handler::get_logger()->log("Exception caught", logger::severity::INFORMATION);
            delete execute_add_pool;
            throw std::invalid_argument(ex.what());
        }

        delete execute_add_pool;

        handler::get_logger()->log("Successful command execution", logger::severity::INFORMATION);
    }

    else
    {
        if (_next_handler != nullptr)
        {
            _next_handler->handle_request(params, stream, console);
        }
    }
}

// выполнение команды add_pool
void handler_add_pool::add_pool_command::execute(
        const std::vector<std::string> &params,
        std::pair<type_pool*, memory*> *pool,
        std::pair<type_scheme*, memory*> *scheme,
        std::pair<type_collection*, std::map<std::string, type_order_collection*>> *collection,
        std::istream &stream,
        bool console)
{
    type_data_base *data_base = handler::get_instance();

    if (console)
    {
        int number_allocator = 0;
        std::cout << "Select an allocator: " << std::endl;
        std::cout << "1. Allocator list" << std::endl;
        std::cout << "2. Allocator descriptor" << std::endl;
        std::cout << "3. Allocator buddies" << std::endl;
        std::cout << "Print the number: ";
        std::cin >> number_allocator;

        switch (number_allocator)
        {
            case 1:
                data_base->insert(params[1], std::make_pair(handler::allocate_pool(), new allocator_list(SIZE_TO_ALLOCATOR)));
                break;
            case 2:
                data_base->insert(params[1], std::make_pair(handler::allocate_pool(), new allocator_descriptor(SIZE_TO_ALLOCATOR)));
                break;
            case 3:
                data_base->insert(params[1], std::make_pair(handler::allocate_pool(), new allocator_buddies(SIZE_TO_ALLOCATOR)));
                break;
            default:
                throw std::invalid_argument("ERROR: Invalid number of allocator");
        }
    }

    else
    {
        std::string allocator_params;

        if (stream.eof())
        {
            handler::get_logger()->log("Exception caught", logger::severity::INFORMATION);
            throw std::out_of_range("ERROR: Unable to read allocator name");
        }

        std::getline(stream, allocator_params);
        std::string name_allocator;

        try
        {
            read_arguments(allocator_params, "ALLOCATOR", ":", name_allocator);
        }

        catch (const std::exception &ex)
        {
            handler::get_logger()->log("Exception caught", logger::severity::INFORMATION);
            throw std::invalid_argument(ex.what());
        }

        if (name_allocator == "allocator_list")
        {
            data_base->insert(params[1], std::make_pair(handler::allocate_pool(), new allocator_list(SIZE_TO_ALLOCATOR)));
        }

        else if (name_allocator == "allocator_descriptor")
        {
            data_base->insert(params[1], std::make_pair(handler::allocate_pool(), new allocator_descriptor(SIZE_TO_ALLOCATOR)));
        }

        else if (name_allocator == "allocator_buddies")
        {
            data_base->insert(params[1], std::make_pair(handler::allocate_pool(), new allocator_buddies(SIZE_TO_ALLOCATOR)));
        }

        else
        {
            handler::get_logger()->log("Exception caught", logger::severity::INFORMATION);
            throw std::invalid_argument("ERROR: Invalid name of allocator");
        }
    }
}


// выполнение команды remove_pool
void handler_remove_pool::handle_request(
        const std::vector<std::string> &params,
        std::istream &stream,
        bool console)
{
    if (!params.empty() && params[0] == "REMOVE_POOL")
    {
        handler::get_logger()->log("REMOVE_POOL command called", logger::severity::INFORMATION);

        type_data_base *data_base = handler::get_instance();

        if (params.size() != 2)
        {
            handler::get_logger()->log("Exception caught", logger::severity::INFORMATION);
            throw std::invalid_argument("ERROR: Invalid arguments to remove pool");
        }

        if (!data_base->in(params[1]))
        {
            handler::get_logger()->log("Exception caught", logger::severity::INFORMATION);
            throw std::invalid_argument("ERROR: There is no such pool");
        }

        std::pair<type_pool*, memory*> pool = data_base->get(params[1]);

        command *execute_remove_pool = new remove_pool_command();

        try
        {
            execute_remove_pool->execute(params, &pool, nullptr, nullptr, stream, console);
        }

        catch (const std::exception &ex)
        {
            handler::get_logger()->log("Exception caught", logger::severity::INFORMATION);
            delete execute_remove_pool;
            throw std::invalid_argument(ex.what());
        }

        delete execute_remove_pool;

        handler::get_logger()->log("Successful command execution", logger::severity::INFORMATION);
    }

    else
    {
        if (_next_handler != nullptr)
        {
            _next_handler->handle_request(params, stream, console);
        }
    }
}

// выполнение команды remove_pool
void handler_remove_pool::remove_pool_command::execute(
        const std::vector<std::string> &params,
        std::pair<type_pool*, memory*> *pool,
        std::pair<type_scheme*, memory*> *scheme,
        std::pair<type_collection*, std::map<std::string, type_order_collection*>> *collection,
        std::istream &stream,
        bool console)
{
    auto end_pool = reinterpret_cast<type_pool_rb*>(pool->first)->end_prefix();
    auto begin_pool = reinterpret_cast<type_pool_rb*>(pool->first)->begin_prefix();

    // проходимся по пулу
    for (auto iterator_pool = begin_pool; iterator_pool != end_pool; ++iterator_pool)
    {
        auto end_scheme = std::get<2>(*iterator_pool).first->end_prefix();
        auto begin_scheme = std::get<2>(*iterator_pool).first->begin_prefix();

        // проходимся по схемам
        for (auto iterator_scheme = begin_scheme; iterator_scheme != end_scheme; ++iterator_scheme)
        {
            auto end_collection = std::get<2>(*iterator_scheme)->first->end_prefix();
            auto begin_collection = std::get<2>(*iterator_scheme)->first->begin_prefix();

            // проходимся по коллекциям
            for (auto iterator_collection = begin_collection; iterator_collection != end_collection; ++iterator_collection)
            {
                delete std::get<2>(*iterator_collection)->first;
                delete std::get<2>(*iterator_collection);
            }

            // освобождаем ресурсы
            delete std::get<2>(*iterator_scheme)->first;
            delete std::get<2>(*iterator_scheme)->second["SURNAME_STUDENT"];
            delete std::get<2>(*iterator_scheme)->second["NAME_STUDENT"];
            delete std::get<2>(*iterator_scheme)->second["PATRONYMIC_STUDENT"];
            delete std::get<2>(*iterator_scheme)->second["DATE"];
            delete std::get<2>(*iterator_scheme)->second["TIME"];
            delete std::get<2>(*iterator_scheme)->second["MARK"];
            delete std::get<2>(*iterator_scheme)->second["SURNAME_TEACHER"];
            delete std::get<2>(*iterator_scheme)->second["NAME_TEACHER"];
            delete std::get<2>(*iterator_scheme)->second["PATRONYMIC_TEACHER"];
            delete std::get<2>(*iterator_scheme);
            std::get<2>(*iterator_pool).first->remove(std::get<1>(*iterator_scheme));
        }

        pool->first->remove(std::get<1>(*iterator_pool));
        delete std::get<2>(*iterator_pool).first;
    }

    delete pool->first;
    delete pool->second;
    handler::get_instance()->remove(params[1]);

    pool->first = nullptr;
    pool->second = nullptr;
}


// выполнение команды ADD_SCHEME
void handler_add_scheme::handle_request(
        const std::vector<std::string> &params,
        std::istream &stream,
        bool console)
{
    if (!params.empty() && params[0] == "ADD_SCHEME")
    {
        handler::get_logger()->log("ADD_SCHEME command called", logger::severity::INFORMATION);

        type_data_base *data_base = handler::get_instance();

        if (params.size() != 3)
        {
            handler::get_logger()->log("Exception caught", logger::severity::INFORMATION);
            throw std::invalid_argument("ERROR: Invalid arguments to add data_scheme");
        }

        if (!data_base->in(params[1]))
        {
            handler::get_logger()->log("Exception caught", logger::severity::INFORMATION);
            throw std::invalid_argument("ERROR: There is no such pool");
        }

        std::pair<type_pool*, memory*> pool = data_base->get(params[1]);

        if (pool.first->in(params[2]))
        {
            handler::get_logger()->log("Exception caught", logger::severity::INFORMATION);
            throw std::invalid_argument("Scheme with the same name already exists");
        }

        command *execute_add_scheme = new add_scheme_command;

        try
        {
            execute_add_scheme->execute(params, &pool, nullptr, nullptr, stream, console);
        }

        catch (const std::exception &ex)
        {
            handler::get_logger()->log("Exception caught", logger::severity::INFORMATION);
            delete execute_add_scheme;
            throw std::invalid_argument(ex.what());
        }

        delete execute_add_scheme;

        handler::get_logger()->log("Successful command execution", logger::severity::INFORMATION);
    }
    else
    {
        if (_next_handler != nullptr)
        {
            _next_handler->handle_request(params, stream, console);
        }
    }
}

// выполнение команды add_scheme
void handler_add_scheme::add_scheme_command::execute(
        const std::vector<std::string> &params,
        std::pair<type_pool*, memory*> *pool,
        std::pair<type_scheme*, memory*> *scheme,
        std::pair<type_collection*, std::map<std::string, type_order_collection*>> *collection,
        std::istream &stream,
        bool console)
{
    (*pool).first->insert(params[2], std::make_pair(handler::allocate_scheme(), (*pool).second));
}

// выполнение команды REMOVE_SCHEME
void handler_remove_scheme::handle_request(
        const std::vector<std::string> &params,
        std::istream &stream,
        bool console)
{
    if (!params.empty() && params[0] == "REMOVE_SCHEME")
    {
        handler::get_logger()->log("REMOVE_SCHEME command called", logger::severity::INFORMATION);

        type_data_base *data_base = handler::get_instance();

        if (params.size() != 3)
        {
            handler::get_logger()->log("Exception caught", logger::severity::INFORMATION);
            throw std::invalid_argument("ERROR: Invalid arguments to remove data_scheme");
        }

        if (!data_base->in(params[1]))
        {
            handler::get_logger()->log("Exception caught", logger::severity::INFORMATION);
            throw std::invalid_argument("ERROR: There is no such pool");
        }

        std::pair<type_pool*, memory*> pool = data_base->get(params[1]);

        if (!pool.first->in(params[2]))
        {
            handler::get_logger()->log("Exception caught", logger::severity::INFORMATION);
            throw std::invalid_argument("ERROR: There is no such data_scheme");
        }

        std::pair<type_scheme*, memory*> scheme = pool.first->get(params[2]);

        command *execute_remove_scheme = new remove_scheme_command;

        try
        {
            execute_remove_scheme->execute(params, &pool, &scheme, nullptr, stream, console);
        }

        catch (const std::exception &ex)
        {
            handler::get_logger()->log("Exception caught", logger::severity::INFORMATION);
            delete execute_remove_scheme;
            throw std::invalid_argument(ex.what());
        }

        delete execute_remove_scheme;

        handler::get_logger()->log("Successful command execution", logger::severity::INFORMATION);
    }

    else
    {
        if (_next_handler != nullptr)
        {
            _next_handler->handle_request(params, stream, console);
        }
    }
}

void handler_remove_scheme::remove_scheme_command::execute(
        const std::vector<std::string> &params,
        std::pair<type_pool*, memory*> *pool,
        std::pair<type_scheme*, memory*> *scheme,
        std::pair<type_collection*, std::map<std::string, type_order_collection*>> *collection,
        std::istream &stream,
        bool console)
{
    auto end_scheme = reinterpret_cast<type_scheme_rb*>(scheme->first)->end_prefix();
    auto begin_scheme = reinterpret_cast<type_scheme_rb*>(scheme->first)->begin_prefix();

    // проходимся по схеме
    for (auto iterator_scheme = begin_scheme; iterator_scheme != end_scheme; ++iterator_scheme)
    {
        auto end_collection = std::get<2>(*iterator_scheme)->first->end_prefix();
        auto begin_collection = std::get<2>(*iterator_scheme)->first->begin_prefix();

        // проходимся по коллекциям
        for (auto iterator_collection = begin_collection; iterator_collection != end_collection; ++iterator_collection)
        {
            delete std::get<2>(*iterator_collection)->first;
            delete std::get<2>(*iterator_collection);
        }

        // освобождаем ресурсы
        delete std::get<2>(*iterator_scheme)->first;
        delete std::get<2>(*iterator_scheme)->second["SURNAME_STUDENT"];
        delete std::get<2>(*iterator_scheme)->second["NAME_STUDENT"];
        delete std::get<2>(*iterator_scheme)->second["PATRONYMIC_STUDENT"];
        delete std::get<2>(*iterator_scheme)->second["DATE"];
        delete std::get<2>(*iterator_scheme)->second["TIME"];
        delete std::get<2>(*iterator_scheme)->second["MARK"];
        delete std::get<2>(*iterator_scheme)->second["SURNAME_TEACHER"];
        delete std::get<2>(*iterator_scheme)->second["NAME_TEACHER"];
        delete std::get<2>(*iterator_scheme)->second["PATRONYMIC_TEACHER"];
        delete std::get<2>(*iterator_scheme);
        scheme->first->remove(std::get<1>(*iterator_scheme));
    }

    delete scheme->first;
    pool->first->remove(params[2]);
    scheme->first = nullptr;
}


// выполнение команды ADD_COLLECTION
void handler_add_collection::handle_request(
        const std::vector<std::string> &params,
        std::istream &stream,
        bool console)
{
    if (!params.empty() && params[0] == "ADD_COLLECTION")
    {
        handler::get_logger()->log("ADD_COLLECTION command called", logger::severity::INFORMATION);

        type_data_base *data_base = handler::get_instance();

        if (params.size() != 4)
        {
            handler::get_logger()->log("Exception caught", logger::severity::INFORMATION);
            throw std::invalid_argument("ERROR: Invalid arguments to add collection");
        }

        if (!data_base->in(params[1]))
        {
            handler::get_logger()->log("Exception caught", logger::severity::INFORMATION);
            throw std::invalid_argument("ERROR: There is no such pool");
        }

        std::pair<type_pool*, memory*> pool = data_base->get(params[1]);

        if (!pool.first->in(params[2]))
        {
            handler::get_logger()->log("Exception caught", logger::severity::INFORMATION);
            throw std::invalid_argument("ERROR: There is no such data_scheme");
        }

        std::pair<type_scheme*, memory*> scheme = pool.first->get(params[2]);

        if (scheme.first->in(params[3]))
        {
            handler::get_logger()->log("Exception caught", logger::severity::INFORMATION);
            throw std::invalid_argument("Collection with the same name already exists");
        }

        command *execute_add_collection = new add_collection_command;

        try
        {
            execute_add_collection->execute(params, &pool, &scheme, nullptr, stream, console);
        }

        catch (const std::exception &ex)
        {
            handler::get_logger()->log("Exception caught", logger::severity::INFORMATION);
            delete execute_add_collection;
            throw std::invalid_argument(ex.what());
        }

        delete execute_add_collection;

        handler::get_logger()->log("Successful command execution", logger::severity::INFORMATION);
    }

    else
    {
        if (_next_handler != nullptr)
        {
            _next_handler->handle_request(params, stream, console);
        }
    }
}

// добавляем коллекцию
void handler_add_collection::add_collection_command::execute(
        const std::vector<std::string> &params,
        std::pair<type_pool*, memory*> *pool,
        std::pair<type_scheme*, memory*> *scheme,
        std::pair<type_collection*, std::map<std::string, type_order_collection*>> *collection,
        std::istream &stream,
        bool console)
{
    type_data_base *data_base = handler::get_instance();

    std::map<std::string, type_order_collection*> order_collections;
    // заполняем данные
    order_collections.insert({"SURNAME_STUDENT", new type_order_collection()});
    order_collections.insert({"NAME_STUDENT", new type_order_collection()});
    order_collections.insert({"PATRONYMIC_STUDENT", new type_order_collection()});
    order_collections.insert({"DATE", new type_order_collection()});
    order_collections.insert({"TIME", new type_order_collection()});
    order_collections.insert({"MARK", new type_order_collection()});
    order_collections.insert({"SURNAME_TEACHER", new type_order_collection()});
    order_collections.insert({"NAME_TEACHER", new type_order_collection()});
    order_collections.insert({"PATRONYMIC_TEACHER", new type_order_collection()});

    if (console)
    {
        int number_tree = 0;

        std::cout << "Select the type of tree for the collection: " << std::endl;
        std::cout << "1. Red-black tree" << std::endl;
        std::cout << "2. Splay tree" << std::endl;
        std::cout << "3. AVL tree" << std::endl;
        std::cout << "Print the number: ";
        std::cin >> number_tree;
        std::cout << std::endl;

        try
        {
            scheme->first->insert(params[3], new std::pair<type_collection*, std::map<std::string,
                    type_order_collection*>>(std::make_pair(handler::allocate_collection(pool->second,
                    static_cast<handler::TREE>(number_tree - 1)), order_collections)));
        }

        catch (const std::exception &ex)
        {
            throw std::invalid_argument(ex.what());
        }
    }

    else
    {
        std::string tree_params;

        if (stream.eof())
        {
            throw std::out_of_range("ERROR: Unable to read type tree");
        }

        std::getline(stream, tree_params);
        std::string name_type;

        try
        {
            read_arguments(tree_params, "TREE", ":", name_type);
        }

        catch (const std::exception &ex)
        {
            throw std::invalid_argument(ex.what());
        }


        if (name_type == "RED_BLACK_TREE")
        {
            scheme->first->insert(params[3], new std::pair<type_collection*, std::map<std::string, type_order_collection*>>(std::make_pair(handler::allocate_collection(pool->second, handler::TREE::RED_BLACK_TREE), order_collections)));
        }

        else if (name_type == "AVL_TREE")
        {
            scheme->first->insert(params[3], new std::pair<type_collection*, std::map<std::string, type_order_collection*>>(std::make_pair(handler::allocate_collection(pool->second, handler::TREE::AVL_TREE), order_collections)));
        }

        else if (name_type == "SPLAY_TREE")
        {
            scheme->first->insert(params[3], new std::pair<type_collection*, std::map<std::string, type_order_collection*>>(std::make_pair(handler::allocate_collection(pool->second, handler::TREE::SPLAY_TREE), order_collections)));
        }

        else
        {
            throw std::invalid_argument("ERROR: Invalid name of type tree");
        }
    }
}


// выполнение команды REMOVE_COLLECTION
void handler_remove_collection::handle_request(
        const std::vector<std::string> &params,
        std::istream &stream,
        bool console)
{
    if (!params.empty() && params[0] == "REMOVE_COLLECTION")
    {
        handler::get_logger()->log("REMOVE_COLLECTION command called", logger::severity::INFORMATION);

        type_data_base *data_base = handler::get_instance();

        if (params.size() != 4)
        {
            handler::get_logger()->log("Exception caught", logger::severity::INFORMATION);
            throw std::invalid_argument("ERROR: Invalid arguments to remove collection");
        }

        if (!data_base->in(params[1]))
        {
            handler::get_logger()->log("Exception caught", logger::severity::INFORMATION);
            throw std::invalid_argument("ERROR: There is no such pool");
        }

        std::pair<type_pool*, memory*> pool = data_base->get(params[1]);

        if (!pool.first->in(params[2]))
        {
            handler::get_logger()->log("Exception caught", logger::severity::INFORMATION);
            throw std::invalid_argument("ERROR: There is no such data_scheme");
        }

        std::pair<type_scheme*, memory*> scheme = pool.first->get(params[2]);

        if (!scheme.first->in(params[3]))
        {
            handler::get_logger()->log("Exception caught", logger::severity::INFORMATION);
            throw std::invalid_argument("ERROR: There is no such collection");
        }

        std::pair<type_collection*, std::map<std::string, type_order_collection*>> *collection = scheme.first->get(params[3]);

        command *execute_remove_collection = new remove_collection_command;

        try
        {
            execute_remove_collection->execute(params, &pool, &scheme, collection, stream, console);
        }

        catch (const std::exception &ex)
        {
            handler::get_logger()->log("Exception caught", logger::severity::INFORMATION);
            delete execute_remove_collection;
            throw std::invalid_argument(ex.what());
        }

        delete execute_remove_collection;

        handler::get_logger()->log("Successful command execution", logger::severity::INFORMATION);
    }

    else
    {
        if (_next_handler != nullptr)
        {
            _next_handler->handle_request(params, stream, console);
        }
    }
}

void handler_remove_collection::remove_collection_command::execute(
        const std::vector<std::string> &params,
        std::pair<type_pool*, memory*> *pool,
        std::pair<type_scheme*, memory*> *scheme,
        std::pair<type_collection*, std::map<std::string, type_order_collection*>> *collection,
        std::istream &stream,
        bool console)
{

    auto end_collection = reinterpret_cast<type_collection_rb*>(scheme->first->get(params[3])->first)->end_prefix();
    auto begin_collection = reinterpret_cast<type_collection_rb*>((*scheme).first->get(params[3])->first)->begin_prefix();

    for (auto iterator_collection = begin_collection; iterator_collection != end_collection; ++iterator_collection)
    {
        delete std::get<2>(*iterator_collection)->first;
        delete std::get<2>(*iterator_collection);
    }

    // удаляем данные
    delete collection->first;
    delete collection->second["SURNAME_STUDENT"];
    delete collection->second["NAME_STUDENT"];
    delete collection->second["PATRONYMIC_STUDENT"];
    delete collection->second["DATE"];
    delete collection->second["TIME"];
    delete collection->second["MARK"];
    delete collection->second["SURNAME_TEACHER"];
    delete collection->second["NAME_TEACHER"];
    delete collection->second["PATRONYMIC_TEACHER"];
    delete collection;

    scheme->first->remove(params[3]);

    collection = nullptr;
}


// добавление данных в созданный pool_1 scheme_1 collection_1
// команда ADD pool_1 scheme_1 collection_1
// *** данные
void handler_add_data::handle_request(
        const std::vector<std::string> &params,
        std::istream &stream,
        bool console)
{
    if (!params.empty() && params[0] == "ADD")
    {
        handler::get_logger()->log("ADD command called", logger::severity::INFORMATION);

        type_data_base *data_base = handler::get_instance();

        if (params.size() != 4)
        {
            handler::get_logger()->log("Exception caught", logger::severity::INFORMATION);
            throw std::invalid_argument("ERROR: Invalid arguments to add");
        }

        if (!data_base->in(params[1]))
        {
            handler::get_logger()->log("Exception caught", logger::severity::INFORMATION);
            throw std::invalid_argument("ERROR: There is no such pool");
        }

        std::pair<type_pool*, memory*> pool = data_base->get(params[1]);

        if (!pool.first->in(params[2]))
        {
            handler::get_logger()->log("Exception caught", logger::severity::INFORMATION);
            throw std::invalid_argument("ERROR: There is no such data_scheme");
        }

        std::pair<type_scheme*, memory*> scheme = pool.first->get(params[2]);

        if (!scheme.first->in(params[3]))
        {
            handler::get_logger()->log("Exception caught", logger::severity::INFORMATION);
            throw std::invalid_argument("ERROR: There is no such collection");
        }

        std::pair<type_collection*, std::map<std::string, type_order_collection*>> *collection = scheme.first->get(params[3]);

        command *execute_add = new add_data_command;

        try
        {
            execute_add->execute(params, &pool, &scheme, collection, stream, console);
        }

        catch (const std::exception &ex)
        {
            handler::get_logger()->log("Exception caught", logger::severity::INFORMATION);
            delete execute_add;
            throw std::invalid_argument(ex.what());
        }

        delete execute_add;

        handler::get_logger()->log("Successful command execution", logger::severity::INFORMATION);
    }

    else
    {
        if (_next_handler != nullptr)
        {
            _next_handler->handle_request(params, stream, console);
        }
    }
}

// добавление новых записей
void handler_add_data::add_data_command::execute(
        const std::vector<std::string> &params,
        std::pair<type_pool*, memory*> *pool,
        std::pair<type_scheme*, memory*> *scheme,
        std::pair<type_collection*, std::map<std::string, type_order_collection*>> *collection,
        std::istream &stream,
        bool console)
{
    size_t id_session = 0;
    size_t id_student = 0;
    std:: string reporting_form;
    std:: string name_subject;

    session_data *to_insert = nullptr;

    try
    {
        if (console)
        {
            try
            {
                read_key_session_data_from_console(id_session, id_student, reporting_form, name_subject);
                read_session_data_from_console(to_insert, reporting_form);
            }

            catch (const std::exception &ex)
            {
                throw std::invalid_argument(ex.what());
            }

            if (collection->first->in(Key{id_session, id_student, reporting_form, name_subject}))
            {
                throw std::invalid_argument("ERROR: The same session data already exists");
            }
        }

        else
        {
            try
            {
                read_key_session_data_from_file(id_session, id_student, reporting_form, name_subject, stream);
                read_session_data_from_file(to_insert, stream, reporting_form);
            }

            catch (const std::exception &ex)
            {
                throw std::invalid_argument(ex.what());
            }

            if (collection->first->in(Key{id_session, id_student, reporting_form, name_subject}))
            {
                throw std::invalid_argument("ERROR: The same session data already exists");
            }
        }
    }

    catch (const std::exception &ex)
    {
        throw std::invalid_argument(ex.what());
    }

    // заполняем коллекцию
    auto *to_insert_value = new type_value(std::make_pair(to_insert, type_states()));
    collection->first->insert(Key{id_session, id_student, reporting_form, name_subject}, to_insert_value);
    collection->second["SURNAME_STUDENT"]->insert(to_insert->get_surname_student()->get_value(), to_insert_value);
    collection->second["NAME_STUDENT"]->insert(to_insert->get_name_student()->get_value(), to_insert_value);
    collection->second["PATRONYMIC_STUDENT"]->insert(to_insert->get_patronymic_student()->get_value(), to_insert_value);
    collection->second["DATE"]->insert(to_insert->get_date_event()->get_value(), to_insert_value);
    collection->second["TIME"]->insert(to_insert->get_time_event()->get_value(), to_insert_value);
    collection->second["MARK"]->insert(to_insert->get_mark()->get_value(), to_insert_value);
    collection->second["SURNAME_TEACHER"]->insert(to_insert->get_surname_teacher()->get_value(), to_insert_value);
    collection->second["NAME_TEACHER"]->insert(to_insert->get_name_teacher()->get_value(), to_insert_value);
    collection->second["PATRONYMIC_TEACHER"]->insert(to_insert->get_patronymic_teacher()->get_value(), to_insert_value);
}


// команда GET pool_1 scheme_1 collection_1
void handler_get_data::handle_request(
        const std::vector<std::string> &params,
        std::istream &stream,
        bool console)
{
    if (!params.empty() && params[0] == "GET")
    {
        handler::get_logger()->log("GET command called", logger::severity::INFORMATION);

        type_data_base *data_base = handler::get_instance();

        if (params.size() != 4)
        {
            handler::get_logger()->log("Exception caught", logger::severity::INFORMATION);
            throw std::invalid_argument("ERROR: Invalid arguments to add collection");
        }

        if (!data_base->in(params[1]))
        {
            handler::get_logger()->log("Exception caught", logger::severity::INFORMATION);
            throw std::invalid_argument("ERROR: There is no such pool");
        }

        std::pair<type_pool*, memory*> pool = data_base->get(params[1]);

        if (!pool.first->in(params[2]))
        {
            handler::get_logger()->log("Exception caught", logger::severity::INFORMATION);
            throw std::invalid_argument("ERROR: There is no such data_scheme");
        }

        std::pair<type_scheme*, memory*> scheme = pool.first->get(params[2]);

        if (!scheme.first->in(params[3]))
        {
            handler::get_logger()->log("Exception caught", logger::severity::INFORMATION);
            throw std::invalid_argument("ERROR: There is no such collection");
        }

        std::pair<type_collection*, std::map<std::string, type_order_collection*>> *collection = scheme.first->get(params[3]);

        command *execute_get = new get_data_command;

        try
        {
            execute_get->execute(params, &pool, &scheme, collection, stream, console);
        }

        catch (const std::exception &ex)
        {
            handler::get_logger()->log("Exception caught", logger::severity::INFORMATION);
            delete execute_get;
            throw std::invalid_argument(ex.what());
        }

        delete execute_get;

        handler::get_logger()->log("Successful command execution", logger::severity::INFORMATION);
    }

    else
    {
        if (_next_handler != nullptr)
        {
            _next_handler->handle_request(params, stream, console);
        }
    }
}


// сформировать объект session_data на определенный момент времени
session_data *get_state(
        const std::string &datetime,
        type_value *states)
{
    // объявляем новый элемент session_data и заполняем данными из states
    auto *temp = new session_data();
    temp->set_surname_student(states->first->get_surname_student()->get_value());
    temp->set_name_student(states->first->get_name_student()->get_value());
    temp->set_patronymic_student(states->first->get_patronymic_student()->get_value());
    temp->set_date_event(states->first->get_date_event()->get_value());
    temp->set_time_event(states->first->get_time_event()->get_value());
    temp->set_mark(states->first->get_mark()->get_value());
    temp->set_surname_teacher(states->first->get_surname_teacher()->get_value());
    temp->set_name_teacher(states->first->get_name_teacher()->get_value());
    temp->set_patronymic_teacher(states->first->get_patronymic_teacher()->get_value());

    if (datetime.empty())
    {
        if (!states->second.empty())
        {
            for (const auto &item: states->second)
            {
                for (const auto &item_states: states->second[item.first])
                {
                    if (item_states.first == "SURNAME_STUDENT")
                    {
                        temp->set_surname_student(item_states.second);
                    }

                    else if (item_states.first == "NAME_STUDENT")
                    {
                        temp->set_name_student(item_states.second);
                    }

                    else if (item_states.first == "PATRONYMIC_STUDENT")
                    {
                        temp->set_patronymic_student(item_states.second);
                    }

                    else if (item_states.first == "DATE")
                    {
                        temp->set_date_event(item_states.second);
                    }

                    else if (item_states.first == "TIME")
                    {
                        temp->set_time_event(item_states.second);
                    }

                    else if (item_states.first == "MARK")
                    {
                        temp->set_mark(item_states.second);
                    }

                    else if (item_states.first == "SURNAME_TEACHER")
                    {
                        temp->set_surname_teacher(item_states.second);
                    }

                    else if (item_states.first == "NAME_TEACHER")
                    {
                        temp->set_name_teacher(item_states.second);
                    }

                    else if (item_states.first == "PATRONYMIC_TEACHER")
                    {
                        temp->set_patronymic_teacher(item_states.second);
                    }
                }
            }
        }

        return temp;
    }

    long long time = get_time(datetime); // datetime в секундах

    if (!states->second.empty())
    {
        for (const auto &item: states->second)
        {
            if (time > item.first)
            {
                for (const auto &item_states: states->second[item.first])
                {
                    if (item_states.first == "SURNAME_STUDENT")
                    {
                        temp->set_surname_student(item_states.second);
                    }

                    else if (item_states.first == "NAME_STUDENT")
                    {
                        temp->set_name_student(item_states.second);
                    }

                    else if (item_states.first == "PATRONYMIC_STUDENT")
                    {
                        temp->set_patronymic_student(item_states.second);
                    }

                    else if (item_states.first == "DATE")
                    {
                        temp->set_date_event(item_states.second);
                    }

                    else if (item_states.first == "TIME")
                    {
                        temp->set_time_event(item_states.second);
                    }

                    else if (item_states.first == "MARK")
                    {
                        temp->set_mark(item_states.second);
                    }

                    else if (item_states.first == "SURNAME_TEACHER")
                    {
                        temp->set_surname_teacher(item_states.second);
                    }

                    else if (item_states.first == "NAME_TEACHER")
                    {
                        temp->set_name_teacher(item_states.second);
                    }

                    else if (item_states.first == "PATRONYMIC_TEACHER")
                    {
                        temp->set_patronymic_teacher(item_states.second);
                    }
                }
            }
        }
    }

    return temp;
}

// нахождение записей
void handler_get_data::get_data_command::execute(
        const std::vector<std::string> &params,
        std::pair<type_pool*, memory*> *pool,
        std::pair<type_scheme*, memory*> *scheme,
        std::pair<type_collection*, std::map<std::string, type_order_collection*>> *collection,
        std::istream &stream,
        bool console)
{
    size_t id_session = 0;
    size_t id_student = 0;
    std:: string reporting_form;
    std:: string name_subject;
    type_value *get_target = nullptr;

    std::function<bool(const std::string&)> check_functions[9] = {
            check_surname_student,
            check_name_student,
            check_patronymic_student,
            check_date_event,
            check_time_event,
            check_mark,
            check_surname_teacher,
            check_name_teacher,
            check_patronymic_teacher
    };

    std::string names_field[9] = {
            "SURNAME_STUDENT",
            "NAME_STUDENT",
            "PATRONYMIC_STUDENT",
            "DATE",
            "TIME",
            "MARK",
            "SURNAME_TEACHER",
            "NAME_TEACHER",
            "PATRONYMIC_TEACHER"
    };

    try
    {
        if (console)
        {
            int number_field = 0;
            std::string field_value;

            std::cout << "Choose field to be searched: " << std::endl;
            std::cout << "1. Id_session, id_student, reporting_form and name_subject" << std::endl;
            std::cout << "2. Surname student" << std::endl;
            std::cout << "3. Name student" << std::endl;
            std::cout << "4. Patronymic student" << std::endl;
            std::cout << "5. Date" << std::endl;
            std::cout << "6. Time" << std::endl;
            std::cout << "7. Mark" << std::endl;
            std::cout << "8. Surname teacher" << std::endl;
            std::cout << "9. Name teacher" << std::endl;
            std::cout << "10. Patronymic teacher" << std::endl;
            std::cout << "Number field: ";
            std::cin >> number_field;

            // поиск по ключу (id_session, id_student, reporting_form and name_subject)
            if (number_field == 1)
            {
                try
                {
                    read_key_session_data_from_console(id_session, id_student, reporting_form, name_subject);
                }

                catch (const std::exception &ex)
                {
                    throw std::invalid_argument(ex.what());
                }

                if (!(collection->first->in(Key{id_session, id_student, reporting_form, name_subject})))
                {
                    throw std::invalid_argument("ERROR: There is no such session_data");
                }

                get_target = collection->first->get(Key{id_session, id_student, reporting_form, name_subject});
            }

            // по остальным полям
            else if (number_field >= 2 && number_field <= 10)
            {
                std::cout << "Print " + names_field[number_field - 2] + ": ";
                std::cin >> field_value;

                if ((number_field - 2) != 5)
                {
                    if (!check_functions[number_field - 2](field_value))
                    {
                        throw std::invalid_argument("ERROR: Invalid field value");
                    }
                }

                // валидация
                if ((number_field - 2) == 5)
                {

                    if (!check_id(field_value))
                    {
                        throw std::invalid_argument("ERROR: Invalid field value (mark)");
                    }

                    int check_number = std::stoi(field_value);
                    if (((reporting_form == "Coursework" || reporting_form == "Exam") &&
                    (check_number < 2 || check_number > 5)) ||
                    ((reporting_form == "Test") && (check_number < 0 || check_number > 1)))
                    {
                        throw std::invalid_argument("ERROR: Invalid field value (mark)");
                    }
                }

                if (collection->second[names_field[number_field - 2]]->in(field_value))
                {
                    get_target = collection->second[names_field[number_field - 2]]->get(field_value);
                }

                else
                {
                    throw std::invalid_argument("ERROR: There is no such session_data");
                }
            }

            else
            {
                throw std::invalid_argument("ERROR: Invalid number field");
            }
        }

        else
        {
            std::string name_field;
            std::string value_field;
            std::string line;

            if (stream.eof())
            {
                throw std::invalid_argument("ERROR: End of file");
            }

            std::getline(stream, name_field);
            name_field = handler::delete_spaces(name_field);

            // если запрос по ключу
            if (name_field == "ID_SESSION_ID_STUDENT_REPORTING_FORM_NAME_SUBJECT")
            {
                try
                {
                    read_key_session_data_from_file(id_session, id_student, reporting_form, name_subject, stream);
                }

                catch (const std::exception &ex)
                {
                    throw std::invalid_argument(ex.what());
                }

                if (!(collection->first->in(Key{id_session, id_student, reporting_form, name_subject})))
                {
                    throw std::invalid_argument("Invalid argument key");
                }

                get_target = collection->first->get(Key{id_session, id_student, reporting_form, name_subject});
            }

            // если запрос по полям
            else if (name_field == "SURNAME_STUDENT" ||
                     name_field == "NAME_STUDENT" ||
                     name_field == "PATRONYMIC_STUDENT" ||
                     name_field == "DATE" ||
                     name_field == "TIME" ||
                     name_field == "MARK" ||
                     name_field == "SURNAME_TEACHER" ||
                     name_field == "NAME_TEACHER" ||
                     name_field == "PATRONYMIC_TEACHER")
            {
                if (stream.eof())
                {
                    throw std::invalid_argument("ERROR: End of file");
                }

                std::getline(stream, line);

                try
                {
                    read_arguments(line, name_field, ":", value_field);
                }

                catch (const std::exception &ex)
                {
                    throw std::invalid_argument(ex.what());
                }


                if (collection->second[name_field]->in(value_field))
                {
                    get_target = collection->second[name_field]->get(value_field);
                }

                else
                {
                    throw std::invalid_argument("ERROR: There is no such session_data");
                }
            }

            else
            {
                throw std::invalid_argument("ERROR: Invalid name field");
            }
        }
    }

    catch (const std::exception &ex)
    {
        throw std::invalid_argument(ex.what());
    }


    if (console)
    {
        session_data *temp = nullptr;
        std::vector<std::pair<std::string, std::string>> last_item;
        std::string datetime;
        long long _datetime = 0;
        int choice = 0;

        std::cout << "At what point in time do you want to receive data?" << std::endl;
        std::cout << "1. Current point in time" << std::endl;
        std::cout << "2. Set point in time" << std::endl;
        std::cout << "Print the number: ";
        std::cin >> choice;

        switch (choice)
        {
            case 1:
                temp = get_state("", get_target);
                std::cout << std::endl << "FOUND USER:" << std::endl << std::endl;
                output_session_data(temp);
                std::cout << std::endl;
                delete temp;

                break;
            case 2:
                std::cout << "Put the datetime in format %d/%m/%y-%h:%m:%s: ";
                std::cin >> datetime;

                _datetime = get_time(datetime); // кол-во секунд
                datetime = handler::delete_spaces(datetime);

                if (check_date_event(datetime))
                {
                    temp = get_state(datetime, get_target);
                    std::cout << std::endl << "FOUND USER AT THE POINT TIME " + datetime + ":" << std::endl << std::endl;
                    output_session_data(temp);
                    std::cout << std::endl;
                    delete temp;
                }

                else
                {
                    throw std::invalid_argument("ERROR: Invalid format datetime");
                }

                break;
            default:
                throw std::invalid_argument("ERROR: Invalid number to get item");
        }
    }

    else
    {
        std::string state;

        if (stream.eof())
        {
            throw std::invalid_argument("ERROR: File end");
        }

        std::getline(stream, state);
        state = handler::delete_spaces(state);

        if (state == "CURRENT")
        {
            session_data *temp = get_state("", get_target);
            std::cout << std::endl << "FOUND USER:" << std::endl << std::endl;
            output_session_data(temp);
            std::cout << std::endl;
            delete temp;
        }

        else if (check_date_event(state))
        {
            session_data *temp = get_state(state, get_target);
            std::cout << std::endl << "FOUND USER AT THE POINT TIME " + state + ":" << std::endl << std::endl;
            output_session_data(temp);
            std::cout << std::endl;
            delete temp;
        }

        else
        {
            throw std::invalid_argument("ERROR: Invalid state to get");
        }
    }
}


// команда GET_RANGE pool_1 scheme_1 collection_1
void handler_get_range::handle_request(
        const std::vector<std::string> &params,
        std::istream &stream,
        bool console)
{
    if (!params.empty() && params[0] == "GET_RANGE")
    {
        handler::get_logger()->log("GET_RANGE command called", logger::severity::INFORMATION);

        type_data_base *data_base = handler::get_instance();

        if (params.size() != 4)
        {
            handler::get_logger()->log("Exception caught", logger::severity::INFORMATION);
            throw std::invalid_argument("ERROR: Invalid arguments to add collection");
        }

        if (!data_base->in(params[1]))
        {
            handler::get_logger()->log("Exception caught", logger::severity::INFORMATION);
            throw std::invalid_argument("ERROR: There is no such pool");
        }

        std::pair<type_pool*, memory*> pool = data_base->get(params[1]);

        if (!pool.first->in(params[2]))
        {
            handler::get_logger()->log("Exception caught", logger::severity::INFORMATION);
            throw std::invalid_argument("ERROR: There is no such data_scheme");
        }

        std::pair<type_scheme*, memory*> scheme = pool.first->get(params[2]);

        if (!scheme.first->in(params[3]))
        {
            handler::get_logger()->log("Exception caught", logger::severity::INFORMATION);
            throw std::invalid_argument("ERROR: There is no such collection");
        }

        std::pair<type_collection*, std::map<std::string, type_order_collection*>> *collection = scheme.first->get(params[3]);

        command *execute_get_range = new get_range_command;

        try
        {
            execute_get_range->execute(params, &pool, &scheme, collection, stream, console);
        }

        catch (const std::exception &ex)
        {
            handler::get_logger()->log("Exception caught", logger::severity::INFORMATION);
            delete execute_get_range;
            throw std::invalid_argument(ex.what());
        }

        delete execute_get_range;

        handler::get_logger()->log("Successful command execution", logger::severity::INFORMATION);
    }

    else
    {
        if (_next_handler != nullptr)
        {
            _next_handler->handle_request(params, stream, console);
        }
    }
}

// вывести данные в промежутке ключей или полей
void handler_get_range::get_range_command::execute(
        const std::vector<std::string> &params,
        std::pair<type_pool*, memory*> *pool,
        std::pair<type_scheme*, memory*> *scheme,
        std::pair<type_collection*, std::map<std::string, type_order_collection*>> *collection,
        std::istream &stream,
        bool console)
{
    size_t id_session_lower_bound = 0;
    size_t id_student_lower_bound = 0;
    size_t id_session_upper_bound = 0;
    size_t id_student_upper_bound = 0;
    std::string reporting_form_lower_bound;
    std::string name_subject_lower_bound;
    std::string reporting_form_upper_bound;
    std::string name_subject_upper_bound;

    std::vector<type_value*> res;

    std::function<bool(const std::string&)> check_functions[9] = {
            check_surname_student,
            check_name_student,
            check_patronymic_student,
            check_date_event,
            check_time_event,
            check_mark,
            check_surname_teacher,
            check_name_teacher,
            check_patronymic_teacher
    };

    std::string names_field[9] = {
            "SURNAME_STUDENT",
            "NAME_STUDENT",
            "PATRONYMIC_STUDENT",
            "DATE",
            "TIME",
            "MARK",
            "SURNAME_TEACHER",
            "NAME_TEACHER",
            "PATRONYMIC_TEACHER"
    };

    try
    {
        if (console)
        {
            int number_field = 0;
            std::string lower_bound;
            std::string upper_bound;

            std::cout << "Choose field to be searched: " << std::endl;
            std::cout << "1. Id_session, id_student, reporting_form and name_subject" << std::endl;
            std::cout << "2. Surname student" << std::endl;
            std::cout << "3. Name student" << std::endl;
            std::cout << "4. Patronymic student" << std::endl;
            std::cout << "5. Date" << std::endl;
            std::cout << "6. Time" << std::endl;
            std::cout << "7. Mark" << std::endl;
            std::cout << "8. Surname teacher" << std::endl;
            std::cout << "9. Name teacher" << std::endl;
            std::cout << "10. Patronymic teacher" << std::endl;
            std::cout << "Number field: ";
            std::cin >> number_field;

            try
            {
                if (number_field == 1)
                {
                    std::cout << "Print the lower bound: " << std::endl;
                    read_key_session_data_from_console(id_session_lower_bound, id_student_lower_bound,
                            reporting_form_lower_bound, name_subject_lower_bound);
                    std::cout << "Print the upper bound: " << std::endl;
                    read_key_session_data_from_console(id_session_upper_bound, id_student_upper_bound,
                            reporting_form_upper_bound, reporting_form_upper_bound);

                    res = collection->first->get_range(Key{id_session_lower_bound, id_student_lower_bound,
                                                           reporting_form_lower_bound, name_subject_lower_bound},
                                                                   Key{id_session_upper_bound, id_student_upper_bound,
                                                                       reporting_form_upper_bound, reporting_form_upper_bound});
                }

                else if (number_field >= 2 && number_field <= 9)
                {
                    std::cout << "Print the lower bound:: " << std::endl;
                    std::cin >> lower_bound;
                    std::cout << "Print the upper bound: " << std::endl;
                    std::cin >> upper_bound;

                    if ((number_field - 2) != 5)
                    {
                        if (!check_functions[number_field - 2](lower_bound) ||
                            !check_functions[number_field - 2](upper_bound))
                        {
                            throw std::invalid_argument("ERROR: Invalid argument lower or upper bound");
                        }
                    }

                    // валидация
                    if ((number_field - 2) == 5)
                    {

                        if (!check_id(lower_bound))
                        {
                            throw std::invalid_argument("ERROR: Invalid argument lower mark");
                        }

                        if (!check_id(upper_bound))
                        {
                            throw std::invalid_argument("ERROR: Invalid argument upper mark");
                        }

                        int check_number_lower = std::stoi(lower_bound);
                        if (((reporting_form_lower_bound == "Coursework" || reporting_form_lower_bound == "Exam") &&
                             (check_number_lower < 2 || check_number_lower > 5))  ||
                            ((reporting_form_lower_bound == "Test") && (check_number_lower < 0 || check_number_lower > 1)))
                        {
                            throw std::invalid_argument("ERROR: Invalid argument lower mark");
                        }

                        int check_number_upper = std::stoi(lower_bound);
                        if (((reporting_form_lower_bound == "Coursework" || reporting_form_lower_bound == "Exam" ) &&
                             (check_number_upper < 2 || check_number_upper > 5))  ||
                            ((reporting_form_lower_bound == "Test") && (check_number_upper < 0 || check_number_upper > 1)))
                        {
                            throw std::invalid_argument("ERROR: Invalid argument upper mark");
                        }

                    }

                    res = collection->second[names_field[number_field - 2]]->get_range(lower_bound, upper_bound);
                }

                else
                {
                    throw std::invalid_argument("ERROR: Invalid number_field");
                }
            }

            catch (const std::exception &ex)
            {
                throw std::invalid_argument(ex.what());
            }
        }

        else
        {
            std::string name_field;
            std::string lower_bound;
            std::string upper_bound;
            std::string line_lower_bound;
            std::string line_upper_bound;

            if (stream.eof())
            {
                throw std::invalid_argument("ERROR: End of file");
            }

            std::getline(stream, name_field);
            name_field = handler::delete_spaces(name_field);

            if (name_field == "ID_SESSION_ID_STUDENT_REPORTING_FORM_NAME_SUBJECT")
            {
                try
                {
                    read_key_session_data_from_file(id_session_lower_bound, id_student_lower_bound,
                            reporting_form_lower_bound, name_subject_lower_bound, stream);
                    read_key_session_data_from_file(id_session_upper_bound, id_student_upper_bound,
                            reporting_form_upper_bound, name_subject_upper_bound, stream);
                }

                catch (const std::exception &ex)
                {
                    throw std::invalid_argument(ex.what());
                }

                res = collection->first->get_range(Key{id_session_lower_bound, id_student_lower_bound,
                                                       reporting_form_lower_bound, name_subject_lower_bound},
                        Key{id_session_upper_bound, id_student_upper_bound,
                            reporting_form_upper_bound, reporting_form_upper_bound});

            }

            // если поиск по другим полям
            else if (name_field == "SURNAME_STUDENT" ||
                     name_field == "NAME_STUDENT" ||
                     name_field == "PATRONYMIC_STUDENT" ||
                     name_field == "DATE" ||
                     name_field == "TIME" ||
                     name_field == "MARK" ||
                     name_field == "SURNAME_TEACHER" ||
                     name_field == "NAME_TEACHER" ||
                     name_field == "PATRONYMIC_TEACHER")
            {
                if (stream.eof())
                {
                    throw std::invalid_argument("ERROR: End of file");
                }

                std::getline(stream, line_lower_bound);

                if (stream.eof())
                {
                    throw std::invalid_argument("ERROR: End of file");
                }

                std::getline(stream, line_upper_bound);

                try
                {
                    read_arguments(line_lower_bound, name_field, ":", lower_bound);
                    read_arguments(line_upper_bound, name_field, ":", upper_bound);
                }

                catch (const std::exception &ex)
                {
                    throw std::invalid_argument(ex.what());
                }

                res = collection->second[name_field]->get_range(lower_bound, upper_bound);
            }

            else
            {
                throw std::invalid_argument("ERROR: Invalid name field");
            }
        }
    }

    catch (const std::exception &ex)
    {
        throw std::invalid_argument(ex.what());
    }

    session_data *temp = nullptr;

    // если ничего не нашли
    if (res.empty())
    {
        std::cout << "No matches" << std::endl;
    }

    // если нашли подходящие данные
    else
    {
        std::cout << std::endl << "FOUND USERS:" << std::endl << std::endl;

        if (console)
        {
            std::vector<std::pair<std::string, std::string>> last_item;
            std::string datetime;
            long long _datetime = 0;
            int choice = 0;

            std::cout << "At what point in time do you want to receive data?" << std::endl;
            std::cout << "1. Current point in time" << std::endl;
            std::cout << "2. Set point in time" << std::endl;
            std::cout << "Print the number: ";
            std::cin >> choice;

            switch (choice)
            {
                case 1:
                    for (auto &item: res)
                    {
                        temp = get_state("", item);
                        output_session_data(temp);
                        std::cout << std::endl;
                        delete temp;
                    }

                    break;
                case 2:
                    std::cout << "Put the datetime in format %d/%m/%y-%h:%m:%s: ";
                    std::cin >> datetime;

                    _datetime = get_time(datetime);
                    datetime = handler::delete_spaces(datetime);

                    if (check_date_event(datetime))
                    {
                        for (auto &item: res)
                        {
                            temp = get_state(datetime, item);
                            output_session_data(temp);
                            std::cout << std::endl;
                            delete temp;
                        }
                    }

                    else
                    {
                        throw std::invalid_argument("ERROR: Invalid format datetime");
                    }

                    break;
                default:
                    throw std::invalid_argument("ERROR: Invalid number to get item");
            }
        }

        else
        {
            std::string state;

            if (stream.eof())
            {
                throw std::invalid_argument("ERROR: File end");
            }

            std::getline(stream, state);
            state = handler::delete_spaces(state);

            if (state == "CURRENT")
            {
                for (auto &item: res)
                {
                    temp = get_state("", item);
                    output_session_data(temp);
                    std::cout << std::endl;
                    delete temp;
                }
            }

            else if (check_date_event(state))
            {
                for (auto &item: res)
                {
                    temp = get_state(state, item);
                    output_session_data(temp);
                    std::cout << std::endl;
                    delete temp;
                }
            }

            else
            {
                throw std::invalid_argument("ERROR: Invalid state to get");
            }
        }
    }
}


// команда UPDATE pool_1 scheme_1 collection_1
void handler_update::handle_request(
        const std::vector<std::string> &params,
        std::istream &stream,
        bool console)
{
    if (!params.empty() && params[0] == "UPDATE")
    {
        handler::get_logger()->log("UPDATE command called", logger::severity::INFORMATION);

        type_data_base *data_base = handler::get_instance();

        if (params.size() != 4)
        {
            handler::get_logger()->log("Exception caught", logger::severity::INFORMATION);
            throw std::invalid_argument("ERROR: Invalid arguments to add collection");
        }

        if (!data_base->in(params[1]))
        {
            handler::get_logger()->log("Exception caught", logger::severity::INFORMATION);
            throw std::invalid_argument("ERROR: There is no such pool");
        }

        std::pair<type_pool*, memory*> pool = data_base->get(params[1]);

        if (!pool.first->in(params[2]))
        {
            handler::get_logger()->log("Exception caught", logger::severity::INFORMATION);
            throw std::invalid_argument("ERROR: There is no such data_scheme");
        }

        std::pair<type_scheme*, memory*> scheme = pool.first->get(params[2]);

        if (!scheme.first->in(params[3]))
        {
            handler::get_logger()->log("Exception caught", logger::severity::INFORMATION);
            throw std::invalid_argument("ERROR: There is no such collection");
        }

        std::pair<type_collection*, std::map<std::string, type_order_collection*>> *collection = scheme.first->get(params[3]);

        command *execute_update = new update_command;

        try
        {
            execute_update->execute(params, &pool, &scheme, collection, stream, console);
        }

        catch (const std::exception &ex)
        {
            handler::get_logger()->log("Exception caught", logger::severity::INFORMATION);
            delete execute_update;
            throw std::invalid_argument(ex.what());
        }

        delete execute_update;

        handler::get_logger()->log("Successful command execution", logger::severity::INFORMATION);
    }

    else
    {
        if (_next_handler != nullptr)
        {
            _next_handler->handle_request(params, stream, console);
        }
    }
}

// обновление данных (и добавление)
void handler_update::update_command::execute(
        const std::vector<std::string> &params,
        std::pair<type_pool*, memory*> *pool,
        std::pair<type_scheme*, memory*> *scheme,
        std::pair<type_collection*, std::map<std::string, type_order_collection*>> *collection,
        std::istream &stream,
        bool console)
{
    size_t id_session = 0;
    size_t id_student = 0;
    std:: string reporting_form;
    std:: string name_subject;

    session_data to_insert;

    try
    {
        if (console)
        {
            read_key_session_data_from_console(id_session, id_student,reporting_form, name_subject);
        }

        else
        {
            read_key_session_data_from_file(id_session, id_student, reporting_form, name_subject, stream);
        }
    }

    catch (const std::exception &ex)
    {
        throw std::invalid_argument(ex.what());
    }

    if (collection->first->in(Key{id_session, id_student, reporting_form, name_subject}))
    {
        type_value *update_item = collection->first->get(Key{id_session, id_student, reporting_form, name_subject});
        long long current_time = get_time(get_current_datetime());

        if (console)
        {
            int number_field = -1;

            while (number_field != 0)
            {
                std::string new_value;

                std::cout << "Choose number to update some field or cancel" << std::endl;
                std::cout << "0. cancel" << std::endl;
                std::cout << "1. Surname student" << std::endl;
                std::cout << "2. Patronymic student" << std::endl;
                std::cout << "3. Date" << std::endl;
                std::cout << "4. Time" << std::endl;
                std::cout << "5. Mark" << std::endl;
                std::cout << "6. Surname teacher" << std::endl;
                std::cout << "7. Name teacher" << std::endl;
                std::cout << "8. Patronymic teacher" << std::endl;
                std::cout << "Print the number: ";
                std::cin >> number_field;

                // выбираем поле и добавляем новое значение в него
                switch (number_field)
                {
                    case 0:
                        break;
                    case 1:
                        std::cout << "Surname student: ";
                        std::cin >> new_value;
                        update_item->second[current_time].emplace_back("SURNAME_STUDENT", new_value);
                        break;
                    case 2:
                        std::cout << "Name student: ";
                        std::cin >> new_value;
                        update_item->second[current_time].emplace_back("NAME_STUDENT", new_value);
                        break;
                    case 3:
                        std::cout << "Patronymic student: ";
                        std::cin >> new_value;
                        update_item->second[current_time].emplace_back("PATRONYMIC_STUDENT", new_value);
                        break;
                    case 4:
                        std::cout << "Date: ";
                        std::cin >> new_value;
                        update_item->second[current_time].emplace_back("DATE", new_value);
                        break;
                    case 5:
                        std::cout << "Time: ";
                        std::cin >> new_value;
                        update_item->second[current_time].emplace_back("TIME", new_value);
                        break;
                    case 6:
                        std::cout << "Mark: ";
                        std::cin >> new_value;
                        update_item->second[current_time].emplace_back("MARK", new_value);
                        break;
                    case 7:
                        std::cout << "Surname teacher: ";
                        std::cin >> new_value;
                        update_item->second[current_time].emplace_back("SURNAME_TEACHER", new_value);
                        break;
                    case 8:
                        std::cout << "Name teacher: ";
                        std::cin >> new_value;
                        update_item->second[current_time].emplace_back("NAME_TEACHER", new_value);
                        break;
                    case 9:
                        std::cout << "Patronymic teacher: ";
                        std::cin >> new_value;
                        update_item->second[current_time].emplace_back("PATRONYMIC_TEACHER", new_value);
                        break;
                    default:
                        throw std::invalid_argument("ERROR: Invalid number to update");
                }
            }
        }

        else
        {
            std::string line;

            // пока не написано команды STOP_UPDATE делаем аналогичные добавления
            while (line != "STOP_UPDATE")
            {
                if (stream.eof())
                {
                    throw std::invalid_argument("ERROR: Error update");
                }

                std::getline(stream, line);

                if (handler::delete_spaces(line) != "STOP_UPDATE")
                {
                    if (line.find(':') == std::string::npos)
                    {
                        throw std::invalid_argument("ERROR: Error field to update");
                    }

                    std::string name_argument = handler::delete_spaces(line.substr(0, line.find(':')));
                    std::string value_argument = handler::delete_spaces(line.substr(line.find(':') + 1));

                    if (name_argument == "SURNAME_STUDENT")
                    {
                        update_item->second[current_time].emplace_back("SURNAME_STUDENT", value_argument);
                    }

                    else if (name_argument == "NAME_STUDENT")
                    {
                        update_item->second[current_time].emplace_back("NAME_STUDENT", value_argument);
                    }

                    else if (name_argument == "PATRONYMIC_STUDENT")
                    {
                        update_item->second[current_time].emplace_back("PATRONYMIC_STUDENT", value_argument);
                    }

                    else if (name_argument == "DATE")
                    {
                        update_item->second[current_time].emplace_back("DATE", value_argument);
                    }

                    else if (name_argument == "TIME")
                    {
                        update_item->second[current_time].emplace_back("TIME", value_argument);
                    }

                    else if (name_argument == "MARK")
                    {
                        update_item->second[current_time].emplace_back("MARK", value_argument);
                    }

                    else if (name_argument == "SURNAME_TEACHER")
                    {
                        update_item->second[current_time].emplace_back("SURNAME_TEACHER", value_argument);
                    }

                    else if (name_argument == "NAME_TEACHER")
                    {
                        update_item->second[current_time].emplace_back("NAME_TEACHER", value_argument);
                    }

                    else if (name_argument == "PATRONYMIC_TEACHER")
                    {
                        update_item->second[current_time].emplace_back("PATRONYMIC_TEACHER", value_argument);
                    }

                    else
                    {
                        throw std::invalid_argument("ERROR: Error field to update");
                    }
                }
            }
        }
    }

    else
    {
        throw std::invalid_argument("ERROR: There is no such session_data");
    }
}


// команда REMOVE pool_1 scheme_1 collection_1
void handler_remove::handle_request(
        const std::vector<std::string> &params,
        std::istream &stream,
        bool console)
{
    if (!params.empty() && params[0] == "REMOVE")
    {
        handler::get_logger()->log("REMOVE command called", logger::severity::INFORMATION);

        type_data_base *data_base = handler::get_instance();

        if (params.size() != 4)
        {
            handler::get_logger()->log("Exception caught", logger::severity::INFORMATION);
            throw std::invalid_argument("ERROR: Invalid arguments to add collection");
        }

        if (!data_base->in(params[1]))
        {
            handler::get_logger()->log("Exception caught", logger::severity::INFORMATION);
            throw std::invalid_argument("ERROR: There is no such pool");
        }

        std::pair<type_pool*, memory*> pool = data_base->get(params[1]);

        if (!pool.first->in(params[2]))
        {
            handler::get_logger()->log("Exception caught", logger::severity::INFORMATION);
            throw std::invalid_argument("ERROR: There is no such data_scheme");
        }

        std::pair<type_scheme*, memory*>scheme = pool.first->get(params[2]);

        if (!scheme.first->in(params[3]))
        {
            handler::get_logger()->log("Exception caught", logger::severity::INFORMATION);
            throw std::invalid_argument("ERROR: There is no such collection");
        }

        std::pair<type_collection*, std::map<std::string, type_order_collection*>> *collection = scheme.first->get(params[3]);

        command *execute_remove = new remove_command;

        try
        {
            execute_remove->execute(params, &pool, &scheme, collection, stream, console);
        }

        catch (const std::exception &ex)
        {
            handler::get_logger()->log("Exception caught", logger::severity::INFORMATION);
            delete execute_remove;
            throw std::invalid_argument(ex.what());
        }

        delete execute_remove;

        handler::get_logger()->log("Successful command execution", logger::severity::INFORMATION);
    }

    else
    {
        if (_next_handler != nullptr)
        {
            _next_handler->handle_request(params, stream, console);
        }
    }
}

// удаление данных по ключу
void handler_remove::remove_command::execute(
        const std::vector<std::string> &params,
        std::pair<type_pool*, memory*> *pool,
        std::pair<type_scheme*, memory*> *scheme,
        std::pair<type_collection*, std::map<std::string, type_order_collection*>> *collection,
        std::istream &stream,
        bool console)
{
    size_t id_session = 0;
    size_t id_student = 0;
    std:: string reporting_form;
    std:: string name_subject;

    try
    {
        if (console)
        {
            read_key_session_data_from_console(id_session, id_student,reporting_form, name_subject);
        }

        else
        {
            read_key_session_data_from_file(id_session, id_student, reporting_form, name_subject, stream);
        }
    }

    catch (const std::exception &ex)
    {
        throw std::invalid_argument(ex.what());
    }

    if (collection->first->in(Key{id_session, id_student, reporting_form, name_subject}))
    {
        type_value *deleted_value = collection->first->get(Key{id_session, id_student, reporting_form, name_subject});

        collection->second["SURNAME_STUDENT"]->remove(deleted_value->first->get_surname_student()->get_value());
        collection->second["NAME_STUDENT"]->remove(deleted_value->first->get_name_student()->get_value());
        collection->second["PATRONYMIC_STUDENT"]->remove(deleted_value->first->get_patronymic_student()->get_value());
        collection->second["DATE"]->remove(deleted_value->first->get_date_event()->get_value());
        collection->second["TIME"]->remove(deleted_value->first->get_time_event()->get_value());
        collection->second["MARK"]->remove(deleted_value->first->get_mark()->get_value());
        collection->second["SURNAME_TEACHER"]->remove(deleted_value->first->get_surname_teacher()->get_value());
        collection->second["NAME_TEACHER"]->remove(deleted_value->first->get_name_teacher()->get_value());
        collection->second["PATRONYMIC_TEACHER"]->remove(deleted_value->first->get_patronymic_teacher()->get_value());
        collection->first->remove(Key{id_session, id_student, reporting_form, name_subject});
    }

    else
    {
        throw std::invalid_argument("ERROR: There is no such session_data");
    }
}


// чтение key data из файла
void read_key_session_data_from_file(
        size_t &id_session,
        size_t &id_student,
        std:: string &reporting_form,
        std:: string &name_subject,
        std::istream &stream)
{
    std::string name_arguments[4] = {
            "SESSION_ID",
            "STUDENT_ID",
            "REPORTING_FORM",
            "NAME_SUBJECT"
    };

    int num_read_fields = 0;
    std::string fields[4];
    std::string line;

    for (int i = 0; i < 4 && !stream.eof(); i++)
    {
        std::getline(stream, line);

        try
        {
            read_arguments(line, name_arguments[i], ":", fields[num_read_fields++]);
        }

        catch (const std::exception &ex)
        {
            throw std::invalid_argument(ex.what());
        }
    }

    if (num_read_fields != 4)
    {
        throw std::out_of_range("ERROR: Not enough data");
    }

    if (!check_id(fields[0]))
    {
        throw std::invalid_argument("ERROR: Invalid SESSION_ID");
    }

    if (!check_id(fields[1]))
    {
        throw std::invalid_argument("ERROR: Invalid STUDENT_ID");
    }

    if (!check_reporting_form(fields[2]))
    {
        throw std::invalid_argument("ERROR: Invalid REPORTING_FORM");
    }

    if (!check_name_subject(fields[3]))
    {
        throw std::invalid_argument("ERROR: Invalid NAME_SUBJECT");
    }

    id_session = std::stoi(fields[0]);
    id_student = std::stoi(fields[1]);
    reporting_form = fields[2];
    name_subject = fields[3];
}

// чтение и заполнение полей из файла
void read_session_data_from_file(
        session_data *&target,
        std::istream &stream,
        std:: string &reporting_form)
{
    target = new session_data;

    std::string name_arguments[9] = {
            "SURNAME_STUDENT",
            "NAME_STUDENT",
            "PATRONYMIC_STUDENT",
            "DATE",
            "TIME",
            "MARK",
            "SURNAME_TEACHER",
            "NAME_TEACHER",
            "PATRONYMIC_TEACHER"
    };

    int num_read_fields = 0;
    std::string fields[9];
    std::string line;

    for (int i = 0; i < 9 && !stream.eof(); i++)
    {
        std::getline(stream, line);

        try
        {
            read_arguments(line, name_arguments[i], ":", fields[num_read_fields++]);
        }

        catch (const std::exception &ex)
        {
            throw std::invalid_argument(ex.what());
        }
    }

    if (num_read_fields != 9)
    {
        throw std::out_of_range("ERROR: Not enough data");
    }

    if (!check_surname_student(fields[0]))
    {
        throw std::invalid_argument("ERROR: Invalid surname student");
    }

    target->set_surname_student(fields[0]);


    if (!check_name_student(fields[1]))
    {
        throw std::invalid_argument("ERROR: Invalid name student");
    }

    target->set_name_student(fields[1]);


    if (!check_patronymic_student(fields[2]))
    {
        throw std::invalid_argument("ERROR: Invalid patronymic student");
    }

    target->set_patronymic_student(fields[2]);


    if (!check_date_event(fields[3]))
    {
        throw std::invalid_argument("ERROR: Invalid date");
    }

    target->set_date_event(fields[3]);


    if (!check_time_event(fields[4]))
    {
        throw std::invalid_argument("ERROR: Invalid time");
    }

    target->set_time_event(fields[4]);



    if (!check_id(fields[5]))
    {
        throw std::invalid_argument("ERROR: Invalid mark");
    }

    int check_number = std::stoi(fields[5]);
    if (((reporting_form == "Coursework" || reporting_form == "Exam") &&
    (check_number < 2 || check_number > 5))  ||
    ((reporting_form == "Test") && (check_number < 0 || check_number > 1)))
    {
        throw std::invalid_argument("ERROR: Invalid mark");
    }

    target->set_mark(fields[5]);



    if (!check_surname_teacher(fields[6]))
    {
        throw std::invalid_argument("ERROR: Invalid surname teacher");
    }

    target->set_surname_teacher(fields[6]);


    if (!check_name_teacher(fields[7]))
    {
        throw std::invalid_argument("ERROR: Invalid name teacher");
    }

    target->set_name_teacher(fields[7]);


    if (!check_patronymic_teacher(fields[8]))
    {
        throw std::invalid_argument("ERROR: Invalid patronymic teacher");
    }

    target->set_patronymic_teacher(fields[8]);
}


// чтение данных key data из консоли
void read_key_session_data_from_console(
        size_t &id_session,
        size_t &id_student,
        std:: string &reporting_form,
        std:: string &name_subject)
{
    std::string id_session_string;
    std::string id_student_string;
    std::string reporting_form_string;
    std::string name_subject_string;

    std::cout << "SESSION_ID: ";
    std::cin >> id_session_string;

    if (!check_id(id_session_string))
    {
        throw std::invalid_argument("ERROR: Invalid session id");
    }


    std::cout << "STUDENT_ID: ";
    std::cin >> id_student_string;

    if (!check_id(id_student_string))
    {
        throw std::invalid_argument("ERROR: Invalid student id");
    }


    std::cout << "REPORTING_FORM: ";
    std::cin >> reporting_form_string;

    if (!check_reporting_form(reporting_form_string))
    {
        throw std::invalid_argument("ERROR: Invalid reporting form");
    }


    std::cout << "NAME_SUBJECT: ";
    std::cin >> name_subject_string;

    if (!check_name_subject(name_subject_string))
    {
        throw std::invalid_argument("ERROR: Invalid name subject");
    }

    id_session = std::stoi(id_session_string);
    id_student = std::stoi(id_student_string);
    reporting_form = reporting_form_string;
    name_subject = name_subject_string;
}

// чтение и заполнение полей из консоли
void read_session_data_from_console(
        session_data *&target,
        std:: string &reporting_form)
{
    std::string data;
    target = new session_data; // создаем новый объект класса session_data и заполняем поля

    std::cout << "SURNAME_STUDENT: ";
    std::cin >> data;
    if (!check_surname_student(data))
    {
        throw std::invalid_argument("ERROR: Invalid surname student");
    }

    target->set_surname_student(data);


    std::cout << "NAME_STUDENT: ";
    std::cin >> data;
    if (!check_name_student(data))
    {
        throw std::invalid_argument("ERROR: Invalid name student");
    }

    target->set_name_student(data);


    std::cout << "PATRONYMIC_STUDENT: ";
    std::cin >> data;
    if (!check_patronymic_student(data))
    {
        throw std::invalid_argument("ERROR: Invalid patronymic student");
    }

    target->set_patronymic_student(data);

    std::cout << "DATE: ";
    std::cin >> data;
    if (!check_date_event(data))
    {
        throw std::invalid_argument("ERROR: Invalid date");
    }

    target->set_date_event(data);


    std::cout << "TIME: ";
    std::cin >> data;
    if (!check_time_event(data))
    {
        throw std::invalid_argument("ERROR: Invalid time");
    }

    target->set_time_event(data);



    std::cout << "MARK: ";
    std::cin >> data;

    if (!check_id(data))
    {
        throw std::invalid_argument("ERROR: Invalid mark");
    }

    int check_number = std::stoi(data);
    if (((reporting_form == "Coursework" || reporting_form == "Exam") &&
         (check_number < 2 || check_number > 5))  ||
        ((reporting_form == "Test") && (check_number < 0 || check_number > 1)))
    {
        throw std::invalid_argument("ERROR: Invalid mark");
    }

    target->set_mark(data);



    std::cout << "SURNAME_TEACHER: ";
    std::cin >> data;
    if (!check_surname_teacher(data))
    {
        throw std::invalid_argument("ERROR: Invalid surname teacher");
    }

    target->set_surname_teacher(data);


    std::cout << "NAME_TEACHER: ";
    std::cin >> data;
    if (!check_name_teacher(data))
    {
        throw std::invalid_argument("ERROR: Invalid name teacher");
    }

    target->set_name_teacher(data);


    std::cout << "PATRONYMIC_TEACHER: ";
    std::cin >> data;
    if (!check_patronymic_teacher(data))
    {
        throw std::invalid_argument("ERROR: Invalid patronymic teacher");
    }

    target->set_patronymic_teacher(data);
    std::cout << std::endl;
}

// вывод всех полей на консоль
void output_session_data(session_data *&target)
{
    std::cout << "SURNAME_STUDENT: " << target->get_surname_student()->get_value() << std::endl;
    std::cout << "NAME_STUDENT: " << target->get_name_student()->get_value() << std::endl;
    std::cout << "PATRONYMIC_STUDENT: " << target->get_patronymic_student()->get_value() << std::endl;
    std::cout << "DATE: " << target->get_date_event()->get_value() << std::endl;
    std::cout << "TIME: " << target->get_time_event()->get_value() << std::endl;
    std::cout << "MARK: " << target->get_mark()->get_value() << std::endl;
    std::cout << "SURNAME_TEACHER: " << target->get_surname_teacher()->get_value() << std::endl;
    std::cout << "NAME_TEACHER: " << target->get_name_teacher()->get_value() << std::endl;
    std::cout << "PATRONYMIC_TEACHER: " << target->get_patronymic_teacher()->get_value() << std::endl;
}

// построчное прочтение параметров
void read_arguments(
        const std::string &str,
        const std::string &name_argument,
        const std::string &sep,
        std::string &value_argument)
{
    if (str.find(sep) == std::string::npos)
    {
        throw std::invalid_argument("ERROR: Invalid line of arguments");
    }

    std::string _name_argument = handler::delete_spaces(str.substr(0, str.find(sep)));

    if (_name_argument != name_argument)
    {
        throw std::invalid_argument("ERROR: Invalid name argument");
    }

    // выделяем из строки название аллокатора, если все верно написано
    value_argument = handler::delete_spaces(str.substr(str.find(sep) + 1));
}

std::string get_current_datetime()
{
    std::time_t seconds = std::time(nullptr);
    std::tm* now = std::localtime(&seconds);

    char buffer[BUFSIZ];
    strftime(buffer, sizeof(buffer), "%d/%m/%Y %X", now);

    return buffer;
}

// 30/08/2023 23:10:15

bool is_leap(int year)
{
    if (year % 4 == 0)
    {
        if (year % 100 == 0)
        {
            if (year % 400 == 0)
            {
                return true;
            }
        }

        else
        {
            return true;
        }
    }

    return false;
}

long long get_time(const std::string &time)
{
    int day_in_months[12] = {
            31,
            28,
            31,
            30,
            31,
            30,
            31,
            31,
            30,
            31,
            30,
            31
    };

    long long full_seconds = 0;
    long long full_days = 0;

    int day = std::stoi(time.substr(0, 2));
    int month = std::stoi(time.substr(3, 2));
    int year = std::stoi(time.substr(6, 4));
    int hours = std::stoi(time.substr(11, 2));
    int minutes = std::stoi(time.substr(14, 2));
    int seconds = std::stoi(time.substr(17, 2));

    full_days += day;
    full_days += (month == 2) ? (is_leap(year) ? day_in_months[month - 1] + 1 : day_in_months[month - 1]) : (day_in_months[month - 1]);

    for (int i = 0; i <= year; i++)
    {
        if (is_leap(i))
        {
            full_days += 366;
        }

        else
        {
            full_days += 365;
        }
    }

    full_seconds += seconds;
    full_seconds += minutes * 60;
    full_seconds += hours * 60 * 60;
    full_seconds += full_days * 24 * 60 * 60;

    return full_seconds; // возвращаем общее время в секундах
}
