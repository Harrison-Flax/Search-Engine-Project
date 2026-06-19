#ifndef AVLORGANIZER_H
#define AVLORGANIZER_H

#include <string>
#include <list>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <iostream>

template <typename Comparable>
struct AvlNode {
        Comparable Key;
        std::list<std::string> documents;
        AvlNode *left;
        AvlNode *right;
        int height;

        // Constructor for an AVL tree node
        AvlNode(const Comparable &ele, AvlNode *lt, AvlNode *rt, int h = 0)
            : Key{ele}, left{lt}, right{rt}, height{h} {}
    };

template <typename Comparable>
class AVLOrganizer {
private:
    AvlNode<Comparable>* root;

    // Helper function to delete all nodes (recursive)
    void makeEmpty(AvlNode<Comparable>*& t) {
        if (t != nullptr) {
            makeEmpty(t->left);
            makeEmpty(t->right);
            delete t;
        }
        t = nullptr;
    }

public:
    AVLOrganizer() {
        root = nullptr;
    };

    ~AVLOrganizer() {
        makeEmpty(root);
    };

    void insert(const std::string& Key, const std::list<std::string>& documents) {
        insert(Key, documents, root);
    };
    
    // Rotation functions
    AvlNode<Comparable>* rotateLeft(AvlNode<Comparable>* node);
    AvlNode<Comparable>* rotateRight(AvlNode<Comparable>* node);

    // Balancing function
    AvlNode<Comparable>* balance(AvlNode<Comparable>* node);

    // Get height of a node
    int getHeight(AvlNode<Comparable>* node);

    std::string serialize() {
        rapidjson::StringBuffer s;
        rapidjson::Writer<rapidjson::StringBuffer> writer(s);

        writer.StartObject(); // Start an object for serialization

        writer.Key("root");
        serializeHelper(root, writer);

        writer.EndObject(); // End the object

        return s.GetString();
    }

    void serializeHelper(AvlNode<Comparable>* node, rapidjson::Writer<rapidjson::StringBuffer>& writer) {
        if (node) {
            writer.StartObject();

            writer.Key("key");
            writer.String(node->Key.c_str()); // Serialize the string directly

            // Serialize other node members as needed (e.g., documents)
            writer.Key("documents");
            writer.StartArray();
            for (const auto& document : node->documents) {
                writer.String(document.c_str());
            }
            writer.EndArray();

            // Recursively serialize left and right children
            writer.Key("left");
            serializeHelper(node->left, writer);

            writer.Key("right");
            serializeHelper(node->right, writer);

            writer.EndObject();
        } else {
            writer.Null(); // Handle null nodes
        }
    }

    void deserialize(const std::string& serializedData) {
        rapidjson::Document document;
        document.Parse(serializedData.c_str());

        if (!document.IsObject()) {
            std::cerr << "Invalid JSON format for deserialization" << std::endl;
            return;
        }

        if (document.HasMember("root")) {
            root = deserializeHelper(document["root"]);
        }
    }

    AvlNode<Comparable>* deserializeHelper(const rapidjson::Value& node) {
         if(node.IsNull()) {
            return nullptr;
         }

        AvlNode<Comparable>* newNode = nullptr;

        if (node.HasMember("key") && node["key"].IsString()) {
        // Create an AvlNode with the key, initializing left and right pointers to nullptr
        newNode = new AvlNode<Comparable>(node["key"].GetString(), nullptr, nullptr);
    } else {
        // Handle the case where the key is missing or not a string
        // Log an error to handle this case
        std::cerr << "Invalid JSON format for deserialization" << std::endl;
        return nullptr;
    }

    // Deserialize other node members as needed
    if (newNode) {
        newNode->left = deserializeHelper(node["left"]);
        newNode->right = deserializeHelper(node["right"]);
    }

    return newNode;
}
};

#endif
