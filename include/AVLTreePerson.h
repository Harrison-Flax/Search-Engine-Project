#pragma once

#include "AVLTree.h"
#include <string>
#include <list>

// Define the structure for Person Entries
struct PersonEntry {
    std::string name;
    std::list<int> documentIds;

    PersonEntry(const std::string& n, const std::list<int>& ids)
        : name(n), documentIds(ids) {}

    bool operator<(const PersonEntry& other) const {
        return name < other.name;
    }
    bool operator>(const PersonEntry& other) const {
        return name > other.name;
    }
    bool operator==(const PersonEntry& other) const {
        return name == other.name;
    }
};
template<typename Comparable>
class AVLTreePerson : public AVLTree<PersonEntry> {
public:
    AVLTreePerson() : AVLTree<PersonEntry>() {}
    ~AVLTreePerson() {}

    void insert(const std::string& key, int document) {
        // Assuming findNode() is a method in AVLTree to find a node by key
        AVLNode* node = this->findNode(this->root, PersonEntry(key, {})); 

        if (node != nullptr) {
            // Key found, add document to the existing list
            node->element.documentIds.push_back(document);
        } else {
            // Key not found, create a new entry and insert
            PersonEntry newEntry(key, {document});
            AVLTree<PersonEntry>::insert(newEntry);
        }
    }

    std::list<int> search(const std::string& key) {
        // Assuming findNode() is a method in AVLTree to find a node by key
        AVLNode* node = this->findNode(this->root, PersonEntry(key, {}));

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
