#ifndef COMPARATORS_H
#define COMPARATORS_H

#include <iostream>
#include "session_data.h"

struct Key
{
    std::size_t id_session;
    std::size_t id_student;
    std::string reporting_form;
    std::string name_subject;
};

class compare_str_keys
{

public:

    int operator()(
            const std::string &first_cmp,
            const std::string &second_cmp) const
    {
        return first_cmp.compare(second_cmp);
    }

};

class compare_data_keys
{

public:

    int operator()(
            const Key &first,
            const Key &second) const // сравнение ключей
    {
        if (first.id_session > second.id_session)
        {
            return 1;
        }

        if (first.id_session < second.id_session)
        {
            return -1;
        }

        if (first.id_student > second.id_student)
        {
            return 1;
        }

        if (first.id_student < second.id_student)
        {
            return -1;
        }

        int reportingComparison = first.reporting_form.compare(second.reporting_form);
        if (reportingComparison != 0)
        {
            return reportingComparison;
        }

        return first.name_subject.compare(second.name_subject);
    }
};

#endif //COMPARATORS_H