#ifndef AVL_TREE_PERSON_H
#define AVL_TREE_PERSON_H

#include "AVLTree.h"
#include <string>
#include <set>
#include <list>

// Define the structure for Person Entries
struct PersonEntry {
    std::string term;
    // Set avoids duplicates
    std::set<int> documentIds;

    // Constructor with every ID
    PersonEntry(const std::string& t, const std::set<int>& ids)
        : term(t), documentIds(ids) {}

    // Constructor with only one ID
    PersonEntry(const std::string& t, int id)
        : term(t), documentIds({id}) {}
    
    void addDocument(int id) {
        documentIds.insert(id);
    }

    bool operator<(const PersonEntry& other) const {
        return term < other.term;
    }
    bool operator>(const PersonEntry& other) const {
        return term > other.term;
    }
    bool operator==(const PersonEntry& other) const {
        return term == other.term;
    }
};

class AVLTreePerson : public AVLTree<PersonEntry> {
public:
    AVLTreePerson() : AVLTree<PersonEntry>() {}
    ~AVLTreePerson() {}

    void insert(const std::string& key, int document) {
        // Assuming findNode() is a method in AVLTree to find a node by key
        AvlNode* node = this->findNode(this->root, PersonEntry(key, 0)); 

        if (node != nullptr) {
            // Key found, add document ID to the existing entry
            node->element.addDocumentID(document);
        } else {
            // Key not found, create a new entry and insert
            AVLTree<PersonEntry>::insert(PersonEntry(key, document));
        }
    }

    // Returning all of the documnt IDs that match the name of a person
    std::set<int> search(const std::string& key) {
        // Assuming findNode() is a method in AVLTree to find a node by key
        AvlNode* node = this->findNode(this->root, PersonEntry(key, 0));

        if (node != nullptr) {
            // Key found, return the set of document IDs
            return std::set<int>(node->element.documentIds.begin(),
                                 node->element.documentIds.end());
        }
        // Key not found, return an empty list
        return std::set<int>();
    }
};

#endif