#ifndef SESSION_DATA_H
#define SESSION_DATA_H

#include "string_flyweight.h"

class session_data
{

private:

    std::shared_ptr<string_flyweight> _surname_student;
    std::shared_ptr<string_flyweight> _name_student;
    std::shared_ptr<string_flyweight> _patronymic_student;
    std::shared_ptr<string_flyweight> _date_event;
    std::shared_ptr<string_flyweight> _time_event;
    std::shared_ptr<string_flyweight> _mark;
    std::shared_ptr<string_flyweight> _surname_teacher;
    std::shared_ptr<string_flyweight> _name_teacher;
    std::shared_ptr<string_flyweight> _patronymic_teacher;


public:

    std::shared_ptr<string_flyweight> get_surname_student() const;
    void set_surname_student(const std::string &surname_student);


    std::shared_ptr<string_flyweight> get_name_student() const;
    void set_name_student(const std::string &name_student);


    std::shared_ptr<string_flyweight> get_patronymic_student() const;
    void set_patronymic_student(const std::string &patronymic_student);


    std::shared_ptr<string_flyweight> get_date_event() const;
    void set_date_event(const std::string &date_event);


    std::shared_ptr<string_flyweight> get_time_event() const;
    void set_time_event(const std::string &time_event);

    std::shared_ptr<string_flyweight> get_mark() const;
    void set_mark(const std::string &mark);


    std::shared_ptr<string_flyweight> get_surname_teacher() const;
    void set_surname_teacher(const std::string &surname_teacher);


    std::shared_ptr<string_flyweight> get_name_teacher() const;
    void set_name_teacher(const std::string &name_teacher);


    std::shared_ptr<string_flyweight> get_patronymic_teacher() const;
    void set_patronymic_teacher(const std::string &patronymic_teacher);
};

#endif