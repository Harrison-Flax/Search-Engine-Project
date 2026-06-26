#ifndef AVL_TREE_WORDS_H
#define AVL_TREE_WORDS_H

#include "AVLTree.h"
#include <string>
#include <set>
#include <list>

struct WordEntry {
    std::string term;
    std::set<int> documentIds;

    // Constructor with all IDs
    WordEntry(const std::string& t, const std::set<int>& ids)
        : term(t), documentIds(ids) {}

    // Constructor with a single ID
    WordEntry(const std::string& t, int id)
        : term(t), documentIds({id}) {}

    void addDocumentID(int id) {
        documentIds.insert(id);
    }

    // Comparison operators
    bool operator<(const WordEntry& other) const {
        return term < other.term;
    }

    bool operator>(const WordEntry& other) const {
        return term > other.term;
    }

    bool operator==(const WordEntry& other) const {
        return term == other.term;
    }
};

// Stemmed and non-stopword tokens only
class AVLTreeWords : public AVLTree<WordEntry> {
private:
    int nodeCount;

public:
    AVLTreeWords() : AVLTree<WordEntry>(), nodeCount(0) {}
    ~AVLTreeWords() {}

    // Insert document IDs for word key
    void insert(const std::string& key, int document) {
        AvlNode* node = this->findNode(this->root, WordEntry(key, 0));
        if (node != nullptr) {
            // Adding the document ID
            node->element.addDocumentID(document);
        } else {
            // Insert new WordEntry with the document ID
            AVLTree<WordEntry>::insert(WordEntry(key, document));
            ++nodeCount;
        }
    }

    // Return all the document IDs that contain the specific word
    std::list<int> search(const std::string& key) {
        AvlNode* node = this->findNode(this->root, WordEntry(key, 0));
        if(node != nullptr) {
            return std::list<int>(node->element.documentIds.begin(),
                                  node->element.documentIds.end());  
        }
        return std::list<int>();
    }

    // Total size
    int size() const {
        return nodeCount;
    }
    
};

#endif