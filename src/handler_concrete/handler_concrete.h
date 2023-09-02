#ifndef HANDLE_CONCRETE_H
#define HANDLE_CONCRETE_H

#define SIZE_TO_ALLOCATOR 100000

#include "handler.h"
#include "validation_data.h"


void read_key_session_data_from_file(
        size_t &id_session,
        size_t &id_student,
        std:: string &reporting_form,
        std:: string &name_subject,
        std::istream &stream);

void read_session_data_from_file(
        session_data *&target,
        std::istream &stream,
        std:: string &reporting_form);

void read_key_session_data_from_console(
        size_t &id_session,
        size_t &id_student,
        std:: string &reporting_form,
        std:: string &name_subject);

void read_session_data_from_console(
        session_data *&target,
        std:: string &reporting_form);

void output_session_data(session_data *&target);

void read_arguments(
        const std::string &str,
        const std::string &name_argument,
        const std::string &sep,
        std::string &value_argument);

std::string get_current_datetime();

session_data *get_state(
        const std::string &datetime,
        type_value *states);

bool is_leap(int year);

long long get_time(const std::string &time);

class handler_concrete: public handler
{

public:

    handler_concrete();

    void handle_request(
            const std::vector<std::string> &params,
            std::istream &stream,
            bool console) override;

    ~handler_concrete() override;
};

class handler_run: public handler
{

public:

    void handle_request(
            const std::vector<std::string> &params,
            std::istream &stream,
            bool console) override;

    class run_command: public command
    {

    private:

        handler *_handler;

    public:

        explicit run_command(handler *handler_init);

        void execute(
                const std::vector<std::string> &params,
                std::pair<type_pool*, memory*> *pool,
                std::pair<type_scheme*, memory*> *scheme,
                std::pair<type_collection*, std::map<std::string, type_order_collection*>> *collection,
                std::istream &stream,
                bool console) override;
    };

    ~handler_run() override = default;
};

class handler_add_pool: public handler
{

public:

    void handle_request(
            const std::vector<std::string> &params,
            std::istream &stream,
            bool console) override;

    class add_pool_command: public command
    {

    public:

        void execute(
                const std::vector<std::string> &params,
                std::pair<type_pool*, memory*> *pool,
                std::pair<type_scheme*, memory*> *scheme,
                std::pair<type_collection*, std::map<std::string, type_order_collection*>> *collection,
                std::istream &stream,
                bool console) override;
    };

    ~handler_add_pool() override = default;
};

class handler_remove_pool: public handler
{

public:

    void handle_request(
            const std::vector<std::string> &params,
            std::istream &stream,
            bool console) override;

    class remove_pool_command: public command
    {

    public:

        void execute(
                const std::vector<std::string> &params,
                std::pair<type_pool*, memory*> *pool,
                std::pair<type_scheme*, memory*> *scheme,
                std::pair<type_collection*, std::map<std::string, type_order_collection*>> *collection,
                std::istream &stream,
                bool console) override;
    };

    ~handler_remove_pool() override = default;
};

class handler_add_scheme: public handler
{

public:

    void handle_request(
            const std::vector<std::string> &params,
            std::istream &stream,
            bool console) override;

    class add_scheme_command: public command
    {

    public:

        void execute(
                const std::vector<std::string> &params,
                std::pair<type_pool*, memory*> *pool,
                std::pair<type_scheme*, memory*> *scheme,
                std::pair<type_collection*, std::map<std::string, type_order_collection*>> *collection,
                std::istream &stream,
                bool console) override;
    };

    ~handler_add_scheme() override = default;
};

class handler_remove_scheme: public handler
{

public:

    void handle_request(
            const std::vector<std::string> &params,
            std::istream &stream,
            bool console) override;

    class remove_scheme_command: public command
    {

    public:

        void execute(
                const std::vector<std::string> &params,
                std::pair<type_pool*, memory*> *pool,
                std::pair<type_scheme*, memory*> *scheme,
                std::pair<type_collection*, std::map<std::string, type_order_collection*>> *collection,
                std::istream &stream,
                bool console) override;
    };

    ~handler_remove_scheme() override = default;
};

class handler_add_collection: public handler
{

public:

    void handle_request(
            const std::vector<std::string> &params,
            std::istream &stream,
            bool console) override;

    class add_collection_command: public command
    {

    public:

        void execute(
                const std::vector<std::string> &params,
                std::pair<type_pool*, memory*> *pool,
                std::pair<type_scheme*, memory*> *scheme,
                std::pair<type_collection*, std::map<std::string, type_order_collection*>> *collection,
                std::istream &stream,
                bool console) override;
    };

    ~handler_add_collection() override = default;
};

class handler_remove_collection: public handler
{

public:

    void handle_request(
            const std::vector<std::string> &params,
            std::istream &stream,
            bool console) override;

    class remove_collection_command: public command
    {

    public:

        void execute(
                const std::vector<std::string> &params,
                std::pair<type_pool*, memory*> *pool,
                std::pair<type_scheme*, memory*> *scheme,
                std::pair<type_collection*, std::map<std::string, type_order_collection*>> *collection,
                std::istream &stream,
                bool console) override;
    };

    ~handler_remove_collection() override = default;
};

class handler_add_data: public handler
{

public:

    void handle_request(
            const std::vector<std::string> &params,
            std::istream &stream,
            bool console) override;

    class add_data_command: public command
    {

    public:

        void execute(
                const std::vector<std::string> &params,
                std::pair<type_pool*, memory*> *pool,
                std::pair<type_scheme*, memory*> *scheme,
                std::pair<type_collection*, std::map<std::string, type_order_collection*>> *collection,
                std::istream &stream,
                bool console) override;
    };

    ~handler_add_data() override = default;
};

class handler_get_data: public handler
{

public:

    void handle_request(
            const std::vector<std::string> &params,
            std::istream &stream,
            bool console) override;

    class get_data_command: public command
    {

    public:

        void execute(
                const std::vector<std::string> &params,
                std::pair<type_pool*, memory*> *pool,
                std::pair<type_scheme*, memory*> *scheme,
                std::pair<type_collection*, std::map<std::string, type_order_collection*>> *collection,
                std::istream &stream,
                bool console) override;
    };

    ~handler_get_data() override = default;
};

class handler_get_range: public handler
{

public:

    void handle_request(
            const std::vector<std::string> &params,
            std::istream &stream,
            bool console) override;

    class get_range_command: public command
    {

    public:

        void execute(
                const std::vector<std::string> &params,
                std::pair<type_pool*, memory*> *pool,
                std::pair<type_scheme*, memory*> *scheme,
                std::pair<type_collection*, std::map<std::string, type_order_collection*>> *collection,
                std::istream &stream,
                bool console) override;
    };

    ~handler_get_range() override = default;
};

class handler_update: public handler
{

public:

    void handle_request(
            const std::vector<std::string> &params,
            std::istream &stream,
            bool console) override;

    class update_command: public command
    {

    public:

        void execute(
                const std::vector<std::string> &params,
                std::pair<type_pool*, memory*> *pool,
                std::pair<type_scheme*, memory*> *scheme,
                std::pair<type_collection*, std::map<std::string, type_order_collection*>> *collection,
                std::istream &stream,
                bool console) override;
    };

    ~handler_update() override = default;
};

class handler_remove: public handler
{

public:

    void handle_request(
            const std::vector<std::string> &params,
            std::istream &stream,
            bool console) override;

    class remove_command: public command
    {

    public:

        void execute(
                const std::vector<std::string> &params,
                std::pair<type_pool*, memory*> *pool,
                std::pair<type_scheme*, memory*> *scheme,
                std::pair<type_collection*, std::map<std::string, type_order_collection*>> *collection,
                std::istream &stream,
                bool console) override;
    };

    ~handler_remove() override = default;
};

#endif //HANDLE_CONCRETE_H
