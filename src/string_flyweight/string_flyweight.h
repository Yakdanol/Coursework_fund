#ifndef STRING_FLYWEIGHT_H
#define STRING_FLYWEIGHT_H

#include <iostream>
#include <unordered_map>
#include <memory>

class string_flyweight
{

private:

    std::string _value;

public:

    const std::string &get_value() const;

    void set_value(const std::string &value);
};

class string_flyweight_factory
{

private:

    std::unordered_map<std::string, std::shared_ptr<string_flyweight>> _string_flyweights;

public:

    static string_flyweight_factory &get_instance();

    std::shared_ptr<string_flyweight> get_string_flyweight(const std::string &value);
};

#endif //STRING_FLYWEIGHT_H