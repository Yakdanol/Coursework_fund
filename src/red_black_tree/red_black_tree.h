#ifndef RED_BLACK_TREE_H
#define RED_BLACK_TREE_H

#include "binary_search_tree.h"
#include <memory>

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
    class red_black_tree : public binary_search_tree<tkey, tvalue, tkey_comparer>
{

protected:

    enum class color_node
    {
        RED,
        BLACK
    };

    struct red_black_node : public binary_search_tree<tkey, tvalue, tkey_comparer>::bin_node
    {
        color_node color;
    };

protected:

    class red_black_insertion_template_method : public binary_search_tree<tkey, tvalue, tkey_comparer>::bin_insertion_template_method
    {

    private:

        size_t get_node_size() const override;

    private:

        red_black_tree<tkey, tvalue, tkey_comparer> *_tree;

    protected:

        void after_insert_concrete(
                tkey const &key,
                typename binary_search_tree<tkey, tvalue, tkey_comparer>::bin_node *&subtree_root_address,
                std::stack<typename binary_search_tree<tkey, tvalue, tkey_comparer>::bin_node**> &path_to_subtree_root_exclusive) override;

    public:

        explicit red_black_insertion_template_method(
                red_black_tree<tkey, tvalue, tkey_comparer> *tree);
    };

    class red_black_removing_template_method : public binary_search_tree<tkey, tvalue, tkey_comparer>::bin_removing_template_method
    {

    private:

        red_black_tree<tkey, tvalue, tkey_comparer> *_tree;
        std::unique_ptr<red_black_node> _information_deleted_node;

    private:

        void get_info_deleted_node(
                typename binary_search_tree<tkey, tvalue, tkey_comparer>::bin_node *deleted_node,
                std::stack<typename binary_search_tree<tkey, tvalue, tkey_comparer>::bin_node**> &path_to_subtree_root_exclusive) override;

    protected:

        void after_remove_concrete(
                tkey const &key,
                typename binary_search_tree<tkey, tvalue, tkey_comparer>::bin_node *&subtree_root_address,
                std::stack<typename binary_search_tree<tkey, tvalue, tkey_comparer>::bin_node**> &path_to_subtree_root_exclusive) override;

    public:

        explicit red_black_removing_template_method(
                red_black_tree<tkey, tvalue, tkey_comparer> *tree);

        ~red_black_removing_template_method();
    };

public:

    explicit red_black_tree(
            memory *allocator_binary_search_tree = nullptr,
            logger *logger_tree = nullptr);

    ~red_black_tree() final = default;

private:

    color_node get_color(
            red_black_node *current_node);

public:

    red_black_tree(const red_black_tree<tkey, tvalue, tkey_comparer> &target_copy); // конструктор копирования

    red_black_tree& operator=(const red_black_tree<tkey, tvalue, tkey_comparer> &target_copy); // оператор присваивания копированием

    red_black_tree(red_black_tree<tkey, tvalue, tkey_comparer> &&target_copy) noexcept; // конструктор перемещения

    red_black_tree& operator=(red_black_tree<tkey, tvalue, tkey_comparer> &&target_copy) noexcept; // оператор присваивания перемещения
};


template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
size_t red_black_tree<tkey, tvalue, tkey_comparer>::red_black_insertion_template_method::get_node_size() const
{
    return sizeof(red_black_node);
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
typename red_black_tree<tkey, tvalue, tkey_comparer>::color_node red_black_tree<tkey, tvalue, tkey_comparer>::get_color(
        red_black_node *current_node)
{
    return current_node == nullptr ? color_node::BLACK : current_node->color; // тк по умолчанию листы черные
}


// балансировка после вставки
template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
void red_black_tree<tkey, tvalue, tkey_comparer>::red_black_insertion_template_method::after_insert_concrete(
        tkey const &key,
        typename binary_search_tree<tkey, tvalue, tkey_comparer>::bin_node *&subtree_root_address,
        std::stack<typename binary_search_tree<tkey, tvalue, tkey_comparer>::bin_node**> &path_to_subtree_root_exclusive)
{
    // 1 случай - пустое дерево. Вставляем и перекрашиваем
    if (path_to_subtree_root_exclusive.empty())
    {
        reinterpret_cast<red_black_node*>(subtree_root_address)->color = red_black_tree<tkey, tvalue, tkey_comparer>::color_node::BLACK;

        return;
    }

    else
    {
        // если в дереве остался только корень, то оставляем его КРАСНЫМ, т.к. корень всегда красный
        if (subtree_root_address->left_subtree == nullptr &&
            subtree_root_address->right_subtree == nullptr)
        {
            reinterpret_cast<red_black_node*>(subtree_root_address)->color = red_black_tree<tkey, tvalue, tkey_comparer>::color_node::RED;
        }
    }

    red_black_node **parent = nullptr;
    red_black_node **uncle = nullptr;
    red_black_node **grand_parent = nullptr;
    red_black_node **great_grand_parent = nullptr;

    if (!path_to_subtree_root_exclusive.empty())
    {
        parent = reinterpret_cast<red_black_node**>(path_to_subtree_root_exclusive.top());
        path_to_subtree_root_exclusive.pop();

        if (!path_to_subtree_root_exclusive.empty())
        {
            grand_parent = reinterpret_cast<red_black_node**>(path_to_subtree_root_exclusive.top());
            path_to_subtree_root_exclusive.pop();

            if ((*grand_parent)->left_subtree == *parent)
            {
                uncle = reinterpret_cast<red_black_node**>(&((*grand_parent)->right_subtree));
            }
            else
            {
                uncle = reinterpret_cast<red_black_node**>(&((*grand_parent)->left_subtree));
            }

            if (!path_to_subtree_root_exclusive.empty())
            {
                great_grand_parent = reinterpret_cast<red_black_node**>(path_to_subtree_root_exclusive.top());
                path_to_subtree_root_exclusive.pop();
            }
        }
    }

    // 2 случай - отец черный. Кладем обратно цепочку до прадедушки в стек и выходим
    if (parent != nullptr &&
        _tree->get_color(*parent) == red_black_tree<tkey, tvalue, tkey_comparer>::color_node::BLACK)
    {
        if (great_grand_parent != nullptr)
        {
            path_to_subtree_root_exclusive.push(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::bin_node**>(great_grand_parent)); // кладем обратно в стек прадедушку
        }

        if (grand_parent != nullptr)
        {
            path_to_subtree_root_exclusive.push(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::bin_node**>(grand_parent)); // кладем обратно в стек дедушку
        }

        path_to_subtree_root_exclusive.push(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::bin_node**>(parent)); // кладем обратно в стек отца

        return;
    }

    else // 3 случай - отец красный
    {
        if (parent != nullptr &&
            grand_parent != nullptr &&
            _tree->get_color(*parent) == red_black_tree<tkey, tvalue, tkey_comparer>::color_node::RED)
        {
            // 3.2.3 Зиг-заг. Узел х справа от отца, а отец слева от деда
            if ((*parent)->right_subtree == subtree_root_address &&
                *parent == (*grand_parent)->left_subtree)
            {
                // делаем левый поворот относительно x-отца
                _tree->left_rotate(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::bin_node**>(parent),
                        reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::bin_node**>(grand_parent));
                subtree_root_address = (*parent)->left_subtree; // меняем указатель, приводя к случаю 3.2.1
            }

            // 3.2.4 Зиг-заг. Узел x слева от отца, а отец справа от деда
            else
            {
                if (subtree_root_address == (*parent)->left_subtree &&
                    *parent == (*grand_parent)->right_subtree)
                {
                    // делаем правый поворот относительно x-отца
                    _tree->right_rotate(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::bin_node**>(parent),
                            reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::bin_node**>(grand_parent));
                    subtree_root_address = (*parent)->right_subtree; // меняем указатель, приводя к случаю 3.2.2
                }
            }

            // если дядя черный
            if (_tree->get_color(*uncle) == red_black_tree<tkey, tvalue, tkey_comparer>::color_node::BLACK)
            {
                // перекрашиваем отца и деда
                (*parent)->color = red_black_tree<tkey, tvalue, tkey_comparer>::color_node::BLACK;
                (*grand_parent)->color = red_black_tree<tkey, tvalue, tkey_comparer>::color_node::RED;

                // случай 3.2.1. Все находятся слева (X и отец слева)
                if (subtree_root_address == (*parent)->left_subtree &&
                    *parent == (*grand_parent)->left_subtree)
                {
                    // делаем правый поворот
                    _tree->right_rotate(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::bin_node**>(grand_parent),
                            reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::bin_node**>(great_grand_parent));
                    // обновляем указатель
                    *parent = reinterpret_cast<red_black_node*>((*grand_parent)->right_subtree);
                }

                // случай 3.2.2. Все находятся справа (X и отец справа)
                else
                {
                    _tree->left_rotate(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::bin_node**>(grand_parent),
                            reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::bin_node**>(great_grand_parent));
                    // обновляем указатель
                    *parent = reinterpret_cast<red_black_node*>((*grand_parent)->left_subtree);
                }

                // добавляем указатель на деда и прадеда для продолжения рекурсии
                if (great_grand_parent != nullptr)
                {
                    path_to_subtree_root_exclusive.push(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::bin_node**>(great_grand_parent));
                }

                path_to_subtree_root_exclusive.push(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::bin_node**>(grand_parent));
            }

            // если дядя красный
            else
            {
                if (great_grand_parent != nullptr)
                {
                    path_to_subtree_root_exclusive.push(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::bin_node**>(great_grand_parent));
                }

                // перекрашиваем дядю в черный, отца в черный и деда в красный
                (*uncle)->color = red_black_tree<tkey, tvalue, tkey_comparer>::color_node::BLACK;
                (*parent)->color = red_black_tree<tkey, tvalue, tkey_comparer>::color_node::BLACK;
                (*grand_parent)->color = red_black_tree<tkey, tvalue, tkey_comparer>::color_node::RED;
                auto **bin_grand_parent = reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::bin_node**>(grand_parent);

                // запускаем проверку кч для прадедушки
                after_insert_concrete(key, *bin_grand_parent, path_to_subtree_root_exclusive);

                // добавляем указатели на деда и отца для продолжения рекурсии
                path_to_subtree_root_exclusive.push(bin_grand_parent);
                path_to_subtree_root_exclusive.push(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::bin_node**>(parent));
            }
        }
    }
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
red_black_tree<tkey, tvalue, tkey_comparer>::red_black_insertion_template_method::red_black_insertion_template_method(
        red_black_tree<tkey, tvalue, tkey_comparer> *tree) :
        _tree(tree),
        binary_search_tree<tkey, tvalue, tkey_comparer>::bin_insertion_template_method(tree)
{

}


template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
void red_black_tree<tkey, tvalue, tkey_comparer>::red_black_removing_template_method::get_info_deleted_node(
        typename binary_search_tree<tkey, tvalue, tkey_comparer>::bin_node *deleted_node,
        std::stack<typename binary_search_tree<tkey, tvalue, tkey_comparer>::bin_node**> &path_to_subtree_root_exclusive)
{
    _information_deleted_node = std::make_unique<red_black_node>();
    if (path_to_subtree_root_exclusive.empty())
    {
        _information_deleted_node->is_left_child = false;
        _information_deleted_node->is_right_child = false;
    }

    else
    {
        if ((*path_to_subtree_root_exclusive.top())->right_subtree == deleted_node)
        {
            _information_deleted_node->is_left_child = false;
            _information_deleted_node->is_right_child = true;
        }

        if ((*path_to_subtree_root_exclusive.top())->left_subtree == deleted_node)
        {
            _information_deleted_node->is_left_child = true;
            _information_deleted_node->is_right_child = false;
        }
    }

    _information_deleted_node->key = deleted_node->key;
    _information_deleted_node->value = deleted_node->value;
    _information_deleted_node->left_subtree = deleted_node->left_subtree;
    _information_deleted_node->right_subtree = deleted_node->right_subtree;
    _information_deleted_node->color = reinterpret_cast<red_black_node*>(deleted_node)->color;
}


template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
void red_black_tree<tkey, tvalue, tkey_comparer>::red_black_removing_template_method::after_remove_concrete(
        tkey const &key,
        typename binary_search_tree<tkey, tvalue, tkey_comparer>::bin_node *&subtree_root_address, // адрес отца удаленного узла!!!
        std::stack<typename binary_search_tree<tkey, tvalue, tkey_comparer>::bin_node **> &path_to_subtree_root_exclusive)
{
    if (subtree_root_address == nullptr)
    {
        return;
    }

    // если красный или черный узел с 2 детьми, то ничего не делаем, достаточно бст
    if ((_information_deleted_node->color == red_black_tree<tkey, tvalue, tkey_comparer>::color_node::RED ||
         _information_deleted_node->color == red_black_tree<tkey, tvalue, tkey_comparer>::color_node::BLACK) &&
        (_information_deleted_node->left_subtree != nullptr &&
         _information_deleted_node->right_subtree != nullptr))
    {
        return;
    }

    // 4. если Черный узел с 1 ребенком
    if (_information_deleted_node->color == red_black_tree<tkey, tvalue, tkey_comparer>::color_node::BLACK &&
        ((_information_deleted_node->left_subtree == nullptr && _information_deleted_node->right_subtree != nullptr) ||
         (_information_deleted_node->left_subtree != nullptr && _information_deleted_node->right_subtree == nullptr)))
    {
        if (_information_deleted_node->is_right_child == true) // если узел был правым сыном, то красим в черный
        {
            reinterpret_cast<red_black_node*>(subtree_root_address->right_subtree)->color = red_black_tree<tkey, tvalue, tkey_comparer>::color_node::BLACK;
        }

        if (_information_deleted_node->is_left_child == true) // если узел был левым сыном, то красим в черный
        {
            reinterpret_cast<red_black_node*>(subtree_root_address->left_subtree)->color = red_black_tree<tkey, tvalue, tkey_comparer>::color_node::BLACK;
        }

        if (_information_deleted_node->is_left_child == false &&
            _information_deleted_node->is_right_child == false) // если удаляемый узел - корень, то красим в черный
        {
            reinterpret_cast<red_black_node*>(subtree_root_address)->color = red_black_tree<tkey, tvalue, tkey_comparer>::color_node::BLACK;
        }

        return;
    }

    // если Черный узел без детей
    if (_information_deleted_node->color == red_black_tree<tkey, tvalue, tkey_comparer>::color_node::BLACK &&
        _information_deleted_node->left_subtree == nullptr && _information_deleted_node->right_subtree == nullptr)
    {
        typename binary_search_tree<tkey, tvalue, tkey_comparer>::bin_node **parent = nullptr;

        if (!path_to_subtree_root_exclusive.empty())
        {
            parent = path_to_subtree_root_exclusive.top();
            path_to_subtree_root_exclusive.pop();
        }

        // если узел был левым сыном
        if (_information_deleted_node->is_left_child == true)
        {
            // и у него был правый брат
            if (subtree_root_address->right_subtree != nullptr)
            {
                // получаем цвета: отца, правого брата и сыновей брата
                color_node color_subtree = _tree->get_color(reinterpret_cast<red_black_node*>(subtree_root_address));
                color_node color_sibling = _tree->get_color(reinterpret_cast<red_black_node*>(subtree_root_address->right_subtree));
                color_node color_children_right = _tree->get_color(reinterpret_cast<red_black_node*>(subtree_root_address->right_subtree->right_subtree));
                color_node color_children_left = _tree->get_color(reinterpret_cast<red_black_node*>(subtree_root_address->right_subtree->left_subtree));

                // если брат черный
                if (color_sibling == red_black_tree<tkey, tvalue, tkey_comparer>::color_node::BLACK)
                {
                    // если один из детей брата или оба красные
                    if (color_children_right == red_black_tree<tkey, tvalue, tkey_comparer>::color_node::RED ||
                        color_children_left == red_black_tree<tkey, tvalue, tkey_comparer>::color_node::RED)
                    {
                        // если правый сын красный (случай 2.1)
                        if (color_children_right == red_black_tree<tkey, tvalue, tkey_comparer>::color_node::RED)
                        {
                            // перекрашиваем брата в цвет отца, отца в черный, а правого сына брата в черный
                            reinterpret_cast<red_black_node*>(subtree_root_address->right_subtree)->color = color_subtree;
                            reinterpret_cast<red_black_node*>(subtree_root_address)->color = red_black_tree<tkey, tvalue, tkey_comparer>::color_node::BLACK;
                            reinterpret_cast<red_black_node*>(subtree_root_address->right_subtree->right_subtree)->color = red_black_tree<tkey, tvalue, tkey_comparer>::color_node::BLACK;
                            // делаем левый поворот
                            _tree->left_rotate(&subtree_root_address, parent);
                        }

                        // если левый сын красный, а правый черный
                        if (color_children_left == red_black_tree<tkey, tvalue, tkey_comparer>::color_node::RED &&
                            color_children_right == red_black_tree<tkey, tvalue, tkey_comparer>::color_node::BLACK)
                        {
                            // брата красим в красный, левого сына в цвет брата и делаем правый поворот
                            reinterpret_cast<red_black_node*>(subtree_root_address->right_subtree)->color = red_black_tree<tkey, tvalue, tkey_comparer>::color_node::RED;
                            reinterpret_cast<red_black_node*>(subtree_root_address->right_subtree->left_subtree)->color = color_sibling;
                            _tree->right_rotate(&(subtree_root_address->right_subtree), &subtree_root_address);

                            // после этого красим нового брата в цвет отца, отца в черный, сына нового брата в черный
                            reinterpret_cast<red_black_node*>(subtree_root_address->right_subtree)->color = color_subtree;
                            reinterpret_cast<red_black_node*>(subtree_root_address)->color = red_black_tree<tkey, tvalue, tkey_comparer>::color_node::BLACK;
                            reinterpret_cast<red_black_node*>(subtree_root_address->right_subtree->right_subtree)->color = red_black_tree<tkey, tvalue, tkey_comparer>::color_node::BLACK;
                            // делаем левый поворот
                            _tree->left_rotate(&subtree_root_address, parent);
                        }
                    }

                    // если оба ребенка брата черные (гг)
                    if (color_children_left == red_black_tree<tkey, tvalue, tkey_comparer>::color_node::BLACK &&
                        color_children_right == red_black_tree<tkey, tvalue, tkey_comparer>::color_node::BLACK)
                    {
                        // красим брата в красный, а отца в черный
                        reinterpret_cast<red_black_node*>(subtree_root_address->right_subtree)->color = red_black_tree<tkey, tvalue, tkey_comparer>::color_node::RED;
                        reinterpret_cast<red_black_node*>(subtree_root_address)->color = red_black_tree<tkey, tvalue, tkey_comparer>::color_node::BLACK;

                        // отец был черный
                        if (color_subtree == red_black_tree<tkey, tvalue, tkey_comparer>::color_node::BLACK)
                        {
                            // копируем информацию отца вместо удаленного сына для продолжения балансировки
                            _information_deleted_node->color = reinterpret_cast<red_black_node*>(subtree_root_address)->color;
                            _information_deleted_node->key = subtree_root_address->key;
                            _information_deleted_node->value = subtree_root_address->value;
                            _information_deleted_node->left_subtree = subtree_root_address->left_subtree;
                            _information_deleted_node->right_subtree = subtree_root_address->right_subtree;

                            // если был дедушка, то узнаем с какой стороны от него отец
                            if (parent != nullptr)
                            {
                                if ((*parent)->left_subtree == subtree_root_address)
                                {
                                    _information_deleted_node->is_left_child = true;
                                    _information_deleted_node->is_right_child = false;
                                }

                                if ((*parent)->right_subtree == subtree_root_address)
                                {
                                    _information_deleted_node->is_left_child = false;
                                    _information_deleted_node->is_right_child = true;
                                }

                                // и запускаем рекурсивно балансировку наверх, для отца и тд наверх
                                after_remove_concrete(key, *parent, path_to_subtree_root_exclusive);
                            }

                            // если дедушки не было
                            else
                            {
                                path_to_subtree_root_exclusive.push(parent); // добавляем null в стек
                                // вызываем рекурсивно балансировку для отца
                                after_remove_concrete(key, subtree_root_address, path_to_subtree_root_exclusive);
                                path_to_subtree_root_exclusive.pop(); // забираем null из стека
                            }
                        }
                    }
                }

                // если брат красный (случай 6.1 Ч0)
                else
                {
                    // красим брата в черный, а отца в красный и делаем левый поворот
                    reinterpret_cast<red_black_node*>(subtree_root_address->right_subtree)->color = red_black_tree<tkey, tvalue, tkey_comparer>::color_node::BLACK;
                    reinterpret_cast<red_black_node*>(subtree_root_address)->color = red_black_tree<tkey, tvalue, tkey_comparer>::color_node::RED;
                    _tree->left_rotate(&subtree_root_address, parent);

                    // добавляем деда и отца в стек и делаем рекурсивный вызов балансировки
                    path_to_subtree_root_exclusive.push(parent);
                    path_to_subtree_root_exclusive.push(&subtree_root_address);
                    after_remove_concrete(key, subtree_root_address->left_subtree, path_to_subtree_root_exclusive);
                    path_to_subtree_root_exclusive.pop();
                    path_to_subtree_root_exclusive.pop();
                }
            }
        }

        // если узел был правым сыном
        else
        {
            // и у него был левый брат
            if (subtree_root_address->left_subtree != nullptr)
            {
                // получаем цвета: отца, левого брата и сыновей брата
                color_node color_subtree = _tree->get_color(reinterpret_cast<red_black_node*>(subtree_root_address));
                color_node color_sibling = _tree->get_color(reinterpret_cast<red_black_node*>(subtree_root_address->left_subtree));
                color_node color_children_right = _tree->get_color(reinterpret_cast<red_black_node*>(subtree_root_address->left_subtree->right_subtree));
                color_node color_children_left = _tree->get_color(reinterpret_cast<red_black_node*>(subtree_root_address->left_subtree->left_subtree));

                // если брат черный (аналогично левой строне, только зеркально)
                if (color_sibling == red_black_tree<tkey, tvalue, tkey_comparer>::color_node::BLACK)
                {
                    // если один из детей брата или оба красные
                    if (color_children_right == red_black_tree<tkey, tvalue, tkey_comparer>::color_node::RED ||
                        color_children_left == red_black_tree<tkey, tvalue, tkey_comparer>::color_node::RED)
                    {
                        // если левый сын красный
                        if (color_children_left == red_black_tree<tkey, tvalue, tkey_comparer>::color_node::RED)
                        {
                            // перекрашиваем брата в цвет отца, отца в черный, а левого сына брата в черный
                            reinterpret_cast<red_black_node*>(subtree_root_address->left_subtree)->color = color_subtree;
                            reinterpret_cast<red_black_node*>(subtree_root_address)->color = red_black_tree<tkey, tvalue, tkey_comparer>::color_node::BLACK;
                            reinterpret_cast<red_black_node*>(subtree_root_address->left_subtree->left_subtree)->color = red_black_tree<tkey, tvalue, tkey_comparer>::color_node::BLACK;
                            // делаем правый поворот
                            _tree->right_rotate(&subtree_root_address, parent);
                        }

                        // если правый сын красный, а левый черный
                        if (color_children_right == red_black_tree<tkey, tvalue, tkey_comparer>::color_node::RED &&
                            color_children_left == red_black_tree<tkey, tvalue, tkey_comparer>::color_node::BLACK)
                        {
                            // брата красим в красный, правого сына в цвет брата и делаем левый поворот
                            reinterpret_cast<red_black_node*>(subtree_root_address->left_subtree)->color = red_black_tree<tkey, tvalue, tkey_comparer>::color_node::RED;
                            reinterpret_cast<red_black_node*>(subtree_root_address->left_subtree->right_subtree)->color = color_sibling;
                            _tree->left_rotate(&(subtree_root_address->left_subtree), &subtree_root_address);

                            // после этого красим нового брата в цвет отца, отца в черный, сына нового брата в черный
                            reinterpret_cast<red_black_node*>(subtree_root_address->left_subtree)->color = color_subtree;
                            reinterpret_cast<red_black_node*>(subtree_root_address)->color = red_black_tree<tkey, tvalue, tkey_comparer>::color_node::BLACK;
                            reinterpret_cast<red_black_node*>(subtree_root_address->left_subtree->left_subtree)->color = red_black_tree<tkey, tvalue, tkey_comparer>::color_node::BLACK;
                            // делаем правый поворот
                            _tree->right_rotate(&subtree_root_address, parent);
                        }
                    }

                    // если оба ребенка брата черные (гг)
                    if (color_children_left == red_black_tree<tkey, tvalue, tkey_comparer>::color_node::BLACK &&
                        color_children_right == red_black_tree<tkey, tvalue, tkey_comparer>::color_node::BLACK)
                    {
                        // красим брата в красный, а отца в черный
                        reinterpret_cast<red_black_node*>(subtree_root_address->left_subtree)->color = red_black_tree<tkey, tvalue, tkey_comparer>::color_node::RED;
                        reinterpret_cast<red_black_node*>(subtree_root_address)->color = red_black_tree<tkey, tvalue, tkey_comparer>::color_node::BLACK;

                        // отец был черный
                        if (color_subtree == red_black_tree<tkey, tvalue, tkey_comparer>::color_node::BLACK)
                        {
                            // копируем информацию отца вместо удаленного сына для продолжения балансировки
                            _information_deleted_node->color = reinterpret_cast<red_black_node*>(subtree_root_address)->color;
                            _information_deleted_node->key = subtree_root_address->key;
                            _information_deleted_node->value = subtree_root_address->value;
                            _information_deleted_node->left_subtree = subtree_root_address->left_subtree;
                            _information_deleted_node->right_subtree = subtree_root_address->right_subtree;

                            // если был дедушка, то узнаем с какой стороны от него отец
                            if (parent != nullptr)
                            {
                                if ((*parent)->left_subtree == subtree_root_address)
                                {
                                    _information_deleted_node->is_left_child = true;
                                    _information_deleted_node->is_right_child = false;
                                }

                                if ((*parent)->right_subtree == subtree_root_address)
                                {
                                    _information_deleted_node->is_left_child = false;
                                    _information_deleted_node->is_right_child = true;
                                }

                                // и запускаем рекурсивно балансировку наверх, для отца и тд наверх
                                after_remove_concrete(key, *parent, path_to_subtree_root_exclusive);
                            }

                                // если дедушки не было
                            else
                            {
                                path_to_subtree_root_exclusive.push(parent); // добавляем null в стек
                                // вызываем рекурсивно балансировку для отца
                                after_remove_concrete(key, subtree_root_address, path_to_subtree_root_exclusive);
                                path_to_subtree_root_exclusive.pop(); // забираем null из стека
                            }
                        }
                    }
                }

                // если брат красный (случай 6.1 Ч0) Аналогично левой стороне!
                else
                {
                    // красим брата в черный, а отца в красный и делаем правый поворот
                    reinterpret_cast<red_black_node*>(subtree_root_address->left_subtree)->color = red_black_tree<tkey, tvalue, tkey_comparer>::color_node::BLACK;
                    reinterpret_cast<red_black_node*>(subtree_root_address)->color = red_black_tree<tkey, tvalue, tkey_comparer>::color_node::RED;
                    _tree->right_rotate(&subtree_root_address, parent);

                    // добавляем деда и отца в стек и делаем рекурсивный вызов балансировки
                    path_to_subtree_root_exclusive.push(parent);
                    path_to_subtree_root_exclusive.push(&subtree_root_address);
                    after_remove_concrete(key, subtree_root_address->right_subtree, path_to_subtree_root_exclusive);
                    path_to_subtree_root_exclusive.pop();
                    path_to_subtree_root_exclusive.pop();
                }
            }
        }

        if (parent != nullptr)
        {
            path_to_subtree_root_exclusive.push(parent);
        }
    }
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
red_black_tree<tkey, tvalue, tkey_comparer>::red_black_removing_template_method::red_black_removing_template_method(
        red_black_tree<tkey, tvalue, tkey_comparer> *tree) :
        _tree(tree),
        binary_search_tree<tkey, tvalue, tkey_comparer>::bin_removing_template_method(tree)
{

}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
red_black_tree<tkey, tvalue, tkey_comparer>::red_black_removing_template_method::~red_black_removing_template_method()
{

}


template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
red_black_tree<tkey, tvalue, tkey_comparer>::red_black_tree(
        memory *allocator_binary_search_tree,
        logger *logger_tree) :
        binary_search_tree<tkey, tvalue, tkey_comparer>(
                new typename binary_search_tree<tkey, tvalue, tkey_comparer>::bin_find_template_method(this),
                new red_black_insertion_template_method(this),
                new red_black_removing_template_method(this),
                allocator_binary_search_tree,
                logger_tree)
{

}


template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
red_black_tree<tkey, tvalue, tkey_comparer>::red_black_tree(
        const red_black_tree<tkey, tvalue, tkey_comparer> &target_copy)
{

}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
red_black_tree<tkey, tvalue, tkey_comparer> & red_black_tree<tkey, tvalue, tkey_comparer>::operator=(
        const red_black_tree<tkey, tvalue, tkey_comparer> &target_copy)
{

}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
red_black_tree<tkey, tvalue, tkey_comparer>::red_black_tree(
        red_black_tree<tkey, tvalue, tkey_comparer> &&target_copy) noexcept
{

}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
red_black_tree<tkey, tvalue, tkey_comparer>& red_black_tree<tkey, tvalue, tkey_comparer>::operator=(
        red_black_tree<tkey, tvalue, tkey_comparer> &&target_copy) noexcept
{

}


#endif // RED_BLACK_TREE_H