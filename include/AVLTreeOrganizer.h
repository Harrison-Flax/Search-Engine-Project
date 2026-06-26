#ifndef AVL_TREE_ORGANIZER_H
#define AVL_TREE_ORGANIZER_H

#include "AVLTree.h"
#include <string>
#include <set>
#include <list>

struct OrganizerEntry {
    std::string term;
    std::set<int> documentIds;

    OrganizerEntry(const std::string& t, const std::set<int>& ids)
        : term(t), documentIds(ids) {}
    
    OrganizerEntry(const std::string& t, int id)
        : term(t), documentIds({id}) {}

    void addDocumentID(int id) {
        documentIds.insert(id);
    }

    bool operator<(const OrganizerEntry& other) const {
        return term < other.term;
    }

    bool operator>(const OrganizerEntry& other) const {
        return term > other.term;
    }

    bool operator==(const OrganizerEntry& other) const {
        return term == other.term;
    }
};

class AVLTreeOrganizer : public AVLTree<OrganizerEntry> {
public:
    AVLTreeOrganizer() : AVLTree<OrganizerEntry>() {}
    ~AVLTreeOrganizer() {}

    // Insert document ID
    void insert(const std::string& key, int document) {
        AvlNode* node = this->findNode(this->root, OrganizerEntry(key, 0)); 

        if (node != nullptr) {
            // Key found, add document to the existing list
            node->element.addDocumentID(document);
        } else {
            // Key not found, create a new entry and insert
            AVLTree<OrganizerEntry>::insert(OrganizerEntry(key, document));
        }
    }

    // Return all document IDs with organization
    std::list<int> search(const std::string& key) {
        AvlNode* node = this->findNode(this->root, OrganizerEntry(key, 0));

        if (node != nullptr) {
            return std::list<int>(node->element.documentIds.begin(),
                                  node->element.documentIds.end());
        }
        return std::list<int>();
    }
};

#endif
                                
