#include "validation_data.h"

// число и > 0
bool check_id(const std::string &id)
{
    if (id.empty())
    {
        return false;
    }

    for (char c: id)
    {
        if (!std::isdigit(c)) // Строка содержит нечисловой символ
        {
            return false;
        }
    }

    int num = std::stoi(id);
    if (num >= 0)
    {
        return true;
    }

    return false;
}

// 3 формата: Курсовая, Экзамен и Зачёт
bool check_reporting_form(const std::string &reporting_form)
{
    if (reporting_form == "Coursework"  || reporting_form == "Test" || reporting_form == "Exam")
    {
        return true;
    }

    return false;
}

// непустое, тк используется в ключе
bool check_name_subject(const std::string &name_subject)
{
    if (name_subject.empty())
    {
        return false;
    }
    return true;
}

// любое
bool check_surname_student(const std::string &surname_student)
{
    return true;
}

// любое
bool check_name_student(const std::string &name_student)
{
    return true;
}

// любое
bool check_patronymic_student(const std::string &patronymic_student)
{
    return true;
}

// любое, тк можно ввести 02/09, или 02/09/2023 или September 2
bool check_date_event(const std::string &date_event)
{
    return true;
}

// любое, тк можно ввести 10:15, или 10:15:00, или 10:15 am/pm
bool check_time_event(const std::string &time_event)
{
    return true;
}

// в самих функциях
bool check_mark(const std::string &mark)
{
    return true;
}

// любое
bool check_surname_teacher(const std::string &surname_teacher)
{
    return true;
}

// любое
bool check_name_teacher(const std::string &name_teacher)
{
    return true;
}

// любое
bool check_patronymic_teacher(const std::string &patronymic_teacher)
{
    return true;
}
