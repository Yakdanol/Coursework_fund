#ifndef ASSOCIATIVE_CONTAINER_H
#define ASSOCIATIVE_CONTAINER_H

#include <utility>
#include <iostream>
#include <vector>

template<
        typename tkey,
        typename tvalue>
class associative_container
{

public:

    struct key_value_struct
    {
        tkey _key;
        tvalue _value;
    };

    virtual bool find(key_value_struct *target_key_and_result_value) = 0;

    virtual void insert(
            tkey const &key,
            tvalue value) = 0;

    virtual tvalue remove(tkey const &key) = 0;

    virtual void update(
            tkey const &key,
            tvalue value) = 0;

    tvalue get(tkey const &key);

    virtual std::vector<tvalue> get_range(
            tkey const &lower_bound,
            tkey const &upper_bound) const = 0;

    bool in(tkey const &key);

    bool operator[](key_value_struct *target_key_and_result_value);

    void operator+=(key_value_struct pair);

    tvalue operator-=(key_value_struct pair);

    tvalue operator-=(tkey const &key);

    virtual ~associative_container() = default;

};

template<
        typename tkey,
        typename tvalue>
tvalue associative_container<tkey, tvalue>::get(tkey const &key)
{
    key_value_struct pair;
    pair._key = key;

    if ((*this)[&pair])
    {
        return pair._value;
    }

    return tvalue();
}

template<
        typename tkey,
        typename tvalue>
bool associative_container<tkey, tvalue>::in(tkey const &key)
{
    key_value_struct pair;
    pair._key = key;

    // поиск в контейнере по ключу key_value_struct
    return (*this)[&pair];
}

template<
        typename tkey,
        typename tvalue>
void associative_container<tkey, tvalue>::operator+=(key_value_struct pair)
{
    return insert(pair._key, std::move(pair._value));
}

template<
        typename tkey,
        typename tvalue>
bool associative_container<tkey, tvalue>::operator[](key_value_struct *target_key_and_result_value)
{
    return find(target_key_and_result_value);
}

template<
        typename tkey,
        typename tvalue>
tvalue associative_container<tkey, tvalue>::operator-=(key_value_struct pair)
{
    return remove(pair._key);
}

template<
        typename tkey,
        typename tvalue>
tvalue associative_container<tkey, tvalue>::operator-=(tkey const &key)
{
    return remove(key);
}

#endif //ASSOCIATIVE_CONTAINER_H
