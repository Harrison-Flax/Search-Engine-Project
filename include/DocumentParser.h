#ifndef DOCUMENT_PARSER_H
#define DOCUMENT_PARSER_H

#include "AVLOrganizer.h"
#include "SearchResult.h"
#include <string>
#include <mutex>

class DocumentParser {
public:
    DocumentParser();
    ~DocumentParser();

    // Parsing
    SearchResult parseDocument(const std::string& filePath, int documentId, AVLOrganizer<std::string>* organizerIndex);
    // Overloaded parseDocument for rapidjson::Document
    SearchResult parseDocument(const rapidjson::Document& doc, int documentId, AVLOrganizer<std::string>* organizerIndex, std::mutex& treeMutex);

    // Check for stop words
    bool isStopWord(const std::string& word);

    // Stemmed words
    std::string stemWord(const std::string& word);
};

#endif