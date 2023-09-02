#ifndef BINARY_SEARCH_TREE_H
#define BINARY_SEARCH_TREE_H

#include "associative_container.h"
#include "memory_holder.h"
#include "logger_holder.h"
#include "logger.h"
#include <stack>
#include <sstream>
#include <iostream>

template <typename T>
std::string to_string(T object)
{
    std::stringstream stream;
    stream << object;

    return stream.str();
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
    class binary_search_tree : public associative_container<tkey, tvalue>, protected memory_holder, protected logger_holder
{

protected:

    struct bin_node
    {
        tkey key;
        tvalue value;
        bin_node *left_subtree;
        bin_node *right_subtree;
        bool is_left_child;
        bool is_right_child;
    };

public:

    class prefix_bypass final
    {

    private:

        binary_search_tree<tkey, tvalue, tkey_comparer> *_tree;
        bin_node *_current_bin_node;
        bin_node *_end_bin_node;
        std::stack<bin_node *> _way;

    public:

        explicit prefix_bypass(
                binary_search_tree<tkey, tvalue, tkey_comparer> *tree,
                bin_node *current_bin_node);

        bool operator==(prefix_bypass const &other) const;

        bool operator!=(prefix_bypass const &other) const;

        prefix_bypass& operator++();

        prefix_bypass operator++(int for_work);

        std::tuple<unsigned int, tkey const&, tvalue&> operator*() const;

    };


    class infix_bypass final
    {

    private:

        binary_search_tree<tkey, tvalue, tkey_comparer> *_tree;
        bin_node *_current_bin_node;
        bin_node *_end_bin_node;
        std::stack<bin_node *> _way;

    public:

        explicit infix_bypass(
                binary_search_tree<tkey, tvalue, tkey_comparer> *tree,
                bin_node *current_bin_node);

        bool operator==(infix_bypass const &other) const;

        bool operator!=(infix_bypass const &other) const;

        infix_bypass& operator++();

        infix_bypass operator++(int for_work);

        std::tuple<unsigned int, tkey const&, tvalue&> operator*() const;

    };


    class postfix_bypass final
    {

        friend class binary_search_tree<tkey, tvalue, tkey_comparer>;

    private:

        binary_search_tree<tkey, tvalue, tkey_comparer> *_tree;
        bin_node *_current_bin_node;
        bin_node *_end_bin_node;
        std::stack<bin_node *> _way;

    public:

        explicit postfix_bypass(
                binary_search_tree<tkey, tvalue, tkey_comparer> *tree,
                bin_node *current_bin_node);

        bool operator==(postfix_bypass const &other) const;

        bool operator!=(postfix_bypass const &other) const;

        postfix_bypass &operator++();

        postfix_bypass operator++(int for_work);

        std::tuple<unsigned int, tkey const&, tvalue&> operator*() const;

    private:

        bin_node *get_node() const;

    };


protected:

    class bin_find_template_method
    {

    private:

        binary_search_tree<tkey, tvalue, tkey_comparer> *_tree;

    public:

        bool find(
                typename associative_container<tkey, tvalue>::key_value_struct *target_key_and_result_value,
                bin_node *&subtree_address);

        explicit bin_find_template_method(binary_search_tree<tkey, tvalue, tkey_comparer> *tree);

    private:

        bool find_concrete(
                typename associative_container<tkey, tvalue>::key_value_struct *target_key_and_result_value,
                bin_node *&subtree_address,
                std::stack<binary_search_tree::bin_node**> &path_to_subtree_root_exclusive);

    protected:

        virtual void after_find_concrete(
                typename associative_container<tkey, tvalue>::key_value_struct *target_key_and_result_value,
                bin_node *&subtree_address,
                std::stack<binary_search_tree::bin_node**> &path_to_subtree_root_exclusive);
    };


    class bin_insertion_template_method : private memory_holder, private logger_holder
    {

    private:

        binary_search_tree<tkey, tvalue, tkey_comparer> *_tree;

    private:

        memory *get_memory() const override;

        logger *get_logger() const override;

        virtual size_t get_node_size() const;

    public:

        explicit bin_insertion_template_method(binary_search_tree<tkey, tvalue, tkey_comparer> *tree);

        void insert(
                tkey const &key,
                tvalue value);

    private:

        void insert_concrete(
                tkey const &key,
                tvalue value,
                bin_node *&subtree_root_address,
                std::stack<binary_search_tree::bin_node**> &path_to_subtree_root_exclusive);

    protected:

        virtual void after_insert_concrete(
                tkey const &key,
                bin_node *&subtree_root_address,
                std::stack<binary_search_tree::bin_node**> &path_to_subtree_root_exclusive);
    };


    class bin_removing_template_method : private memory_holder, private logger_holder
    {

    private:

        binary_search_tree<tkey, tvalue, tkey_comparer> *_tree;

    private:

        memory *get_memory() const override;

        logger *get_logger() const override;

        virtual void get_info_deleted_node(
                bin_node *deleted_node,
                std::stack<binary_search_tree::bin_node**> &path_to_subtree_root_exclusive);

    public:

        explicit bin_removing_template_method(binary_search_tree<tkey, tvalue, tkey_comparer> *tree);

        tvalue remove(
                tkey const &key,
                bin_node *&tree_root_address);

    private:

        virtual tvalue remove_concrete(
                tkey const &key,
                bin_node *&subtree_root_address,
                std::stack<binary_search_tree::bin_node**> &path_to_subtree_root_exclusive);

    protected:

        virtual void after_remove_concrete(
                tkey const &key,
                bin_node *&subtree_root_address,
                std::stack<binary_search_tree::bin_node**> &path_to_subtree_root_exclusive);

    };

protected:

    tkey_comparer _comparator;
    bin_node *_root;
    memory *_allocator{};
    logger *_logger{};
    bin_find_template_method *_find;
    bin_insertion_template_method *_insertion;
    bin_removing_template_method *_removing;

public:

    explicit binary_search_tree(
            memory *allocator_binary_search_tree = nullptr,
            logger *logger_tree = nullptr);

    ~binary_search_tree() override;

private:

        void delete_tree(bin_node *&subtree_root_address);

protected:

    binary_search_tree(
            bin_find_template_method *find,
            bin_insertion_template_method *insert,
            bin_removing_template_method *remove,
            memory *allocator_binary_search_tree = nullptr,
            logger *logger_tree = nullptr,
            bin_node *root = nullptr);

public:

    void insert(
            tkey const &key,
            tvalue value) override;

    void update(
            tkey const &key,
            tvalue value) override;

    std::vector<tvalue> get_range(
            tkey const &lower_bound,
            tkey const &upper_bound) const override;

    bool find(typename associative_container<tkey, tvalue>::key_value_struct *target_key_and_result_value) override;

    tvalue remove(tkey const &key) override;

public:

    prefix_bypass begin_prefix() const noexcept;

    prefix_bypass end_prefix() const noexcept;

    infix_bypass begin_infix() const noexcept;

    infix_bypass end_infix() const noexcept;

    postfix_bypass begin_postfix() const noexcept;

    postfix_bypass end_postfix() const noexcept;

protected:

    virtual void left_rotate(
            bin_node **subtree_root,
            bin_node **parent) const;

    virtual void right_rotate(
            bin_node **subtree_root,
            bin_node **parent) const;

private:

    memory *get_memory() const final;

    logger *get_logger() const final;

private:

    void copy_subtree(bin_node *src);

public:

    binary_search_tree(const binary_search_tree<tkey, tvalue, tkey_comparer> &target_copy); // конструктор копирования

    binary_search_tree& operator=(const binary_search_tree<tkey, tvalue, tkey_comparer> &target_copy); // оператор присваивания копированием

    binary_search_tree(binary_search_tree<tkey, tvalue, tkey_comparer> &&target_copy) noexcept; // конструктор перемещения

    binary_search_tree& operator=(binary_search_tree<tkey, tvalue, tkey_comparer> &&target_copy) noexcept; // оператор присваивания перемещения
};


template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
binary_search_tree<tkey, tvalue, tkey_comparer>::prefix_bypass::prefix_bypass(
        binary_search_tree<tkey, tvalue, tkey_comparer> *tree,
        bin_node *current_bin_node)
{
    _tree = tree;

    bin_node *end_bin_node = _tree->_root;

    if (end_bin_node != nullptr)
    {
        while (end_bin_node->right_subtree != nullptr)
        {
            end_bin_node = end_bin_node->right_subtree;
        }
    }

    _end_bin_node = end_bin_node;

    if (current_bin_node != nullptr)
    {
        _current_bin_node = current_bin_node;

        bin_node *bin_node_search = _tree->_root;

        while (bin_node_search != current_bin_node)
        {
            _way.push(bin_node_search);

            if (_tree->_comparator(current_bin_node->key, bin_node_search->key) > 0)
            {
                bin_node_search = bin_node_search->right_subtree;
            }
            else
            {
                bin_node_search = bin_node_search->left_subtree;
            }
        }
    }

    else
    {
        _current_bin_node = nullptr;
        _way = std::stack<bin_node*>();
    }
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
bool binary_search_tree<tkey, tvalue, tkey_comparer>::prefix_bypass::operator==(
        prefix_bypass const &other) const
{
    if (_way == other._way && _current_bin_node == other._current_bin_node)
    {
        return true;
    }

    return false;
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
bool binary_search_tree<tkey, tvalue, tkey_comparer>::prefix_bypass::operator!=(
        prefix_bypass const &other) const
{
    return !(*this == other);
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
typename binary_search_tree<tkey, tvalue, tkey_comparer>::prefix_bypass &binary_search_tree<tkey, tvalue, tkey_comparer>::prefix_bypass::operator++()
{
    if (_current_bin_node == _end_bin_node)
    {
        _current_bin_node = nullptr;
        _way = std::stack<bin_node*>();

        return *this;
    }

    if (_current_bin_node->left_subtree != nullptr)
    {
        _way.push(_current_bin_node);
        _current_bin_node = _current_bin_node->left_subtree;

        return *this;
    }

    if (_current_bin_node->left_subtree == nullptr &&
        _current_bin_node->right_subtree != nullptr)
    {
        _way.push(_current_bin_node);
        _current_bin_node = _current_bin_node->right_subtree;

        return  *this;
    }

    if (_current_bin_node->left_subtree == nullptr &&
        _current_bin_node->right_subtree == nullptr)
    {
        while (!_way.empty() &&
               (_way.top()->right_subtree == _current_bin_node ||
                _way.top()->right_subtree == nullptr))
        {
            _current_bin_node = _way.top();
            _way.pop();
        }

        if (!_way.empty())
        {
            _current_bin_node = _way.top()->right_subtree;
        }

        return *this;
    }
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
typename binary_search_tree<tkey, tvalue, tkey_comparer>::prefix_bypass binary_search_tree<tkey, tvalue, tkey_comparer>::prefix_bypass::operator++(
        int for_work)
{
    typename binary_search_tree<tkey, tvalue, tkey_comparer>::prefix_bypass previous_state = *this;
    ++(*this);

    return previous_state;
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
std::tuple<unsigned int, tkey const&, tvalue&> binary_search_tree<tkey, tvalue, tkey_comparer>::prefix_bypass::operator*() const
{
    return std::tuple<unsigned int, tkey const&, tvalue&>(_way.size(), _current_bin_node->key, _current_bin_node->value);
}


template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
binary_search_tree<tkey, tvalue, tkey_comparer>::infix_bypass::infix_bypass(
        binary_search_tree<tkey, tvalue, tkey_comparer> *tree,
        bin_node *current_bin_node)
{
    _tree = tree;

    bin_node *end_bin_node = _tree->_root;

    if (end_bin_node != nullptr)
    {
        while (end_bin_node->right_subtree != nullptr)
        {
            end_bin_node = end_bin_node->right_subtree;
        }
    }

    _end_bin_node = end_bin_node;

    if (current_bin_node != nullptr)
    {
        tkey_comparer functor;
        _current_bin_node = current_bin_node;

        bin_node *bin_node_search = _tree->_root;

        while (bin_node_search != current_bin_node)
        {
            _way.push(bin_node_search);

            if (_tree->_comparator(current_bin_node->key, bin_node_search->key) > 0)
            {
                bin_node_search = bin_node_search->right_subtree;
            }

            else
            {
                bin_node_search = bin_node_search->left_subtree;
            }
        }
    }

    else
    {
        _current_bin_node = nullptr;
        _way = std::stack<bin_node*>();
    }
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
bool binary_search_tree<tkey, tvalue, tkey_comparer>::infix_bypass::operator==(infix_bypass const &other) const
{
    if (_way == other._way && _current_bin_node == other._current_bin_node)
    {
        return true;
    }

    return false;
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
bool binary_search_tree<tkey, tvalue, tkey_comparer>::infix_bypass::operator!=(infix_bypass  const &other) const
{
    return !(*this == other);
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
typename binary_search_tree<tkey, tvalue, tkey_comparer>::infix_bypass &binary_search_tree<tkey, tvalue, tkey_comparer>::infix_bypass::operator++()
{
    if (_current_bin_node == _end_bin_node)
    {
        _current_bin_node = nullptr;
        _way = std::stack<bin_node*>();

        return *this;
    }

    if (_current_bin_node->right_subtree != nullptr)
    {
        _way.push(_current_bin_node);
        _current_bin_node = _current_bin_node->right_subtree;

        while (_current_bin_node->left_subtree != nullptr)
        {
            _way.push(_current_bin_node);
            _current_bin_node = _current_bin_node->left_subtree;
        }
    }

    else
    {
        if (!_way.empty())
        {
            if (_way.top()->left_subtree == _current_bin_node)
            {
                _current_bin_node = _way.top();
                _way.pop();
            }

            else
            {
                while (!_way.empty() &&
                       _way.top()->right_subtree == _current_bin_node)
                {
                    _current_bin_node = _way.top();
                    _way.pop();
                }

                if (!_way.empty())
                {
                    _current_bin_node = _way.top();
                    _way.pop();
                }
            }
        }
    }

    return *this;
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
typename binary_search_tree<tkey, tvalue, tkey_comparer>::infix_bypass binary_search_tree<tkey, tvalue, tkey_comparer>::infix_bypass::operator++(
        int for_work)
{
    typename binary_search_tree<tkey, tvalue, tkey_comparer>::infix_bypass previous_state = *this;
    ++(*this);

    return previous_state;
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
std::tuple<unsigned int, tkey const&, tvalue&> binary_search_tree<tkey, tvalue, tkey_comparer>::infix_bypass::operator*() const
{
    return std::tuple<unsigned int, tkey const&, tvalue&>(_way.size(), _current_bin_node->key, _current_bin_node->value);
}


template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
binary_search_tree<tkey, tvalue, tkey_comparer>::postfix_bypass::postfix_bypass(
        binary_search_tree<tkey, tvalue, tkey_comparer> *tree,
        bin_node *current_bin_node)
{
    _tree = tree;
    _end_bin_node = _tree->_root;

    if (current_bin_node != nullptr)
    {
        tkey_comparer functor;
        _current_bin_node = current_bin_node;

        bin_node *bin_node_search = _tree->_root;

        while (bin_node_search != current_bin_node)
        {
            _way.push(bin_node_search);

            if (_tree->_comparator(current_bin_node->key, bin_node_search->key) > 0)
            {
                bin_node_search = bin_node_search->right_subtree;
            }
            else
            {
                bin_node_search = bin_node_search->left_subtree;
            }
        }
    }
    else
    {
        _current_bin_node = nullptr;
        _way = std::stack<bin_node*>();
    }
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
bool binary_search_tree<tkey, tvalue, tkey_comparer>::postfix_bypass::operator==(
        postfix_bypass const &other) const
{
    if (_way == other._way && _current_bin_node == other._current_bin_node)
    {
        return true;
    }

    return false;
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
bool binary_search_tree<tkey, tvalue, tkey_comparer>::postfix_bypass::operator!=(
        postfix_bypass  const &other) const
{
    return !(*this == other);
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
typename binary_search_tree<tkey, tvalue, tkey_comparer>::postfix_bypass &binary_search_tree<tkey, tvalue, tkey_comparer>::postfix_bypass::operator++()
{
    if (_current_bin_node == nullptr)
    {
        return *this;
    }

    if (_way.empty())
    {
        _current_bin_node = nullptr;
    }

    else
    {
        if (_way.top()->right_subtree == _current_bin_node)
        {
            _current_bin_node = _way.top();
            _way.pop();
        }

        else
        {
            if ((_current_bin_node = _way.top()->right_subtree) != nullptr)
            {
                while (_current_bin_node->left_subtree != nullptr ||
                       _current_bin_node->right_subtree != nullptr)
                {
                    _way.push(_current_bin_node);
                    _current_bin_node = _current_bin_node->left_subtree == nullptr
                                        ? _current_bin_node->right_subtree
                                        : _current_bin_node->left_subtree;
                }
            }

            else
            {
                _current_bin_node = _way.top();
                _way.pop();
            }
        }
    }

    return *this;
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
typename binary_search_tree<tkey, tvalue, tkey_comparer>::postfix_bypass binary_search_tree<tkey, tvalue, tkey_comparer>::postfix_bypass::operator++(
        int for_work)
{
    typename binary_search_tree<tkey, tvalue, tkey_comparer>::postfix_bypass previous_state = *this;
    ++(*this);

    return previous_state;
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
std::tuple<unsigned int, tkey const&, tvalue&> binary_search_tree<tkey, tvalue, tkey_comparer>::postfix_bypass::operator*() const
{
    return std::tuple<unsigned int, tkey const&, tvalue&>(_way.size(), _current_bin_node->key, _current_bin_node->value);
}


template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
typename binary_search_tree<tkey, tvalue, tkey_comparer>::bin_node *binary_search_tree<tkey, tvalue, tkey_comparer>::postfix_bypass::get_node() const
{
    return _current_bin_node;
}


template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
bool binary_search_tree<tkey, tvalue, tkey_comparer>::bin_find_template_method::find(
        typename associative_container<tkey, tvalue>::key_value_struct *target_key_and_result_value,
        bin_node *&subtree_address)
{
    std::stack<bin_node**> path_to_subtree_root_exclusive;

    return find_concrete(target_key_and_result_value, subtree_address, path_to_subtree_root_exclusive);
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
binary_search_tree<tkey, tvalue, tkey_comparer>::bin_find_template_method::bin_find_template_method(
        binary_search_tree<tkey, tvalue, tkey_comparer> *tree) :
        _tree(tree)
{

}

// пофиксить
template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
bool binary_search_tree<tkey, tvalue, tkey_comparer>::bin_find_template_method::find_concrete(
        typename associative_container<tkey, tvalue>::key_value_struct *target_key_and_result_value,
        bin_node *&subtree_address,
        std::stack<binary_search_tree::bin_node**> &path_to_subtree_root_exclusive)
{
     if (subtree_address == nullptr)
    {
        if (!path_to_subtree_root_exclusive.empty())
        {
            bin_node **last_node = path_to_subtree_root_exclusive.top();
            path_to_subtree_root_exclusive.pop();

            after_find_concrete(target_key_and_result_value, *last_node, path_to_subtree_root_exclusive);
        }

        return false;
    }

    tkey_comparer functor;
    int res_functor = functor(target_key_and_result_value->_key, subtree_address->key);

    if (res_functor == 0)
    {
        target_key_and_result_value->_value = subtree_address->value;

        after_find_concrete(target_key_and_result_value, subtree_address, path_to_subtree_root_exclusive);

        return true;
    }

    // если дошли до листа, а такого ключа нет
    if (subtree_address->left_subtree == nullptr &&
        subtree_address->right_subtree == nullptr)
    {
        return false;
    }

    bin_node *next_bin_node = res_functor > 0 ? subtree_address->right_subtree : subtree_address->left_subtree;
    path_to_subtree_root_exclusive.push(&subtree_address);

    return find_concrete(target_key_and_result_value, next_bin_node, path_to_subtree_root_exclusive);
}

// только для splay
template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
void binary_search_tree<tkey, tvalue, tkey_comparer>::bin_find_template_method::after_find_concrete(
        typename associative_container<tkey, tvalue>::key_value_struct *target_key_and_result_value,
        bin_node *&subtree_address,
        std::stack<binary_search_tree::bin_node**> &path_to_subtree_root_exclusive)
{

}


template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
memory *binary_search_tree<tkey, tvalue, tkey_comparer>::bin_insertion_template_method::get_memory() const
{
    return _tree->_allocator;
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
logger *binary_search_tree<tkey, tvalue, tkey_comparer>::bin_insertion_template_method::get_logger() const
{
    return _tree->_logger;
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
size_t binary_search_tree<tkey, tvalue, tkey_comparer>::bin_insertion_template_method::get_node_size() const
{
    return sizeof(bin_node);
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
binary_search_tree<tkey, tvalue, tkey_comparer>::bin_insertion_template_method::bin_insertion_template_method(
        binary_search_tree<tkey, tvalue, tkey_comparer> *tree) :
        _tree(tree)
{

}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
void binary_search_tree<tkey, tvalue, tkey_comparer>::bin_insertion_template_method::insert(
        const tkey &key,
        tvalue value)
{
    std::stack<binary_search_tree::bin_node**> path_to_subtree_root_exclusive;
    insert_concrete(key, std::move(value), _tree->_root, path_to_subtree_root_exclusive);
}


template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
void binary_search_tree<tkey, tvalue, tkey_comparer>::bin_insertion_template_method::insert_concrete(
        const tkey &key,
        tvalue value,
        binary_search_tree::bin_node *&subtree_root_address,
        std::stack<binary_search_tree::bin_node**> &path_to_subtree_root_exclusive)
{
    if (subtree_root_address == nullptr)
    {
        subtree_root_address = reinterpret_cast<bin_node*>(guard_allocate(get_node_size()));
        new (subtree_root_address) bin_node{key, std::move(value), nullptr, nullptr};

        if (!path_to_subtree_root_exclusive.empty())
        {
            if (_tree->_comparator(key, (*path_to_subtree_root_exclusive.top())->key) > 0)
            {
                (*path_to_subtree_root_exclusive.top())->right_subtree = subtree_root_address;
            }

            else
            {
                (*path_to_subtree_root_exclusive.top())->left_subtree = subtree_root_address;
            }
        }

        after_insert_concrete(key, subtree_root_address, path_to_subtree_root_exclusive);

        return;
    }

    else
    {
        if (_tree->_comparator(key, subtree_root_address->key) == 0)
        {
            subtree_root_address->value = value;
            return;
        }
    }

    binary_search_tree::bin_node *bin_node_next = _tree->_comparator(key, subtree_root_address->key) > 0 ?
                                                  subtree_root_address->right_subtree : subtree_root_address->left_subtree;
    path_to_subtree_root_exclusive.push(&subtree_root_address);
    insert_concrete(key, std::move(value), bin_node_next, path_to_subtree_root_exclusive);
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
void binary_search_tree<tkey, tvalue, tkey_comparer>::bin_insertion_template_method::after_insert_concrete(
        tkey const &key,
        bin_node *&subtree_root_address,
        std::stack<binary_search_tree::bin_node**> &path_to_subtree_root_exclusive)
{

}


template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
memory *binary_search_tree<tkey, tvalue, tkey_comparer>::bin_removing_template_method::get_memory() const
{
    return _tree->_allocator;
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
logger *binary_search_tree<tkey, tvalue, tkey_comparer>::bin_removing_template_method::get_logger() const
{
    return _tree->_logger;
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
void binary_search_tree<tkey, tvalue, tkey_comparer>::bin_removing_template_method::get_info_deleted_node(
        bin_node *deleted_node,
        std::stack<binary_search_tree::bin_node**> &path_to_subtree_root_exclusive)
{

}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
binary_search_tree<tkey, tvalue, tkey_comparer>::bin_removing_template_method::bin_removing_template_method(
        binary_search_tree<tkey, tvalue, tkey_comparer> *tree) :
        _tree(tree)
{

}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
tvalue binary_search_tree<tkey, tvalue, tkey_comparer>::bin_removing_template_method::remove(
        tkey const &key,
        bin_node *&tree_root_address)
{
    std::stack<binary_search_tree::bin_node**> path_to_subtree_root_exclusive;

    return remove_concrete(key, tree_root_address, path_to_subtree_root_exclusive);
}


template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
tvalue binary_search_tree<tkey, tvalue, tkey_comparer>::bin_removing_template_method::remove_concrete(
        tkey const &key,
        bin_node *&subtree_root_address, // текущий элемент (корень поддерева)
        std::stack<binary_search_tree::bin_node**> &path_to_subtree_root_exclusive)
{
    // переданы неверные аргументы
    if (subtree_root_address == nullptr)
    {
        throw std::invalid_argument("Bad argument to delete");
    }

    if (_tree->_comparator(key, subtree_root_address->key) == 0)
    {
        tvalue removing_value = subtree_root_address->value;

        if (subtree_root_address->left_subtree == nullptr &&
            subtree_root_address->right_subtree == nullptr)
        {
            get_info_deleted_node(subtree_root_address, path_to_subtree_root_exclusive);

            if (!path_to_subtree_root_exclusive.empty())
            {
                if ((*path_to_subtree_root_exclusive.top())->right_subtree == subtree_root_address)
                {
                    (*path_to_subtree_root_exclusive.top())->right_subtree = nullptr;
                }
                else
                {
                    (*path_to_subtree_root_exclusive.top())->left_subtree = nullptr;
                }
            }

            subtree_root_address->~bin_node();
            guard_deallocate(subtree_root_address);
            subtree_root_address = nullptr;

            if (!path_to_subtree_root_exclusive.empty())
            {
                bin_node **temp = path_to_subtree_root_exclusive.top();
                path_to_subtree_root_exclusive.pop();
                after_remove_concrete(key, *temp, path_to_subtree_root_exclusive);
            }

            else
            {
                after_remove_concrete(key, _tree->_root, path_to_subtree_root_exclusive);
            }
        }

        else
        {
            if ((subtree_root_address->left_subtree == nullptr &&
                 subtree_root_address->right_subtree != nullptr) ||
                (subtree_root_address->left_subtree != nullptr &&
                 subtree_root_address->right_subtree == nullptr))
            {
                get_info_deleted_node(subtree_root_address, path_to_subtree_root_exclusive);
                bin_node *bin_node_replacement = subtree_root_address->left_subtree == nullptr ?
                                                 subtree_root_address->right_subtree : subtree_root_address->left_subtree;

                if (!path_to_subtree_root_exclusive.empty())
                {
                    if ((*path_to_subtree_root_exclusive.top())->right_subtree == subtree_root_address)
                    {
                        (*path_to_subtree_root_exclusive.top())->right_subtree = bin_node_replacement;
                    }
                    else
                    {
                        (*path_to_subtree_root_exclusive.top())->left_subtree = bin_node_replacement;
                    }
                }

                subtree_root_address->~bin_node();
                guard_deallocate(subtree_root_address);
                subtree_root_address = bin_node_replacement;

                if (!path_to_subtree_root_exclusive.empty())
                {
                    bin_node **temp = path_to_subtree_root_exclusive.top();
                    path_to_subtree_root_exclusive.pop();
                    after_remove_concrete(key, *temp, path_to_subtree_root_exclusive);
                }

                else
                {
                    after_remove_concrete(key, _tree->_root, path_to_subtree_root_exclusive);
                }
            }

            else
            {
                if (subtree_root_address->left_subtree != nullptr &&
                    subtree_root_address->right_subtree != nullptr)
                {

                    bin_node *max_bin_node = subtree_root_address->left_subtree;

                    while (max_bin_node->right_subtree != nullptr)
                    {
                        max_bin_node = max_bin_node->right_subtree;
                    }

                    tkey key_current = max_bin_node->key;
                    tvalue value_current = max_bin_node->value;

                    _tree->remove(key_current);

                    subtree_root_address->key = std::move(key_current);
                    subtree_root_address->value = std::move(value_current);
                }
            }
        }

        return removing_value;
    }

    binary_search_tree::bin_node *next_bin_node = _tree->_comparator(key, subtree_root_address->key) > 0 ? subtree_root_address->right_subtree : subtree_root_address->left_subtree;
    path_to_subtree_root_exclusive.push(&subtree_root_address);
    tvalue removing_value = remove_concrete(key, next_bin_node, path_to_subtree_root_exclusive);

    return removing_value;
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
void binary_search_tree<tkey, tvalue, tkey_comparer>::bin_removing_template_method::after_remove_concrete(
        tkey const &key,
        bin_node *&subtree_root_address, // корень удаленного узла
        std::stack<binary_search_tree::bin_node**> &path_to_subtree_root_exclusive)
{

}


template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
memory *binary_search_tree<tkey, tvalue, tkey_comparer>::get_memory() const
{
    return _allocator;
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
logger *binary_search_tree<tkey, tvalue, tkey_comparer>::get_logger() const
{
    return _logger;
}


template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
void binary_search_tree<tkey, tvalue, tkey_comparer>::insert(
        tkey const &key,
        tvalue value)
{
    return _insertion->insert(key, std::move(value));
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
void binary_search_tree<tkey, tvalue, tkey_comparer>::update(
        tkey const &key,
        tvalue value)
{
    return _insertion->insert(key, std::move(value));
}


template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
std::vector<tvalue> binary_search_tree<tkey, tvalue, tkey_comparer>::get_range(
        tkey const &lower_bound,
        tkey const &upper_bound) const
{
    std::vector<tvalue> result;

    for (auto iterator = begin_infix(); iterator != end_infix(); ++iterator)
    {
        if (_comparator(lower_bound, std::get<1>(*iterator)) <= 0 &&
            _comparator(upper_bound, std::get<1>(*iterator)) >= 0)
        {
            result.push_back(std::get<2>(*iterator));
        }
    }

    return result;
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
bool binary_search_tree<tkey, tvalue, tkey_comparer>::find(
        typename associative_container<tkey, tvalue>::key_value_struct *target_key_and_result_value)
{
    return _find->find(target_key_and_result_value, _root);
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
tvalue binary_search_tree<tkey, tvalue, tkey_comparer>::remove(
        tkey const &key)
{
    if (_root == nullptr)
    {
        throw std::invalid_argument("Tree is empty");
    }

    return _removing->remove(key, _root);
}


template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
void binary_search_tree<tkey, tvalue, tkey_comparer>::left_rotate(
        bin_node **subtree_root, // текущий корень (узел, который пойдет вниз)
        bin_node **parent) const
{
    bin_node *new_subtree_root = (*subtree_root)->right_subtree;
    (*subtree_root)->right_subtree = new_subtree_root->left_subtree;
    new_subtree_root->left_subtree = *subtree_root;

    if (parent != nullptr)
    {
        if ((*parent)->left_subtree == *subtree_root)
        {
            (*parent)->left_subtree = new_subtree_root;
        }

        else
        {
            (*parent)->right_subtree = new_subtree_root;
        }
    }

    *subtree_root = new_subtree_root;
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
void binary_search_tree<tkey, tvalue, tkey_comparer>::right_rotate(
        bin_node **subtree_root, // текущий корень (узел, который пойдет вниз)
        bin_node **parent) const
{
    bin_node *new_subtree_root = (*subtree_root)->left_subtree;
    (*subtree_root)->left_subtree = new_subtree_root->right_subtree;
    new_subtree_root->right_subtree = *subtree_root;

    if (parent != nullptr)
    {
        if ((*parent)->left_subtree == *subtree_root)
        {
            (*parent)->left_subtree = new_subtree_root;
        }

        else
        {
            (*parent)->right_subtree = new_subtree_root;
        }
    }

    *subtree_root = new_subtree_root;
}


template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
typename binary_search_tree<tkey, tvalue, tkey_comparer>::prefix_bypass binary_search_tree<tkey, tvalue, tkey_comparer>::begin_prefix() const noexcept
{
    return prefix_bypass(const_cast<binary_search_tree<tkey, tvalue, tkey_comparer>*>(this), _root);
}


template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
typename binary_search_tree<tkey, tvalue, tkey_comparer>::prefix_bypass binary_search_tree<tkey, tvalue, tkey_comparer>::end_prefix() const noexcept
{
    return prefix_bypass(const_cast<binary_search_tree<tkey, tvalue, tkey_comparer>*>(this), nullptr);
}


template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
typename binary_search_tree<tkey, tvalue, tkey_comparer>::infix_bypass binary_search_tree<tkey, tvalue, tkey_comparer>::begin_infix() const noexcept
{
    bin_node *current_bin_node = _root;

    if (current_bin_node != nullptr)
    {
        while (current_bin_node->left_subtree != nullptr)
        {
            current_bin_node = current_bin_node->left_subtree;
        }
    }

    return infix_bypass(const_cast<binary_search_tree<tkey, tvalue, tkey_comparer>*>(this), current_bin_node);
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
typename binary_search_tree<tkey, tvalue, tkey_comparer>::infix_bypass binary_search_tree<tkey, tvalue, tkey_comparer>::end_infix() const noexcept
{
    return infix_bypass(const_cast<binary_search_tree<tkey, tvalue, tkey_comparer>*>(this), nullptr);
}


template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
typename binary_search_tree<tkey, tvalue, tkey_comparer>::postfix_bypass binary_search_tree<tkey, tvalue, tkey_comparer>::begin_postfix() const noexcept
{
    bin_node *current_bin_node = _root;

    if (_root == nullptr)
    {
        return end_postfix();
    }

    if (_root->left_subtree != nullptr)
    {
        while (current_bin_node->left_subtree != nullptr)
        {
            current_bin_node = current_bin_node->left_subtree;
        }
    }
    else
    {
        while ((current_bin_node->left_subtree == nullptr) &&
               !(current_bin_node->left_subtree == nullptr && current_bin_node->right_subtree == nullptr))
        {
            current_bin_node = current_bin_node->right_subtree;
        }

        while (current_bin_node->left_subtree != nullptr)
        {
            current_bin_node = current_bin_node->left_subtree;
        }
    }

    return postfix_bypass(const_cast<binary_search_tree<tkey, tvalue, tkey_comparer>*>(this), current_bin_node);
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
typename binary_search_tree<tkey, tvalue, tkey_comparer>::postfix_bypass binary_search_tree<tkey, tvalue, tkey_comparer>::end_postfix() const noexcept
{
    return postfix_bypass(const_cast<binary_search_tree<tkey, tvalue, tkey_comparer>*>(this), nullptr);
}


template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
binary_search_tree<tkey, tvalue, tkey_comparer>::binary_search_tree(
        memory *allocator_binary_search_tree,
        logger *logger_tree) :
        _comparator(),
        _root(nullptr),
        _allocator(allocator_binary_search_tree),
        _logger(logger_tree),
        _find(new bin_find_template_method(this)),
        _insertion(new bin_insertion_template_method(this)),
        _removing(new bin_removing_template_method(this))
{
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
binary_search_tree<tkey, tvalue, tkey_comparer>::binary_search_tree(
        bin_find_template_method *find,
        bin_insertion_template_method *insert,
        bin_removing_template_method *remove,
        memory *allocator_binary_search_tree,
        logger *logger_tree,
        bin_node *root) :
        _comparator(),
        _root(root),
        _find(find),
        _insertion(insert),
        _removing(remove),
        _allocator(allocator_binary_search_tree),
        _logger(logger_tree)
{

}


template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
binary_search_tree<tkey, tvalue, tkey_comparer>::~binary_search_tree()
{
    delete_tree(_root);

    delete _find;
    delete _insertion;
    delete _removing;
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
void binary_search_tree<tkey, tvalue, tkey_comparer>::delete_tree(
        bin_node *&subtree_root_address)
{
    if (subtree_root_address != nullptr)
    {
        delete_tree(subtree_root_address->left_subtree);
        delete_tree(subtree_root_address->right_subtree);
        subtree_root_address->~bin_node();
        guard_deallocate(subtree_root_address);
        subtree_root_address = nullptr;
    }
}


template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
void binary_search_tree<tkey, tvalue, tkey_comparer>::copy_subtree(
        bin_node *src)
{
    if (src != nullptr)
    {
        tvalue value_copy = src->value;

        _insertion->insert(src->key, std::move(value_copy));
        copy_subtree(src->left_subtree);
        copy_subtree(src->right_subtree);
    }
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
binary_search_tree<tkey, tvalue, tkey_comparer>::binary_search_tree(
        const binary_search_tree<tkey, tvalue, tkey_comparer> &target_copy) :
        _comparator(target_copy._comparator),
        _root(nullptr),
        _allocator(target_copy._allocator),
        _logger(target_copy._logger),
        _find(target_copy._find),
        _insertion(target_copy._insertion),
        _removing(target_copy._removing)
{
    copy_subtree(target_copy._root);
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
binary_search_tree<tkey, tvalue, tkey_comparer>& binary_search_tree<tkey, tvalue, tkey_comparer>::operator=(
        const binary_search_tree<tkey, tvalue, tkey_comparer> &target_copy)
{
    _comparator = target_copy._comparator;
    _root = nullptr;
    _allocator = target_copy._allocator;
    _logger = target_copy._logger;
    _find = target_copy._find;
    _insertion = target_copy._insertion;
    _removing = target_copy._removing;

    copy_subtree(target_copy._root);

    return *this;
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
binary_search_tree<tkey, tvalue, tkey_comparer>::binary_search_tree(
        binary_search_tree<tkey, tvalue, tkey_comparer> &&target_copy) noexcept :
        _root(std::move(target_copy._root)),
        _comparator(std::move(target_copy._comparator)),
        _find(target_copy._find),
        _insertion(target_copy._insertion),
        _removing(target_copy._removing)
{
    target_copy._find = nullptr;
    target_copy._insertion = nullptr;
    target_copy._removing = nullptr;
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
binary_search_tree<tkey, tvalue, tkey_comparer>& binary_search_tree<tkey, tvalue, tkey_comparer>::operator=(
        binary_search_tree<tkey, tvalue, tkey_comparer> &&target_copy) noexcept
{
    _root = std::move(target_copy._root);
    _comparator = std::move(target_copy._comparator);
    _find = target_copy._find;
    _insertion = target_copy._insertion;
    _removing = target_copy._removing;

    target_copy._find = nullptr;
    target_copy._insertion = nullptr;
    target_copy._removing = nullptr;

    return *this;
}


#endif // BINARY_SEARCH_TREE_H