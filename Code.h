#include <cstddef>
#include <vector>
#include <memory>
#include <algorithm>
#include <iostream>

#include <set>

template<class T>
class Set {
public:
    Set() {
        root = nullptr;
        size_ = 0;
    }

    struct Node {
        std::vector<std::shared_ptr<Node>> ch_;
        std::shared_ptr<Node> par_;
        T max_l_, max_mid_, max_;
        Node() {
            par_ = nullptr;
            ch_.clear();
        }
        Node(T key) {
            par_ = nullptr;
            max_l_ = key;
            max_ = key;
            ch_.clear();
        }
        ~Node() {
            ch_.clear();
            par_ = nullptr;
        }
    };

    class iterator {
    public:
        iterator() {
            cur = nullptr;
            is_end = true;
        }
        ~iterator() {
            cur = nullptr;
        }
        iterator(std::shared_ptr<Node> node) {
            cur = node;
            is_end = false;
            if (cur == nullptr) {
                is_end = true;
            }
        }
        const T& operator*() {
            return cur->max_l_;
        }
        const T* operator->() {
            return &(cur->max_l_);
        }

        iterator& operator++() {
            get_right();
            return *this;
        }
        iterator& operator--() {
            get_left();
            return *this;
        }

        iterator operator++(int) {
            iterator it(cur);
            get_right();
            return it;
        }

        iterator operator--(int) {
            iterator it(cur);
            get_left();
            return it;
        }

        bool operator==(iterator it) const {
            return (this->is_end == it.is_end && this->cur == it.cur);
        }
        bool operator!=(iterator it) const {
            return (this->is_end != it.is_end) || (this->cur != it.cur);
        }
        std::shared_ptr<Node> cur;
        bool is_end = false;
        void get_right() {
            std::shared_ptr<Node> now = cur;
            while (now->par_ != nullptr && now == now->par_->ch_.back()) {
                now = now->par_;
            }
            if (now->par_ == nullptr) {
                is_end = true;
            } else {
                if (now == now->par_->ch_[0]) {
                    cur = go_left(now->par_->ch_[1]);
                } else {
                    cur = go_left(now->par_->ch_[2]);
                }
            }
        }

        void get_left() {
            if (is_end) {
                is_end = false;
                return;
            }
            std::shared_ptr<Node> now = cur;
            while (now->par_ != nullptr && now == now->par_->ch_[0]) {
                now = now->par_;
            }
            if (now->par_ == nullptr) {
                return;
            }

            if (now == now->par_->ch_[1]) {
                cur = go_right(now->par_->ch_[0]);
            } else {
                cur = go_right(now->par_->ch_[1]);
            }
        }
    };

    template<typename Iterator>
    Set(Iterator first, Iterator last) {
        root = nullptr;
        size_ = 0;
        while (first != last) {
            insert_to_tree(*first);
            ++first;
        }
    }

    Set(std::initializer_list<T> elems) {
        root = nullptr;
        size_ = 0;
        for (auto el : elems) {
            insert_to_tree(el);
        }
    }

    Set(const Set<T> &st) {
        destroy_vertex(root);
        size_ = 0;
        root = nullptr;
        for (auto &el : st) {
            insert(el);
        }
    }

    Set& operator=(const Set<T> &st) {
        if (this->root == st.root)  {
            return *this;
        }
        destroy_vertex(root);
        root = nullptr;
        size_ = 0;
        for (auto &el : st) {
            insert(el);
        }
        return *this;
    }

    ~Set() {
        destroy_vertex(root);
    }

    size_t size() const {
        return size_;
    }

    bool empty() const {
        return size_ == 0;
    }

    void insert(const T& elem) {
        insert_to_tree(elem);
    }

    void erase(const T& elem) {
        erase_in_tree(elem);
    }

    iterator begin() const {
        std::shared_ptr<Node> now = root;
        if (root == nullptr) {
            return end();
        } else {
            now = go_left(now);
            iterator it(now);
            return it;
        }
    }

    iterator end() const {
        std::shared_ptr<Node> now = root;
        if (root == nullptr) {
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

    iterator find(const T& elem) const {
        std::shared_ptr<Node> found = find_vertex(root, elem);
        if (found == nullptr) {
            return end();
        }
        if (!is_equal(found->max_l_, elem)) {
            return end();
        }
        iterator it(found);
        return it;
    }

    iterator lower_bound(const T& elem) const {
        std::shared_ptr<Node> found = find_vertex(root, elem);
        if (found == nullptr) {
            iterator it = end();
            return it;
        }
        if (found->max_l_ < elem) {
            iterator it = end();
            return it;
        }
        iterator it(found);
        return it;
    }

    static std::shared_ptr<Node> go_right(std::shared_ptr<Node> now) {
        if (now->ch_.empty()) {
            return now;
        } else if (now->ch_.size() == 2) {
            return go_right(now->ch_[1]);
        } else if (now->ch_.size() == 3) {
            return go_right(now->ch_[2]);
        } else {
            //std::cout << ">4";
            //std::cout << now->ch_.size() << std::endl;
            return nullptr;
        }
    }

    static std::shared_ptr<Node> go_left(std::shared_ptr<Node> now) {
        if (now->ch_.empty()) {
            return now;
        } else {
            return go_left(now->ch_[0]);
        }
    }

    void start_dfs() {
        dfs(root);
    }
private:

    bool is_equal(T el1, T el2) const {
        return !(el1 < el2) && !(el2 < el1);
    }

    std::shared_ptr<Node> find_vertex(std::shared_ptr<Node> now, T key) const {
        if (now == nullptr) {
            return nullptr;
        }
        if (now->ch_.empty()) {
            return now;
        }
        if (now->ch_.size() == 2) {
            if (now->max_l_ < key) {
                return find_vertex(now->ch_[1], key);
            } else {
                return find_vertex(now->ch_[0], key);
            }
        } else {
            if (now->max_mid_ < key) {
                return find_vertex(now->ch_[2], key);
            } else if (now->max_l_ < key){
                return find_vertex(now->ch_[1], key);
            } else {
                return find_vertex(now->ch_[0], key);
            }
        }
    }

    void make_new_root(std::shared_ptr<Node> p1, std::shared_ptr<Node> p2) {
        std::shared_ptr<Node> new_root = std::make_shared<Node>();
        if (p2->max_l_ < p1->max_l_) {
            new_root->max_l_ = p2->max_l_;
            new_root->ch_.push_back(p2);
            new_root->ch_.push_back(p1);
        } else {
            new_root->max_l_ = p1->max_l_;
            new_root->ch_.push_back(p1);
            new_root->ch_.push_back(p2);
        }
        new_root->max_ =  std::max(new_root->ch_[0]->max_, new_root->ch_[1]->max_);
        p1->par_ = new_root;
        p2->par_ = new_root;
        root = new_root;
    }

    static bool comp_children(std::shared_ptr<Node> p1, std::shared_ptr<Node> p2) {
        return (p1->max_l_ < p2->max_l_);
    }

    void sort_children(std::shared_ptr<Node> cur) {
        if (cur->ch_.size() == 1) {
            return;
        } else if (cur->ch_.size() == 2) {
            if (cur->ch_[1]->max_l_ < cur->ch_[0]->max_l_) {
                std::swap(cur->ch_[1], cur->ch_[0]);
            }
        } else if (cur->ch_.size() == 3) {
            if (cur->ch_[1]->max_l_ < cur->ch_[0]->max_l_ && cur->ch_[1]->max_l_ < cur->ch_[2]->max_l_) {
                std::swap(cur->ch_[1], cur->ch_[0]);
            } else if (cur->ch_[2]->max_l_ < cur->ch_[1]->max_l_ && cur->ch_[2]->max_l_ < cur->ch_[0]->max_l_) {
                std::swap(cur->ch_[2],cur->ch_[0]);
            }
            if (cur->ch_[2]->max_l_ < cur->ch_[1]->max_l_) {
                std::swap(cur->ch_[1], cur->ch_[2]);
            }
        } else {
            if (cur->ch_[1]->max_l_ < cur->ch_[0]->max_l_) {
                std::swap(cur->ch_[1], cur->ch_[0]);
            }
            if (cur->ch_[3]->max_l_ < cur->ch_[2]->max_l_) {
                std::swap(cur->ch_[2], cur->ch_[3]);
            }
            if (cur->ch_[2]->max_l_ < cur->ch_[0]->max_l_) {
                std::swap(cur->ch_[2], cur->ch_[0]);
            }
            if (cur->ch_[3]->max_l_ < cur->ch_[1]->max_l_) {
                std::swap(cur->ch_[3], cur->ch_[1]);
            }
            if (cur->ch_[2]->max_l_ < cur->ch_[1]->max_l_) {
                std::swap(cur->ch_[1], cur->ch_[2]);
            }
        }
    }

    void update_node(std::shared_ptr<Node> node) {
        if (node == nullptr) {
            return;
        }
        if (node->ch_.empty()) {
            return;
        }
        //sort(node->ch_.begin(), node->ch_.end(), comp_children);
        sort_children(node);
        node->max_l_ = node->ch_[0]->max_;
        node->max_ = node->max_l_;
        for (auto &el : node->ch_) {
            el->par_ = node;
            if (node->max_ < el->max_) {
                node->max_ = el->max_;
            }
        }
        if (node->ch_.size() > 2) {
            node->max_mid_ = node->ch_[1]->max_;
        }
    }

    void global_update(std::shared_ptr<Node> node) {
        if (node == nullptr) {
            return;
        }
        //std::cout << node->max_l_ << std::endl;
        update_node(node);
        //std::cout << "OK" << std::endl;
        global_update(node->par_);
    }

    void join_child_to_parent(std::shared_ptr<Node> par, std::shared_ptr<Node> ch) {
        par->ch_.push_back(ch);
        ch->par_ = par;
        update_node(par);
    }

    std::pair<std::shared_ptr<Node>, std::shared_ptr<Node>> split(std::shared_ptr<Node> node) {
        update_node(node);
        std::shared_ptr<Node> left = std::make_shared<Node>();
        std::shared_ptr<Node> right = std::make_shared<Node>();
        left->ch_.push_back(node->ch_[0]);
        left->ch_.push_back(node->ch_[1]);
        right->ch_.push_back(node->ch_[2]);
        right->ch_.push_back(node->ch_[3]);
        left->ch_[0]->par_ = left; // вроде норм
        left->ch_[1]->par_ = left;
        right->ch_[0]->par_ = right;
        right->ch_[1]->par_ = right;
        update_node(left);
        update_node(right);
        return make_pair(left, right);
    }

    void go_up(std::shared_ptr<Node> node) {
        if (node == nullptr) {
            return;
        }
        if (node->ch_.size() < 4) {
            return;
        }
        std::shared_ptr<Node> parent = node->par_;
        std::pair<std::shared_ptr<Node>, std::shared_ptr<Node>> pnn = split(node);
        if (parent == nullptr) {
            std::shared_ptr<Node> new_root = std::make_shared<Node>();
            new_root->ch_.push_back(pnn.first);
            new_root->ch_.push_back(pnn.second);
            pnn.first->par_=new_root;
            pnn.second->par_=new_root;
            root = new_root;
            update_node(root);
        } else {
            parent->ch_.erase(std::find(parent->ch_.begin(), parent->ch_.end(), node));
            parent->ch_.push_back(pnn.first);
            parent->ch_.push_back(pnn.second);
            pnn.first->par_ = parent;
            pnn.second->par_ = parent;
            update_node(parent);
            go_up(parent);
        }
    }

    void insert_to_tree(T key) {
        std::shared_ptr<Node> found = find_vertex(root, key);
        std::shared_ptr<Node> new_ver = std::make_shared<Node>(key);
        if (found == nullptr) {
            size_++;
            root = new_ver;
            return;
        }
        if (is_equal(found->max_l_, key)) {
            return;
        }
        size_++;
        if (found->par_ == nullptr) {
            make_new_root(root, new_ver);
            return;
        }
        join_child_to_parent(found->par_, new_ver);
        go_up(found->par_);
        global_update(found->par_);
    }

    std::shared_ptr<Node> get_uncle(std::shared_ptr<Node> cur) {
        if (cur->par_ == nullptr) {
            return nullptr;
        }
        std::shared_ptr<Node> par = cur->par_;

        if (cur != par->ch_[1]) {
            return par->ch_[1];
        } else {
            return par->ch_[0];
        }
    }

    void erase_up(std::shared_ptr<Node> cur) {
        if (cur == nullptr) {
            return;
        }
        //std::cout << "NOW" << std::endl;
        if (cur->ch_.size() > 1) {
            //std::cout << "HERE" << std::endl;
            return;
        }
        std::shared_ptr<Node> par = cur->par_;
        std::shared_ptr<Node> brother;
        if (par == nullptr) {
            root = cur->ch_[0];
            cur->ch_[0]->par_ = nullptr;
            cur->ch_.clear();
            return;
        }

        brother = par->ch_[1];
        if (cur == brother) {
            brother = par->ch_[0];
        }

        brother->ch_.push_back(cur->ch_[0]);
        cur->ch_[0]->par_ = brother;

        cur->ch_.clear();

        par->ch_.erase(std::find(par->ch_.begin(), par->ch_.end(), cur));
        cur->par_ = nullptr;

        if (brother->ch_.size() > 3) {
            std::pair<std::shared_ptr<Node>, std::shared_ptr<Node>> pnn = split(brother);
            par->ch_.push_back(pnn.first);
            par->ch_.push_back(pnn.second);
            pnn.first->par_ = par;
            pnn.second->par_ = par;
            par->ch_.erase(std::find(par->ch_.begin(), par->ch_.end(), brother));
            brother->ch_.clear();
            brother->par_ = nullptr;
            update_node(par);
        }

        erase_up(par);

    }

    void erase_in_tree(T key) {
        std::shared_ptr<Node> cur = find_vertex(root, key);
        //std::cout << cur->max_l_ << std::endl;
        if (cur == nullptr || !is_equal(cur->max_l_, key)) {
            return;
        }
        size_--;
        std::shared_ptr<Node> par = cur->par_;
        std::shared_ptr<Node> brother;
        if (par == nullptr) {
            root = nullptr;
            cur->par_ = nullptr;
            return;
        }

        brother = par->ch_[1];
        if (cur == brother) {
            brother = par->ch_[0];
        }
        //std::cout << brother->max_l_ << std::endl;
        par->ch_.erase(std::find(par->ch_.begin(), par->ch_.end(), cur));
        cur->par_ = nullptr;

        if (par->ch_.size() > 1) {
            global_update(par);
            return;
        }

        std::shared_ptr<Node> uncle = get_uncle(par);

        if (uncle == nullptr) {
            root = brother;
            root->par_ = nullptr;
            root->ch_.clear();
            return;
        }
        //std::cout << uncle->max_l_ << " " << uncle->max_mid_ << " " << uncle->max_ << std::endl;
        uncle->ch_.push_back(brother);
        par->ch_.erase(std::find(par->ch_.begin(), par->ch_.end(), brother));
        brother->par_ = uncle;
        //std::cout << par->ch_.size() << std::endl;
        if (uncle->ch_.size() > 3) {
            std::pair<std::shared_ptr<Node>, std::shared_ptr<Node>> pnn = split(uncle);

            //std::cout << pnn.first->max_l_ << " " << pnn.first->max_mid_ << " " << pnn.first->max_ << std::endl;
            //std::cout << pnn.second->max_l_ << " " << pnn.second->max_mid_ << " " << pnn.second->max_ << std::endl;

            uncle->par_->ch_.push_back(pnn.first);
            uncle->par_->ch_.push_back(pnn.second);

            pnn.first->par_ = uncle->par_;
            pnn.second->par_ = uncle->par_;

            uncle->par_->ch_.erase(std::find(uncle->par_->ch_.begin(), uncle->par_->ch_.end(), uncle));

            update_node(uncle->par_);

            uncle->par_ = nullptr;

            uncle->ch_.clear();

        }

        par->par_->ch_.erase(std::find(par->par_->ch_.begin(), par->par_->ch_.end(), par));

        std::shared_ptr<Node> next = par->par_;

        par->par_ = nullptr;
        par->ch_.clear();

        erase_up(next);
        //std::cout << brother->max_l_ << std::endl;
        global_update(brother);
    }

    void destroy_vertex(std::shared_ptr<Node> cur) {
        if (cur == nullptr) {
            return;
        }
        if (cur->ch_.empty()) {
            cur->par_ = nullptr;
        } else {
            for (auto el : cur->ch_) {
                destroy_vertex(el);
            }
            cur->par_ = nullptr;
            cur->ch_.clear();
        }
    }

    std::shared_ptr<Node> root = nullptr;

    size_t size_ = 0;

    void dfs(std::shared_ptr<Node> v) {
        if (v == nullptr) {
            return;
        }
        /*if (v->ch_.empty()) {
            std::cout << v->max_l_ << std::endl;
        }*/
        for (int i = 0; i < v->ch_.size(); ++i) {
            std::cout << "from ";
            std::cout << v->max_l_ << " " << v->max_mid_ << " " << v->max_ << std::endl;
            std::cout << " to ";
            std::cout << v->ch_[i]->max_l_ << " " << v->ch_[i]->max_mid_ << " " << v->ch_[i]->max_ << std::endl;
            //PrintNode(v->ch_[i]);
            dfs(v->ch_[i]);
        }
    }
};
