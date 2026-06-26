#ifndef AVL_ORGANIZER_H
#define AVL_ORGANIZER_H

#include "AVLTree.h"
#include "AVLTreeWords.h"
#include "AVLTreePerson.h"
#include "AVLTreeOrganizer.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <iostream>
#include <string>
#include <list>

// Manages three AVL trees with insertion, searching, and seralize/deserialize
template <typename Comparable>
class AVLOrganizer {
private:
    AVLTreeWords wordIndex;
    AVLTreePerson personIndex;
    AVLTreeOrganizer orgIndex;

public:
    AVLOrganizer() {}
    ~AVLOrganizer() {}

    void insert(const std::string& key, int documentId,
                const std::string& type = "word") {
        if (type == "person") {
            personIndex.insert(key, documentId);
        }
        else if (type == "org") {
            orgIndex.insert(key, documentId);
        }
        else {
            wordIndex.insert(key, documentId);
        }
    }

    std::list<int> search(const std::string& key, const std::string& type = "word") {
        if (type == "person") {
            return personIndex.search(key);
        }
        else if (type == "org") {
            return orgIndex.search(key);
        }
        else {
            return wordIndex.search(key);
        }
    }

    std::string serialize() {
        rapidjson::StringBuffer sb;
        rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
        writer.StartObject();

        writer.Key("words");
        serializeTree(wordIndex.root, writer);

        writer.Key("persons");
        serializeTree(personIndex.root, writer);

        writer.Key("orgs");
        serializeTree(orgIndex.root, writer);

        writer.EndObject();
        return sb.GetString();
    }

    void deserialize(const std::string& data) {
        rapidjson::Document doc;
        doc.Parse(data.c_str());

        if (!doc.IsObject()) {
            std::cerr << "Invalid JSON format for deserialization." << std::endl;
            return;
        }

        if (doc.HasMember("words")) {
            deserializeWords(doc["words"]);
        }

        if (doc.HasMember("persons")) {
            deserializePersons(doc["persons"]);
        }

        if (doc.HasMember("orgs")) {
            deserializeOrgs(doc["orgs"]);
        }
    }

    // Total size of the words
    int size() const {
        return wordIndex.size();
    }

private:
    // Serializing with null pointers to avoid dereferencing null nodes
    // Going through the tree recursively
    template <typename NodePtr, typename WriterT>  
    void serializeTree(NodePtr node, WriterT& writer) {
        if (node == nullptr) {
            writer.Null();
            return;
        }

        writer.StartObject();
        writer.Key("term");
        writer.String(node->element.term.c_str());
        writer.Key("ids");
        writer.StartArray();

        for (int id : node->element.documentIds) {
            writer.Int(id);
        }

        writer.EndArray();
        writer.Key("left");
        serializeTree(node->left, writer);
        writer.Key("right");
        serializeTree(node->right, writer);
        writer.EndObject();
    }
    
    // Same with deserialization
    void deserializeWords(const rapidjson::Value& node) {
        if (node.IsNull()) {
            return;
        }

        if (!node.HasMember("term")) {
            return;
        }

        std::string term = node["term"].GetString();

        for(auto& id : node["ids"].GetArray()) {
            wordIndex.insert(term, id.GetInt());
        }

        if (node.HasMember("left")) {
            deserializeWords(node["left"]);
        }

        if (node.HasMember("right")) {
            deserializeWords(node["right"]);
        }
    }

    void deserializePersons(const rapidjson::Value& node) {
        if (node.IsNull()) {
            return;
        }

        if (!node.HasMember("term")) {
            return;
        }

        std::string term = node["term"].GetString();

        for(auto& id : node["ids"].GetArray()) {
            personIndex.insert(term, id.GetInt());
        }

        if (node.HasMember("left")) {
            deserializePersons(node["left"]);
        }

        if (node.HasMember("right")) {
            deserializePersons(node["right"]);
        }
    }

    void deserializeOrgs(const rapidjson::Value& node) {
        if (node.IsNull()) {
            return;
        }

        if (!node.HasMember("term")) {
            return;
        }

        std::string term = node["term"].GetString();

        for(auto& id : node["ids"].GetArray()) {
            orgIndex.insert(term, id.GetInt());
        }

        if (node.HasMember("left")) {
            deserializeOrgs(node["left"]);
        }

        if (node.HasMember("right")) {
            deserializeOrgs(node["right"]);
        }
    }
};

#endif
