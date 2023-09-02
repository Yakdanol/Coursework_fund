#include "string_flyweight.h"

const std::string &string_flyweight::get_value() const
{
    return _value;
}

void string_flyweight::set_value(const std::string &value)
{
    _value = value;
}

string_flyweight_factory &string_flyweight_factory::get_instance()
{
    static string_flyweight_factory instance; // паттерн одиночка
    return instance;
}

// возвращает умный указатель на объект легковесной строки
std::shared_ptr<string_flyweight> string_flyweight_factory::get_string_flyweight(const std::string &value)
{
    auto iterator = _string_flyweights.find(value);

    // если объект с таким значением найден, то возвращаем указатель на этот объект
    if (iterator != _string_flyweights.end())
    {
        return iterator->second;
    }

    // иначе создаем новый объект
    else
    {
        auto flyweight = std::make_shared<string_flyweight>();
        flyweight->set_value(value);
        _string_flyweights[value] = flyweight;

        return flyweight;
    }
}
