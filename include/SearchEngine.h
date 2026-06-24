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
    void compareResults(const std::vector<SearchResult>& results, const std::vector<SearchResult>& otherResults);
    void searchToken(const std::vector<std::string>& names, const std::string& token, const bool excluded);

    // Getters
    std::unordered_map<std::string, SearchResult> getArticleInfo() const;
    AVLOrganizer<std::string>* getOrganizerIndex() const;
    int getTotalArticles() const;

    // CLI UI
    void ui();
    void go(int argc, char** argv);
};

#endif