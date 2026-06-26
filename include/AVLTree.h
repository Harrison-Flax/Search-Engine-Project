// use chapter4_Tree/AVLTree/AVLTree.h on the github.

// https://github.com/mhahsler/CS2341/tree/main/Chapter4_Trees/AVLTree

/*You must implement your own version of an AVL tree and persistence mechanism for the main
word index. You may, of course, refer to other implementations for guidance, but you MAY NOT
incorporate the total implementation from a source different than the class notes.*/

#ifndef AVL_TREE_H
#define AVL_TREE_H

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <string>

// Define the TermEntry struct for the inverted file index
struct TermEntry
{
    std::string term;
    std::vector<int> documentIds;

    TermEntry(const std::string &t, const std::vector<int> &ids)
        : term(t), documentIds(ids) {}

    bool operator<(const TermEntry &other) const
    {
        return term < other.term;
    }
    bool operator>(const TermEntry &other) const
    {
        return term > other.term;
    }
    bool operator==(const TermEntry &other) const
    {
        return term == other.term;
    }
};

template <typename Comparable>
class AVLTree {
public:
    // AvlNode needs to be public so the child classes and test cases can access it easily
    struct AvlNode {
        Comparable element;
        AvlNode* left;
        AvlNode* right;
        int height;

        AvlNode(const Comparable& ele, AvlNode* lt, AvlNode* rt, int h = 0)
            : element{ele}, left{lt}, right{rt}, height{h} {}
    };

    using Node = AvlNode;

    AVLTree() : root{nullptr} {}

    AVLTree(const AVLTree& rhs) : root{nullptr} {
        root = clone(rhs.root);
    }

    ~AVLTree() {
        makeEmpty();
    }

    AVLTree& operator=(const AVLTree& rhs) {
        AVLTree copy = rhs;
        std::swap(*this, copy);
        return *this;
    }

    void insert(const Comparable& x) {
        insert(x, root);
    }

    void remove(const Comparable& x) {
        remove(x, root);
    }

    void makeEmpty() {
        makeEmpty(root);
    }

    bool isEmpty() const {
        return root == nullptr;
    }

    bool contains(const Comparable& x) const {
        return contains(x, root);
    }

    void printTree() const {
        if (isEmpty())
            std::cout << "Empty tree" << std::endl;
        else
            printTree(root);
    }

    int checkBalance() const {
        return checkBalance(root);
    }

    AvlNode* root;

    AvlNode* findNode(AvlNode* t, const Comparable& x) const {
        if (t == nullptr)
            return nullptr;
        else if (x < t->element)
            return findNode(t->left, x);
        else if (x > t->element)
            return findNode(t->right, x);
        else
            return t;
    }

    void insert(const Comparable& x, AvlNode*& t) {
        if (t == nullptr)
            t = new AvlNode{x, nullptr, nullptr};
        else if (x < t->element)
            insert(x, t->left);
        else if (x > t->element)
            insert(x, t->right);
        balance(t);
    }

    void remove(const Comparable& x, AvlNode*& t) {
        if (t == nullptr)
            return;
        if (x < t->element)
            remove(x, t->left);
        else if (x > t->element)
            remove(x, t->right);
        else if (t->left != nullptr && t->right != nullptr)
        {
            t->element = findMin(t->right)->element;
            remove(t->element, t->right);
        }
        else {
            AvlNode* oldNode = t;
            t = (t->left != nullptr) ? t->left : t->right;
            delete oldNode;
        }
        balance(t);
    }

    // Never exceed 1 or else have to rebalance the tree
    // Purpose of AVL Trees
    static const int ALLOWED_IMBALANCE = 1;

    void balance(AvlNode*& t) {
        if (t == nullptr)
            return;

        if (height(t->left) - height(t->right) > ALLOWED_IMBALANCE) {
            if (height(t->left->left) >= height(t->left->right))
                rotateWithLeftChild(t);
            else
                doubleWithLeftChild(t);
        }
        else if (height(t->right) - height(t->left) > ALLOWED_IMBALANCE) {
            if (height(t->right->right) >= height(t->right->left))
                rotateWithRightChild(t);
            else
                doubleWithRightChild(t);
        }
        t->height = std::max(height(t->left), height(t->right)) + 1;
    }

    // All rotations
    void rotateWithLeftChild(AvlNode*& k2) {
        AvlNode* k1 = k2->left;
        k2->left = k1->right;
        k1->right = k2;
        k2->height = std::max(height(k2->left), height(k2->right)) + 1;
        k1->height = std::max(height(k1->left), k2->height) + 1;
        k2 = k1;
    }

    void rotateWithRightChild(AvlNode*& k1) {
        AvlNode* k2 = k1->right;
        k1->right = k2->left;
        k2->left = k1;
        k1->height = std::max(height(k1->left), height(k1->right)) + 1;
        k2->height = std::max(height(k2->right), k1->height) + 1;
        k1 = k2;
    }

    void doubleWithLeftChild(AvlNode*& k3) {
        rotateWithRightChild(k3->left);
        rotateWithLeftChild(k3);
    }

    void doubleWithRightChild(AvlNode*& k1) {
        rotateWithLeftChild(k1->right);
        rotateWithRightChild(k1);
    }

    // Rest of tree functions
    AvlNode* findMin(AvlNode* t) const {
        if (t == nullptr)
            return nullptr;
        if (t->left == nullptr)
            return t;
        return findMin(t->left);
    }

    void makeEmpty(AvlNode*& t) {
        if (t != nullptr) {
            makeEmpty(t->left);
            makeEmpty(t->right);
            delete t;
        }
        t = nullptr;
    }

    void printTree(AvlNode* t, const std::string& indent = "") const {
        if (t != nullptr) {
            std::cout << indent << t->value << std::endl;
            printTree(t->left, indent + "  ");
            printTree(t->right, indent + "  ");
        }
    }

    AvlNode* clone(AvlNode* t) const {
        if (t == nullptr)
            return nullptr;
        return new AvlNode{t->element, clone(t->left), clone(t->right), t->height};
    }

    bool contains(const Comparable& x, AvlNode* t) const {
        if (t == nullptr)
            return false;
        else if (x < t->element)
            return contains(x, t->left);
        else if (x > t->element)
            return contains(x, t->right);
        else
            return true;
    }

    int height(AvlNode* t) const {
        return t == nullptr ? -1 : t->height;
    }

    int check_balance(AvlNode *node)
    {
        if (node == nullptr)
        {
            return -1;
        }

        // use LRN traversal to check the balance of the tree
        int lHeight = check_balance(node->left) + 1;
        int rHeight = check_balance(node->right) + 1;

        if (std::abs(lHeight - rHeight) > ALLOWED_IMBALANCE)
        {
            throw std::runtime_error("tree is not balanced in node with key " + std::to_string(node->key) + ".");
        }

        int trueNodeHeight = std::max(lHeight, rHeight);

        if (trueNodeHeight != node->height)
        {
            throw std::runtime_error("node does not have correct height value in node with key " + std::to_string(node->key) + ".");
        }

        return trueNodeHeight;
    }
};

#endif