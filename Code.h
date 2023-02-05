#pragma once

#include <algorithm>
#include <memory>
#include <vector>

template<class T>

class Set {

private:
    struct Node {
        std::vector<std::shared_ptr<Node>> children;
        std::shared_ptr<Node> par;
        T max_l, max_mid, max;

        Node() = default;

        Node(T key) {
            max_l = key;
            max = key;
        }

        ~Node() = default;
    };

    bool is_equal(T element1, T element2) const {
        return !(element1 < element2) && !(element2 < element1);
    }

    std::shared_ptr<Node> find_vertex(const std::shared_ptr<Node> &now, const T key) const {
        if (now == nullptr) {
            return nullptr;
        }
        if (now->children.empty()) {
            return now;
        }
        if (now->children.size() == 2) {
            if (now->max_l < key) {
                return find_vertex(now->children[1], key);
            } else {
                return find_vertex(now->children[0], key);
            }
        } else {
            if (now->max_mid < key) {
                return find_vertex(now->children[2], key);
            } else if (now->max_l < key) {
                return find_vertex(now->children[1], key);
            } else {
                return find_vertex(now->children[0], key);
            }
        }
    }

    void make_new_root(const std::shared_ptr<Node> &node1, const std::shared_ptr<Node> &node2) {
        std::shared_ptr<Node> new_root = std::make_shared<Node>();
        if (node2->max_l < node1->max_l) {
            new_root->max_l = node2->max_l;
            new_root->children.push_back(node2);
            new_root->children.push_back(node1);
        } else {
            new_root->max_l = node1->max_l;
            new_root->children.push_back(node1);
            new_root->children.push_back(node2);
        }
        new_root->max = std::max(new_root->children[0]->max, new_root->children[1]->max);
        node1->par = new_root;
        node2->par = new_root;
        root_ = new_root;
    }

    static bool comparator_children(const std::shared_ptr<Node> &child1, const std::shared_ptr<Node> &child2) {
        return (child1->max_l < child2->max_l);
    }

    void update_node(const std::shared_ptr<Node> &node) {
        if (node == nullptr) {
            return;
        }
        if (node->children.empty()) {
            return;
        }
        sort(node->children.begin(), node->children.end(), comparator_children);
        node->max_l = node->children[0]->max;
        node->max = node->max_l;
        for (auto &element: node->children) {
            element->par = node;
            if (node->max < element->max) {
                node->max = element->max;
            }
        }
        if (node->children.size() > 2) {
            node->max_mid = node->children[1]->max;
        }
    }

    void global_update(const std::shared_ptr<Node> &node) {
        if (node == nullptr) {
            return;
        }
        update_node(node);
        global_update(node->par);
    }

    void join_child_to_parent(const std::shared_ptr<Node> &par, const std::shared_ptr<Node> &child) {
        par->children.push_back(child);
        child->par = par;
        update_node(par);
    }

    std::pair<std::shared_ptr<Node>, std::shared_ptr<Node>> split(const std::shared_ptr<Node> &node) {
        update_node(node);
        std::shared_ptr<Node> left = std::make_shared<Node>();
        std::shared_ptr<Node> right = std::make_shared<Node>();
        left->children.push_back(node->children[0]);
        left->children.push_back(node->children[1]);
        right->children.push_back(node->children[2]);
        right->children.push_back(node->children[3]);
        left->children[0]->par = left;
        left->children[1]->par = left;
        right->children[0]->par = right;
        right->children[1]->par = right;
        update_node(left);
        update_node(right);
        return make_pair(left, right);
    }

    void go_up(const std::shared_ptr<Node> &node) {
        if (node == nullptr) {
            return;
        }
        if (node->children.size() < 4) {
            return;
        }
        std::shared_ptr<Node> parent = node->par;
        std::pair<std::shared_ptr<Node>, std::shared_ptr<Node>> pnn = split(node);
        if (parent == nullptr) {
            std::shared_ptr<Node> new_root = std::make_shared<Node>();
            new_root->children.push_back(pnn.first);
            new_root->children.push_back(pnn.second);
            pnn.first->par = new_root;
            pnn.second->par = new_root;
            root_ = new_root;
            update_node(root_);
        } else {
            parent->children.erase(std::find(parent->children.begin(), parent->children.end(), node));
            parent->children.push_back(pnn.first);
            parent->children.push_back(pnn.second);
            pnn.first->par = parent;
            pnn.second->par = parent;
            update_node(parent);
            go_up(parent);
        }
    }

    void insert_to_tree(const T key) {
        std::shared_ptr<Node> found = find_vertex(root_, key);
        std::shared_ptr<Node> new_ver = std::make_shared<Node>(key);
        if (found == nullptr) {
            size_++;
            root_ = new_ver;
            return;
        }
        if (is_equal(found->max_l, key)) {
            return;
        }
        size_++;
        if (found->par == nullptr) {
            make_new_root(root_, new_ver);
            return;
        }
        join_child_to_parent(found->par, new_ver);
        go_up(found->par);
        global_update(found->par);
    }

    std::shared_ptr<Node> get_uncle(const std::shared_ptr<Node> &current) {
        if (current->par == nullptr) {
            return nullptr;
        }
        std::shared_ptr<Node> par = current->par;

        if (current != par->children[1]) {
            return par->children[1];
        } else {
            return par->children[0];
        }
    }

    void erase_up(const std::shared_ptr<Node> &current) {
        if (current == nullptr) {
            return;
        }
        if (current->children.size() > 1) {
            return;
        }
        std::shared_ptr<Node> par = current->par;
        std::shared_ptr<Node> brother;
        if (par == nullptr) {
            root_ = current->children[0];
            current->children[0]->par = nullptr;
            current->children.clear();
            return;
        }

        brother = par->children[1];
        if (current == brother) {
            brother = par->children[0];
        }

        brother->children.push_back(current->children[0]);
        current->children[0]->par = brother;

        current->children.clear();

        par->children.erase(std::find(par->children.begin(), par->children.end(), current));
        current->par = nullptr;

        if (brother->children.size() > 3) {
            std::pair<std::shared_ptr<Node>, std::shared_ptr<Node>> pnn = split(brother);
            par->children.push_back(pnn.first);
            par->children.push_back(pnn.second);
            pnn.first->par = par;
            pnn.second->par = par;
            par->children.erase(std::find(par->children.begin(), par->children.end(), brother));
            brother->children.clear();
            brother->par = nullptr;
            update_node(par);
        }

        erase_up(par);

    }

    void erase_in_tree(const T key) {
        std::shared_ptr<Node> current = find_vertex(root_, key);
        if (current == nullptr || !is_equal(current->max_l, key)) {
            return;
        }
        size_--;
        std::shared_ptr<Node> par = current->par;
        std::shared_ptr<Node> brother;
        if (par == nullptr) {
            root_ = nullptr;
            current->par = nullptr;
            return;
        }

        brother = par->children[1];
        if (current == brother) {
            brother = par->children[0];
        }
        par->children.erase(std::find(par->children.begin(), par->children.end(), current));
        current->par = nullptr;

        if (par->children.size() > 1) {
            global_update(par);
            return;
        }

        std::shared_ptr<Node> uncle = get_uncle(par);

        if (uncle == nullptr) {
            root_ = brother;
            root_->par = nullptr;
            root_->children.clear();
            return;
        }
        uncle->children.push_back(brother);
        par->children.erase(std::find(par->children.begin(), par->children.end(), brother));
        brother->par = uncle;
        if (uncle->children.size() > 3) {
            std::pair<std::shared_ptr<Node>, std::shared_ptr<Node>> pnn = split(uncle);

            uncle->par->children.push_back(pnn.first);
            uncle->par->children.push_back(pnn.second);

            pnn.first->par = uncle->par;
            pnn.second->par = uncle->par;

            uncle->par->children.erase(std::find(uncle->par->children.begin(), uncle->par->children.end(), uncle));

            update_node(uncle->par);

            uncle->par = nullptr;

            uncle->children.clear();

        }

        par->par->children.erase(std::find(par->par->children.begin(), par->par->children.end(), par));

        std::shared_ptr<Node> next = par->par;

        par->par = nullptr;
        par->children.clear();

        erase_up(next);
        global_update(brother);
    }

    void destroy_vertex(const std::shared_ptr<Node> &current) {
        if (current == nullptr) {
            return;
        }
        if (current->children.empty()) {
            current->par = nullptr;
        } else {
            for (auto &element: current->children) {
                destroy_vertex(element);
            }
            current->par = nullptr;
            current->children.clear();
        }
    }

    std::shared_ptr<Node> root_ = nullptr;

    size_t size_ = 0;

public:
    class iterator {
    public:
        bool is_end = false;

        iterator() : is_end(true) {}

        iterator(std::shared_ptr<Node> node) {
            current_ = node;
            is_end = (current_ == nullptr);
        }

        ~iterator() = default;

        const T &operator*() {
            return current_->max_l;
        }

        const T *operator->() {
            return &(current_->max_l);
        }

        iterator &operator++() {
            move_right();
            return *this;
        }

        iterator &operator--() {
            move_left();
            return *this;
        }

        iterator operator++(int) {
            iterator it(current_);
            move_right();
            return it;
        }

        iterator operator--(int) {
            iterator it(current_);
            move_left();
            return it;
        }

        bool operator==(iterator it) const {
            return (this->is_end == it.is_end && this->current_ == it.current_);
        }

        bool operator!=(iterator it) const {
            return !(it == *this);
        }

    private:
        std::shared_ptr<Node> current_;

        void move_right() {
            std::shared_ptr<Node> now = current_;
            while (now->par != nullptr && now == now->par->children.back()) {
                now = now->par;
            }
            if (now->par == nullptr) {
                is_end = true;
            } else {
                if (now == now->par->children[0]) {
                    current_ = go_left(now->par->children[1]);
                } else {
                    current_ = go_left(now->par->children[2]);
                }
            }
        }

        void move_left() {
            if (is_end) {
                is_end = false;
                return;
            }
            std::shared_ptr<Node> now = current_;
            while (now->par != nullptr && now == now->par->children[0]) {
                now = now->par;
            }
            if (now->par == nullptr) {
                return;
            }

            if (now == now->par->children[1]) {
                current_ = go_right(now->par->children[0]);
            } else {
                current_ = go_right(now->par->children[1]);
            }
        }
    };

    Set() = default;

    template<typename Iterator>

    Set(Iterator first, Iterator last) {
        for (auto it = first; it != last; ++it) {
            insert_to_tree(*first);
        }
    }

    Set(std::initializer_list<T> initializer_list) {
        for (const auto &element: initializer_list) {
            insert_to_tree(element);
        }
    }

    Set(const Set<T> &st) {
        destroy_vertex(root_);
        size_ = 0;
        root_ = nullptr;
        for (const auto &element: st) {
            insert(element);
        }
    }

    Set &operator=(const Set<T> &st) {
        if (this == &st) {
            return *this;
        }
        destroy_vertex(root_);
        root_ = nullptr;
        size_ = 0;
        for (const auto &element: st) {
            insert(element);
        }
        return *this;
    }

    ~Set() = default;

    size_t size() const {
        return size_;
    }

    bool empty() const {
        return size_ == 0;
    }

    void insert(const T &element) {
        insert_to_tree(element);
    }

    void erase(const T &element) {
        erase_in_tree(element);
    }

    iterator begin() const {
        std::shared_ptr<Node> now = root_;
        if (root_ == nullptr) {
            return end();
        } else {
            now = go_left(now);
            iterator it(now);
            return it;
        }
    }

    iterator end() const {
        std::shared_ptr<Node> now = root_;
        if (root_ == nullptr) {
            iterator it(nullptr);
            it.is_end = true;
            return it;
        } else {
            now = go_right(now);
            iterator it(now);
            it.is_end = true;
            return it;
        }
    }

    iterator find(const T &element) const {
        std::shared_ptr<Node> found = find_vertex(root_, element);
        if (found == nullptr) {
            return end();
        }
        if (!is_equal(found->max_l, element)) {
            return end();
        }
        return iterator(found);
    }

    iterator lower_bound(const T &element) const {
        std::shared_ptr<Node> found = find_vertex(root_, element);
        if (found == nullptr) {
            iterator it = end();
            return it;
        }
        if (found->max_l < element) {
            iterator it = end();
            return it;
        }
        return iterator(found);
    }

    static std::shared_ptr<Node> go_right(const std::shared_ptr<Node> &now) {
        if (now->children.empty()) {
            return now;
        } else if (now->children.size() == 2) {
            return go_right(now->children[1]);
        } else if (now->children.size() == 3) {
            return go_right(now->children[2]);
        }
    }

    static std::shared_ptr<Node> go_left(const std::shared_ptr<Node> &now) {
        if (now->children.empty()) {
            return now;
        } else {
            return go_left(now->children[0]);
        }
    }
};
