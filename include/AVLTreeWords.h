#pragma once

#include "AVLTree.h"
#include <string>
#include <list>

template<typename Comparable>
class AVLTreeWords : public AVLTree<WordEntry> {
public:
    AVLTreeWords() : AVLTree<WordEntry>() {}
    ~AVLTreeWords() {}

    void insert(const std::string& key, int document) {
        AVLNode* node = this->findNode(this->root, WordEntry(key, {})); 

        if (node != nullptr) {
            // Key found, add document to the existing list
            node->element.documentIds.push_back(document);
        } else {
            // Key not found, create a new entry and insert
            WordEntry newEntry(key, {document});
            AVLTree<WordEntry>::insert(newEntry);
        }
    }

    std::list<int> search(const std::string& key) {
        AVLNode* node = this->findNode(this->root, WordEntry(key, {}));

        if (node != nullptr) {
            // Key found, return the list of documents
            return node->element.documentIds;
        } else {
            // Key not found, return an empty list
            return std::list<int>();
        }
    }

private:
    // None rn
};
