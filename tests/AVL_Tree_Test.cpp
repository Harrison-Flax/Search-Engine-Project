#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "AVLTree.h"
#include "doctest.h"
#include <list>

//Using doctest
DOCTEST_TEST_CASE("AVL Tree Balance Check") { //doctest only takes one parameter for test case
    AVLTree<int> tree;
    tree.insert(10);
    tree.insert(20);
    tree.insert(5);

    // Check if the tree is balanced using the provided function
    REQUIRE_NOTHROW(tree.checkBalance());
}

DOCTEST_TEST_CASE("Insertion and Balance Check") {
    AVLTree<int> tree;
    tree.insert(3);
    tree.insert(2);
    tree.insert(1); // Should cause a rotation
    tree.insert(4);
    tree.insert(5); // Another rotation
    REQUIRE_NOTHROW(tree.checkBalance());
}

DOCTEST_TEST_CASE("Removal and Balance Check") {
    AVLTree<int> tree;
    tree.insert(10);
    tree.insert(20);
    tree.insert(30);
    tree.remove(20);
    REQUIRE_NOTHROW(tree.checkBalance());
}

DOCTEST_TEST_CASE("Containment of Elements") {
    AVLTree<int> tree;
    tree.insert(10);
    tree.insert(20);
    tree.insert(30);
    REQUIRE(tree.contains(20) == true);
    REQUIRE(tree.contains(40) == false);
}

DOCTEST_TEST_CASE("Copy Constructor") { 
    AVLTree<int> tree1;
    tree1.insert(10);
    tree1.insert(20);
    AVLTree<int> tree2 = tree1;
    REQUIRE_NOTHROW(tree2.checkBalance());
}

DOCTEST_TEST_CASE("Deletion") {
    AVLTree<int> tree;
    tree.insert(10);
    tree.insert(20);
    tree.insert(30);
    tree.remove(20);
    REQUIRE(tree.contains(20) == false);
}

DOCTEST_TEST_CASE("Empty Tree") {
    AVLTree<int> tree;
    REQUIRE(tree.isEmpty() == true);
}

DOCTEST_TEST_CASE("Non-Empty Tree") {
    AVLTree<int> tree;
    tree.insert(1);
    REQUIRE(tree.isEmpty() == false);
}

DOCTEST_TEST_CASE("Print Tree") {
    AVLTree<int> tree;
    tree.insert(15);
    tree.insert(10);
    tree.insert(45);
    tree.insert(1);
    tree.insert(8);

    //Print the tree contents
    tree.printTree();

    //Use REQUIRE to check if the tree is printed correctly
    REQUIRE(tree.contains(15) == true);
    REQUIRE(tree.contains(10) == true);
    REQUIRE(tree.contains(45) == true);
    REQUIRE(tree.contains(1) == true);
    REQUIRE(tree.contains(8) == true);
}

DOCTEST_TEST_CASE("Contains") {
    AVLTree<int> tree;
    tree.insert(15);
    tree.insert(10);
    tree.insert(45);
    REQUIRE(tree.contains(15) == true);
    REQUIRE(tree.contains(10) == true);
    REQUIRE(tree.contains(45) == true);
    REQUIRE(tree.contains(0) == false);
}

DOCTEST_TEST_CASE("Find Min") {
    AVLTree<int> tree;
    tree.insert(15);
    tree.insert(10);
    tree.insert(45);
    tree.insert(1);
    tree.insert(8);

    //Find the minimum in the tree
    AVLTree<int>::AvlNode* min = tree.findMin(tree.root);

    //Use REQUIRE to check if the minimum is correct
    REQUIRE(min != nullptr); //min should not be null
    REQUIRE(min->element == 1);
}

DOCTEST_TEST_CASE("Left Rotation") {
        AVLTree<int> tree;
        tree.insert(15);
        tree.insert(10);
        tree.insert(45);
        tree.insert(1);
        tree.insert(8);
        tree.rotateWithLeftChild(tree.root);
        // Verify specific node placements, balance, and heights
        REQUIRE(tree.root->element == 8); // After left rotation, 8 becomes the new root
        REQUIRE(tree.root->left->element == 1); // 1 should be the left child of the new root
        REQUIRE(tree.root->right->element == 15); // 15 should be the right child of the new root
}

DOCTEST_TEST_CASE("Right Rotation") {
        AVLTree<int> tree;
        tree.insert(15);
        tree.insert(10);
        tree.insert(45);
        tree.insert(1);
        tree.insert(8);
        tree.rotateWithRightChild(tree.root);
        // Verify specific node placements, balance, and heights
        REQUIRE(tree.root->element == 45); // After right rotation, 45 becomes the new root
        REQUIRE(tree.root->left->element == 15); // 15 should be the left child of the new root
        REQUIRE(tree.root->right->element == 45); // 45 should be the right child of the new root
}

DOCTEST_TEST_CASE("Double Left-Right Rotation") {
        AVLTree<int> tree;
        tree.insert(15);
        tree.insert(10);
        tree.insert(45);
        tree.insert(1);
        tree.insert(8);
        tree.doubleWithLeftChild(tree.root);
        // Verify specific node placements, balance, and heights
        REQUIRE(tree.root->element == 10); // After double left-right rotation, 10 becomes the new root
}

DOCTEST_TEST_CASE("Double Right-Left Rotation Test") {
        AVLTree<int> tree;
        tree.insert(15);
        tree.insert(10);
        tree.insert(45);
        tree.insert(1);
        tree.insert(8);
        tree.doubleWithRightChild(tree.root);
        // Verify specific node placements, balance, and heights
        REQUIRE(tree.root->element == 15); // After double right-left rotation, 15 becomes the new root
}

DOCTEST_TEST_CASE("Make Empty") {
    AVLTree<int> tree;
    tree.insert(15);
    tree.insert(10);
    tree.insert(45);
    tree.makeEmpty();
    REQUIRE(tree.isEmpty() == true);
}

DOCTEST_TEST_CASE("Clone") {
    AVLTree<int> tree;
    AVLTree<int>::AvlNode* clonedTree;
    tree.insert(15);
    tree.insert(10);
    tree.insert(45);
    
    //insert clone call
    clonedTree = tree.clone(tree.root);

    REQUIRE(tree.contains(15, tree.root) == true);
    REQUIRE(tree.contains(10, tree.root) == true);
    REQUIRE(tree.contains(45, tree.root) == true);
}