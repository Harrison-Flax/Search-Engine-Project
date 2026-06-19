#pragma once

#include "AVLTree.h"
#include <string>
#include <list>

template<typename Comparable>
class AVLTreeOrganizer : public AVLTree<OrganizerEntry> {
public:
    AVLTreeOrganizer() : AVLTree<OrganizerEntry>() {}
    ~AVLTreeOrganizer() {}

    void insert(const std::string& key, int document) {
        AVLNode* node = this->findNode(this->root, OrganizerEntry(key, {})); 

        if (node != nullptr) {
            // Key found, add document to the existing list
            node->element.documentIds.push_back(document);
        } else {
            // Key not found, create a new entry and insert
            OrganizerEntry newEntry(key, {document});
            AVLTree<OrganizerEntry>::insert(newEntry);
        }
    }

    std::list<int> search(const std::string& key) {
        AVLNode* node = this->findNode(this->root, OrganizerEntry(key, {}));

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
