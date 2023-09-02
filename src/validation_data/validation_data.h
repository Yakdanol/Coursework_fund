#ifndef VALIDATION_DATA_H
#define VALIDATION_DATA_H

#include <iostream>
#include <string>
#include <cctype>

bool check_id(const std::string &id);

bool check_reporting_form(const std::string &reporting_form);

bool check_name_subject(const std::string & name_subject);

bool check_surname_student(const std::string &surname_student);

bool check_name_student(const std::string &name_student);

bool check_patronymic_student(const std::string &patronymic_student);

bool check_date_event(const std::string &date_event);

bool check_time_event(const std::string &time_event);

bool check_mark(const std::string &mark);

bool check_surname_teacher(const std::string &surname_teacher);

bool check_name_teacher(const std::string &name_teacher);

bool check_patronymic_teacher(const std::string &patronymic_teacher);

#endif // VALIDATION_DATA_H