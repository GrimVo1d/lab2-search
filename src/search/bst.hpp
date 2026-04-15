/// \file bst.hpp
/// \brief Самописное двоичное дерево поиска (BST) с поддержкой неуникальных ключей
///
/// Узлы хранят пары (ключ, указатель на объект); дубликаты помещаются
/// в правое поддерево (a >= x → вправо)
#pragma once

#include "../teacher.hpp"

#include <string>
#include <vector>

namespace pt::search {

class BST {
public:
    BST() = default;

    BST(const BST&) = delete;
    BST& operator=(const BST&) = delete;

    /// \brief Вставить (ключ, указатель); ключи могут повторяться
    void insert(const std::string& key, const Teacher* obj) {
        root_ = insert_impl(root_, key, obj);
    }

    /// \brief Найти все объекты с ключом key
    std::vector<const Teacher*> find_all(const std::string& key) const {
        std::vector<const Teacher*> out;
        find_impl(root_, key, out);
        return out;
    }

private:
    struct Node {
        std::string key;
        const Teacher* obj;
        Node* left = nullptr;
        Node* right = nullptr;
        Node(const std::string& k, const Teacher* o) : key(k), obj(o) {}
    };
    Node* root_ = nullptr;

    static Node* insert_impl(Node* n, const std::string& key, const Teacher* obj) {
        if (!n) return new Node(key, obj);
        if (key < n->key) n->left  = insert_impl(n->left,  key, obj);
        else              n->right = insert_impl(n->right, key, obj); // дубли — вправо
        return n;
    }

    /// Из-за дубликатов после нахождения первого совпадения нужно
    /// также проверить дубли в обоих поддеревьях (могут быть равные ключи)
    static void find_impl(const Node* n, const std::string& key,
                          std::vector<const Teacher*>& out) {
        while (n) {
            if (key < n->key)      n = n->left;
            else if (key > n->key) n = n->right;
            else {
                out.push_back(n->obj);
                // дубликаты по соглашению идут вправо
                find_impl(n->right, key, out);
                // но из-за вставок до текущего узла дубликат мог
                // оказаться и слева на равной глубине? Нет: при равенстве
                // мы всегда уходим вправо, значит слева ключей == n->key нет
                return;
            }
        }
    }
};

} // namespace pt::search
