//#pragma once
#ifndef _SEARCH_ENGINE_H_
#define _SEARCH_ENGINE_H_

#include "AVLOrganizer.h"  

class SearchEngine {
private:
    AVLOrganizer<std::string>* organizerIndex;

public:
    SearchEngine();
    ~SearchEngine();

    void createIndex(const std::string& directory);
    void saveIndexToFile(const std::string& fileName);
    void loadIndexFromFile(const std::string& fileName);
    std::list<std::string> performQuery(const std::string& query);

    void ui();
    void go(int argc, char** argv);
};

#endif