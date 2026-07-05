//#pragma once
#ifndef _SEARCH_ENGINE_H_
#define _SEARCH_ENGINE_H_

#include "AVLOrganizer.h"
#include "SearchResult.h"  
#include <unordered_map>
#include <vector>
#include <string>
#include <list>

class SearchEngine {
private:
    // Data members
    AVLOrganizer<std::string>* organizerIndex;
    std::unordered_map<std::string, SearchResult> articleInfo;
    int totalArticles;

public:
    // Constructor and Destructor
    SearchEngine();
    ~SearchEngine();

    // Indexing and File Operations
    void createIndex(const std::string& directory);
    void saveIndexToFile(const std::string& fileName);
    void loadIndexFromFile(const std::string& fileName);
    
    // Query Operations
    std::list<SearchResult> performQuery(const std::string& query);
    std::vector<SearchResult> compareResults(std::vector<SearchResult> listA, std::vector<SearchResult> listB, const std::string& operationType);
    std::vector<SearchResult> searchToken(const std::string& token, const std::string& prefixType, const bool isExcluded);

    // Getters
    std::unordered_map<std::string, SearchResult> getArticleInfo() const;
    AVLOrganizer<std::string>* getOrganizerIndex() const;
    int getTotalArticles() const;

    // CLI UI
    void ui();
    void go(int argc, char** argv);
};

#endif