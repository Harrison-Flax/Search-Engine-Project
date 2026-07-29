//#pragma once
#ifndef _SEARCH_ENGINE_H_
#define _SEARCH_ENGINE_H_

#include "AVLOrganizer.h"
#include "SearchResult.h"  
#include <unordered_map>
#include <vector>
#include <string>
#include <list>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <zip.h>
#include <atomic>
#include <curl/curl.h>

class SearchEngine {
private:
    // Data members
    AVLOrganizer<std::string>* organizerIndex;
    std::unordered_map<std::string, SearchResult> articleInfo;
    int totalArticles = 0;
    int filesProcessed = 0;

    // For Concurrent Query Processing (sourced from my repo, file_runner)
    std::queue<std::string> jsonQueue;
    std::mutex queueMutex;
    std::mutex mapMutex;
    std::condition_variable cv;
    bool extractionComplete = false;
    std::mutex treeMutex;

    // Reading in API keys from environment variables
    std::string getKaggleCredentials();

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

    // Concurrent Query Processing
    void createIndexFromKaggle();
    void consumerWorker();
    // Needs to be static because it is used as a callback function for libcurl, which requires a C-style function pointer.
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);
    size_t DecompressZipFromMemory(const std::string& zipData);

    // Getters
    std::unordered_map<std::string, SearchResult> getArticleInfo() const;
    AVLOrganizer<std::string>* getOrganizerIndex() const;
    int getTotalArticles() const;

    // CLI UI
    void ui();
    void go(int argc, char** argv);
};

#endif