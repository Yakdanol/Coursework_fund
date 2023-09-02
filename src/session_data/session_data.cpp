#include "session_data.h"


std::shared_ptr<string_flyweight> session_data::get_surname_student() const
{
    return _surname_student;
}

void session_data::set_surname_student(const std::string &surname_student)
{
    string_flyweight_factory &factory = string_flyweight_factory::get_instance();

    _surname_student = factory.get_string_flyweight(surname_student);
}


std::shared_ptr<string_flyweight> session_data::get_name_student() const
{
    return _name_student;
}

void session_data::set_name_student(const std::string &name_student)
{
    string_flyweight_factory &factory = string_flyweight_factory::get_instance();

    _name_student = factory.get_string_flyweight(name_student);
}


std::shared_ptr<string_flyweight> session_data::get_patronymic_student() const
{
    return _patronymic_student;
}

void session_data::set_patronymic_student(const std::string &patronymic_student)
{
    string_flyweight_factory &factory = string_flyweight_factory::get_instance();

    _patronymic_student = factory.get_string_flyweight(patronymic_student);
}


std::shared_ptr<string_flyweight> session_data::get_date_event() const
{
    return _date_event;
}


void session_data::set_date_event(const std::string &date_event)
{
    string_flyweight_factory &factory = string_flyweight_factory::get_instance();

    _date_event = factory.get_string_flyweight(date_event);
}


std::shared_ptr<string_flyweight> session_data::get_time_event() const
{
    return _time_event;
}

void session_data::set_time_event(const std::string &time_event)
{
    string_flyweight_factory &factory = string_flyweight_factory::get_instance();

    _time_event = factory.get_string_flyweight(time_event);
}


std::shared_ptr<string_flyweight> session_data::get_mark() const
{
    return _mark;
}

void session_data::set_mark(const std::string &mark)
{
    string_flyweight_factory &factory = string_flyweight_factory::get_instance();

    _mark = factory.get_string_flyweight(mark);
}


std::shared_ptr<string_flyweight> session_data::get_surname_teacher() const
{
    return _surname_teacher;
}

void session_data::set_surname_teacher(const std::string &surname_teacher)
{
    string_flyweight_factory &factory = string_flyweight_factory::get_instance();

    _surname_teacher = factory.get_string_flyweight(surname_teacher);
}


std::shared_ptr<string_flyweight> session_data::get_name_teacher() const
{
    return _name_teacher;
}

void session_data::set_name_teacher(const std::string &name_teacher)
{
    string_flyweight_factory &factory = string_flyweight_factory::get_instance();

    _name_teacher = factory.get_string_flyweight(name_teacher);
}


std::shared_ptr<string_flyweight> session_data::get_patronymic_teacher() const
{
    return _patronymic_teacher;
}

void session_data::set_patronymic_teacher(const std::string &patronymic_teacher)
{
    string_flyweight_factory &factory = string_flyweight_factory::get_instance();

    _patronymic_teacher = factory.get_string_flyweight(patronymic_teacher);
}