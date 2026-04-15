/// \file rbtree.hpp
/// \brief Самописное красно-чёрное дерево с дубликатами ключей
///
///
/// Свойства красно-чёрного дерева (инвариант):
///  1) Каждый узел либо красный, либо чёрный
///  2) Корень — чёрный
///  3) Все NIL-листья — чёрные
///  4) У красного узла оба потомка чёрные (нет двух красных подряд)
///  5) На любом пути от узла до его листьев одинаковое число чёрных узлов
#pragma once

#include "../teacher.hpp"

#include <string>
#include <vector>

namespace pt::search {

class RBTree {
public:
    RBTree() {
        nil_ = new Node();
        nil_->color = BLACK;
        nil_->left = nil_->right = nil_->parent = nil_;
        root_ = nil_;
    }

    RBTree(const RBTree&) = delete;
    RBTree& operator=(const RBTree&) = delete;

    void insert(const std::string& key, const Teacher* obj) {
        Node* z = new Node(key, obj, nil_);
        z->left = z->right = nil_;
        Node* y = nil_;
        Node* x = root_;
        while (x != nil_) {
            y = x;
            if (z->key < x->key) x = x->left;
            else                 x = x->right;     // дубли -> вправо
        }
        z->parent = y;
        if (y == nil_)            root_ = z;
        else if (z->key < y->key) y->left = z;
        else                      y->right = z;
        z->color = RED;
        insert_fixup(z);
    }

    std::vector<const Teacher*> find_all(const std::string& key) const {
        std::vector<const Teacher*> out;
        Node* x = root_;
        while (x != nil_) {
            if (key < x->key) x = x->left;
            else if (key > x->key) x = x->right;
            else {
                collect_equal(x, key, out);
                return out;
            }
        }
        return out;
    }

private:
    enum Color { RED, BLACK };
    struct Node {
        std::string key;
        const Teacher* obj = nullptr;
        Color color = BLACK;
        Node* left = nullptr;
        Node* right = nullptr;
        Node* parent = nullptr;
        Node() = default;
        Node(const std::string& k, const Teacher* o, Node* nil)
            : key(k), obj(o), color(RED), left(nil), right(nil), parent(nil) {}
    };
    Node* root_;
    Node* nil_;

    void left_rotate(Node* x) {
        Node* y = x->right;
        x->right = y->left;
        if (y->left != nil_) y->left->parent = x;
        y->parent = x->parent;
        if (x->parent == nil_) root_ = y;
        else if (x == x->parent->left) x->parent->left = y;
        else                           x->parent->right = y;
        y->left = x;
        x->parent = y;
    }

    void right_rotate(Node* x) {
        Node* y = x->left;
        x->left = y->right;
        if (y->right != nil_) y->right->parent = x;
        y->parent = x->parent;
        if (x->parent == nil_) root_ = y;
        else if (x == x->parent->right) x->parent->right = y;
        else                            x->parent->left = y;
        y->right = x;
        x->parent = y;
    }

    /// Восстановление RB-свойств после вставки красного узла z
    void insert_fixup(Node* z) {
        while (z->parent->color == RED) {
            if (z->parent == z->parent->parent->left) {
                Node* y = z->parent->parent->right;
                if (y->color == RED) {
                    // Случай 1: дядя красный — перекрашиваем
                    z->parent->color = BLACK;
                    y->color = BLACK;
                    z->parent->parent->color = RED;
                    z = z->parent->parent;
                } else {
                    if (z == z->parent->right) {
                        // Случай 2: треугольник — поворот к линии
                        z = z->parent;
                        left_rotate(z);
                    }
                    // Случай 3: линия — перекраска и поворот
                    z->parent->color = BLACK;
                    z->parent->parent->color = RED;
                    right_rotate(z->parent->parent);
                }
            } else {
                Node* y = z->parent->parent->left;
                if (y->color == RED) {
                    z->parent->color = BLACK;
                    y->color = BLACK;
                    z->parent->parent->color = RED;
                    z = z->parent->parent;
                } else {
                    if (z == z->parent->left) {
                        z = z->parent;
                        right_rotate(z);
                    }
                    z->parent->color = BLACK;
                    z->parent->parent->color = RED;
                    left_rotate(z->parent->parent);
                }
            }
        }
        root_->color = BLACK;
    }

    /// Сбор всех узлов с заданным ключом — он мог расщепиться по обоим
    /// поддеревьям после поворотов
    void collect_equal(Node* x, const std::string& key,
                       std::vector<const Teacher*>& out) const {
        if (x == nil_) return;
        if (x->key == key) {
            out.push_back(x->obj);
            collect_equal(x->left,  key, out);
            collect_equal(x->right, key, out);
        } else if (key < x->key) {
            collect_equal(x->left, key, out);
        } else {
            collect_equal(x->right, key, out);
        }
    }
};

} // namespace pt::search
