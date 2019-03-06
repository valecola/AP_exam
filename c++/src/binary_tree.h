/**
* @file binary_tree.cpp
*
* @brief Binary tree implementation
*
* @author Andrea Lorenzon, Francesco Cicala
*
*
*/
#pragma once

#include <algorithm> // find_if
#include <memory>    // std::unique_ptr
#include <iomanip>   // cout alignment
#include <iostream>  // std::cout, endl
#include <math.h>    // pow()
#include <vector>    // for tree balancing
#include <utility>   // pair



/*! namespace for things not directly able to interact with Tree */
namespace detail {

    template <class K, class T>
    struct Node {
        using data_type = std::pair<const K, T>;

        data_type data;
        /*! unique pointer to the left node */
        std::unique_ptr<Node> left  = nullptr;
        /*! unique pointer to the right node*/
        std::unique_ptr<Node> right = nullptr;
        Node * parent = nullptr;


        Node() = default;
        /*! node constructor by declared value*/
        Node(K k, T val)
        : data(k, val)
        {};
    };

    /*!clone a node, including children pointers and parent*/
    template <class K, class T>
    std::unique_ptr<Node<K, T>> clone(const Node<K, T> * old) {
        auto node = std::make_unique<Node<K, T>>(old->data.first, old->data.second);
        if (old->left) {
            node->left = clone(old->left.get());
            node->left->parent = node.get();
        };
        if (old->right) {
            node->right = clone(old->right.get());
            node->right->parent = node.get();
        };
        return node;
    }

    /*! helper function to traverse left nodes until there is any, giving the min(key) */
    template <typename NodeType>
    NodeType * allLeft(NodeType * node) noexcept {
        while (node->left) { node = node->left.get(); }
        return node;
    }

    /*! helper function to traverse right nodes until there is any, giving max(key)*/
    template <typename NodeType>
    NodeType * allRight(NodeType * node) noexcept {
        while (node->right) { node = node->right.get(); }
        return node;
    }

    /*! helper function to return next node*/
    template <typename NodeType>
    NodeType * successor(NodeType * node) noexcept {
        if (node->right) { return allLeft(node->right.get()); }
        auto * parent = node->parent;
        while (parent && node == parent->right.get()) {
            node = parent;
            parent = node->parent;
        }
        return parent;
    }
}


/*! Implements a binary search tree, templated on key and values */
template <class K, class T>
class Tree {
    

    using Node = detail::Node<K, T>;  // to use directly Node, from detail namespace

    /*! pointer to tree root node*/
    std::unique_ptr<Node> root = nullptr;

    /*! recursive helper function, called by balance() function*/
    void recursive_balancer(std::vector<typename Node::data_type>vec) {       // to do: set to private.

        if (vec.size() < 3)
        {
            for(auto & item : vec)
                this->insert(item.first, item.second);
        }
        else
        {
            this->insert(vec[vec.size() / 2].first, vec[vec.size() / 2].second);
            std::vector<typename Node::data_type> firstHalf(vec.begin(), vec.begin() + vec.size()/2);
            std::vector<typename Node::data_type> secondHalf(vec.begin() + vec.size()/2 + 1, vec.end());
            recursive_balancer(firstHalf);
            recursive_balancer(secondHalf);
        }
    }


//////////////////////////////////////// PUBLIC /////////////////////////////
public:

    /*! constructor */
    Tree (){};

    /*! Move constructor*/
    Tree ( Tree <K,T>&& other) noexcept = default;
    
    /*! Copy assignment */
    Tree & operator=(Tree&& bt) noexcept = default;
    Tree & operator=(const Tree& bt) noexcept = default;

    /*! Copy constructor */
    Tree (const Tree & other)
    : root(clone(other.root.get())),
      height(other.height) 
    {};
        
    

    /*! default destructor */
    ~Tree() noexcept = default;



    /*! Helper function. True if root == nullptr*/
    bool empty() const {
        return root == nullptr;
    }

    /*! Returns tree height */
    size_t height = 0;




/////////////////////////////// ITERATOR TEMPLATE //////////////////////////////

    template <bool Const>
    class iterator_template {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = decltype(Node::data);
        using difference_type = std::ptrdiff_t; //  is the signed integer type of the result of subtracting two pointers. 
        using reference = typename std::conditional_t<Const, const value_type &, value_type &>;  // conditional_t provides member typedef type, which is defined as T1 if B is true at compile time, or as T2 if B is false.
        using pointer = typename std::conditional_t<Const, const value_type *, value_type *>;    // conditional_t provides member typedef type, which is defined as T1 if B is true at compile time, or as T2 if B is false.

    public :
        iterator_template() = default;
        explicit iterator_template(Node * ptr) : itr(ptr) {}

        reference operator*() { return itr->data; };
        pointer operator->() { return &itr->data; }
        iterator_template & operator++() {
            itr = successor(itr);
            return *this;
        }
        iterator_template operator++(int) {
            auto old = *this;
            ++(*this);
            return old;
        }
        friend bool operator== (const iterator_template<Const> & lhs, const iterator_template<Const> & rhs) {
            return lhs.itr == rhs.itr;
        };
        friend bool operator!= (const iterator_template<Const> & lhs, const iterator_template<Const> & rhs) {
            return !(lhs == rhs);
        };
    
        Node * get_node() const { return itr; }
    protected:       // allows inheritance
        Node * itr = nullptr;
    };


    
    using iterator = iterator_template<false>;   // creates iterators
    using const_iterator = iterator_template<true>; // creates const_iterators

////////////////////// end iterators ///////////////////////////////////
// detail:: is automatically called by ADL (see Koenig lookup)

    
    /*! iterator to the node with the lowest key*/
    iterator       begin()        { return iterator(allLeft(root.get())); }
    /*! iterator to the node with the lowest key, for const Trees*/
    const_iterator begin()  const { return const_iterator(allLeft(root.get())); }
    /*! const iterator to the node with the lowest key, for const Trees*/
    const_iterator cbegin() const { return const_iterator(allLeft(root.get())); }

    /*! iterator to the node after the one with the highest key (so nullptr)*/
    iterator       end()          { return iterator(nullptr); }
    /*! iterator to the node after the one with the highest key (so nullptr), for const Trees*/
    const_iterator end()  const   { return const_iterator(nullptr);}
    /*! const iterator to the node after the one with the highest key (so nullptr), for const Trees*/
    const_iterator cend() const   { return const_iterator(nullptr); }

    void insert(const K key, const T value) { /*! add a node provided key and value compatible with the tree */

       /* @brief
        *
        * input: K key, T value. Must match tree K and T types.
        * output: none
        * actions: if root is nullptr, set the new node as root.
        * otherwise, recursively compares keys.
        * if equal, overwrites value.
        * if less or greater, recursively calls itself respectively on left or right child
        *
        *
        */
        Node * current = root.get();
        Node * parent = nullptr;

        size_t temp_height = 0;
        while (current) {
            parent = current;
            if (key > current->data.first) {
                current = current->right.get();
                temp_height++;
                if (temp_height > this->height) { this->height = temp_height;}
            } else if (key < current->data.first) {
                current = current->left.get();
                temp_height++;
                if (temp_height > this->height) { this->height = temp_height;}
            } else {
                current->data.second = value;
                return;
            }
        }

        auto newNode = std::make_unique<Node>(key, value);
        current = newNode.get();
        if (parent) {
            newNode->parent = parent;
            if (newNode->data.first > parent->data.first) {
                parent->right = std::move(newNode);
                // std::cout <<"created node (" <<std::setw(3) <<key
                //           <<", " <<std::setw(3) <<value <<") [" <<current
                //           <<"] on right of " <<parent->key <<std::endl;
            } else {
                parent->left = std::move(newNode);
                // std::cout <<"created node (" <<std::setw(3) <<key
                //           <<", " <<std::setw(3) <<value <<") [" <<current
                //           <<"] on left of " <<parent->key <<std::endl;
            }
        } else {
            root = std::move(newNode);
            // std::cout <<"created node (" <<std::setw(3) <<key
            //           <<", " <<std::setw(3) <<value <<") [" <<current
            //           <<"] at root" <<std::endl;
        }
        return;
    };

    void removeNode(K k) {/*! remove the node corresponding to the given key */
        /* @brief
        *
        * input: K k, the key corresponding to the node to be removed.
        * output: none.
        *
        */

        auto * toBeRemoved = root.get();
        while (toBeRemoved && toBeRemoved->data.first != k) {
            toBeRemoved = (k < toBeRemoved->data.first ? toBeRemoved->left : toBeRemoved->right).get();
        }
        if (!toBeRemoved) { return; }

        /* CASE 0: the node is root */
        if (toBeRemoved->parent == nullptr) {
          //std::cout << "\nCase 0\n";
          //destroy();  <-- sure about that?
        }

        else {
          /* Is it the left child or right child? */
          int leftOrRight{0};
          if (toBeRemoved->parent->right.get() == toBeRemoved)
            leftOrRight = 1;

          /* CASE 1: the node is a leaf */
          if (!(toBeRemoved->left || toBeRemoved->right)) {
            if (toBeRemoved->parent->left.get() == toBeRemoved)
              toBeRemoved->parent->left = std::move(nullptr);
            else
              toBeRemoved->parent->right = std::move(nullptr);
          }
          /* CASE 2: the node has two children */
          else if (toBeRemoved->left && toBeRemoved->right) {
            auto * right = toBeRemoved->right.get();
            auto * replacementPtr = allLeft(right);

            // Take replacement from its parent
            auto replacement = replacementPtr == right
                             ? std::move(toBeRemoved->right)
                             : std::move(replacementPtr->parent->left);

            // If replacement had right children, stick them on its former parent
            if (replacement->right) {
                replacement->right->parent = replacement->parent;
                if (replacementPtr == right) {
                    toBeRemoved->right = std::move(replacement->right);
                } else {
                    replacement->parent->left = std::move(replacement->right);
                }
            }

            // At this point, replacement is fully extracted from tree.
            // Now insert it at its new place

            //steal toBeRemoved left child
            replacement->left = std::move(toBeRemoved->left);
            if (replacement->left) replacement->left->parent = replacement.get();

            //steal toBeRemoved right children
            replacement->right = move(toBeRemoved->right);
            if (replacement->right) replacement->right->parent = replacement.get();

            //replace old node with cut out node
            replacement->parent = toBeRemoved->parent;
            if (toBeRemoved == toBeRemoved->parent->left.get()) {
                replacement->parent->left =  std::move(replacement);
            } else {
                replacement->parent->right = std::move(replacement);
            }

          }
        }
    }

    void listNodes() {                      /*! iterates the tree in order, printing key-value entries */
        for (auto i = begin(); i != end(); ++i) {
          std::cout << "iterated node: (" <<i->key <<", " <<i->value <<")" << std::endl;

        };
    };

    iterator find(K k) const {   /*! traverse the tree looking for a key, otherwise return nullptr iterator, that is the same as end()*/
        auto * node = root.get();
        while (node && node->data.first != k) {
            node = (k < node->data.first ? node->left : node->right).get();
        }
        return iterator(node);
    }
    /*! tree deletion: sets root to nullptr, destroy recursively all nodes*/
    void destroy(){
        this->root = nullptr;
    }



    /*!< tree balance function. calls arrayOfNodes() to linearize the tree, then creates a balanced tree*/
    void balance()
    {
        this->height = 0;
        std::vector<typename Node::data_type> vector(begin(), end()); //salviamo i nodi iterati in  un vector, in ordine di key
        this->root = nullptr;  // cancelliamo tutti i nodi senza distruggere l'albero
        this->recursive_balancer(vector); // ricreiamo l'albero dal vector
    };
};



template<class K, class T>                                                       ///   DA FINIRE!
std::ostream& operator<<(std::ostream& ostream, const Tree<K,T>& tree) {

    if (tree.empty()) {
        ostream << "Error: printing empty tree";
        return ostream;
    }
    for (auto t=tree.cbegin();t!=tree.cend();++t){
        ostream << std::left << std::setw(12)<< t->first << ":" << t->second << "\n";
    }
  return ostream;
}
