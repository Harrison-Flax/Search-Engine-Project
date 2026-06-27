#include "DocumentParser.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <iomanip>
#include <unordered_set>
#include <algorithm>
#include <cctype>

// RapidJSON headers we need for our parsing.
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/document.h"

// Oleander Stemming Library
#include "oleander/stemming.h"
#include "oleander/english_stem.h"

DocumentParser::DocumentParser() {}

DocumentParser::~DocumentParser() {}

// Follows logic from rapidJSONExample.cpp
SearchResult DocumentParser::parseDocument(const std::string& filePath, int documentId, AVLOrganizer<std::string>* organizerIndex) {
    // Open an ifstream on the file and check if it could be opened
    std::ifstream input(filePath);
    if (!input.is_open()) {
        std::cerr << "Cannot open file: " << filePath << std::endl;
        return SearchResult();
    }

    // Create a RapidJSON IStreamWrapper using the file input stream above
    rapidjson::IStreamWrapper isw(input);

    // Create a RapidJSON Document object and use it to parse the IStreamWrapper object
    rapidjson::Document d;
    d.ParseStream(isw);

    // Extracting the fields
    // All from SearchResult (Words, Persons, and Orgs)
    auto title = d["title"].GetString();
    auto uuid = d["uuid"].GetString();
    auto publication = d["thread"]["site"].GetString();
    auto date = d["published"].GetString();
    // Need to split the text into tokens
    auto text = d["text"].GetString();
    auto persons = d["entities"]["persons"].GetArray();
    auto orgs = d["entities"]["organizations"].GetArray();
    auto score = d["score"].GetDouble();

    // Create a SearchResult object with the extracted fields (only those from the .h)
    SearchResult result(uuid, title, publication, date, score, text);

    // Person and body need to be inserted into organizerIndex
    // Persons loop
    for (const auto& person : d["entities"]["persons"].GetArray()) {
        std::string personName = person["name"].GetString();
        std::string sentiment = person["sentiment"].GetString();

        // Insert into organizerIndex
        organizerIndex->insert(personName, documentId, "person");
    }

    // Tokens loop for body text
    // Need to do it via istringstream since there is no pre-tokenized array
    std::istringstream textStream(text);
    std::string token;

    while (textStream >> token) {
        // Need to clean the tokens of punctuation and make them lowercase
        token.erase(std::remove_if(token.begin(), token.end(), ::ispunct), token.end());
        std::transform(token.begin(), token.end(), token.begin(), ::tolower);

        // Inlcuding a check for empty quotes
        if (token.empty()) {
            continue;
        }

        // Check if the token is a stop word
        if (!isStopWord(token)) {
            // Stem the token
            std::string stemmedToken = stemWord(token);
            // Insert the stemmed token into organizerIndex
            organizerIndex->insert(stemmedToken, documentId, "word");
        }
    }

    // Need an org loop as well since it's in my UML diagram
    for (const auto& org : d["entities"]["organizations"].GetArray()) {
        std::string orgName = org["name"].GetString();
        std::string sentiment = org["sentiment"].GetString();

        // Insert into organizerIndex
        organizerIndex->insert(orgName, documentId, "org");
    }

    // Close the input file
    input.close();

    return result;
}

bool DocumentParser::isStopWord(const std::string& word) {
    // Need an unordered set to store the stopwords
    // Should be static so that it persists across function calls
    static std::unordered_set<std::string> stopWords;

    if (stopWords.empty()) {
        // Open the .csv file since it's better than hardcoding
        std::ifstream stopWordsFile("assets/stopwords/stopwords.csv");
        
        // Check if the file opened
        if (!stopWordsFile.is_open()) {
            std::cerr << "Cannot open stopwords file." << std::endl;
            return false;
        }

        // Populating the unordered set with stopwords from the .csv file
        // Reading in stopwords from a .txt file in assets
        // Reference: https://gist.github.com/sebleier/554280
        std::string current_row;
    
        // Reading in the .csv file row by row
        while(std::getline(stopWordsFile, current_row)) {
            std::stringstream ss(current_row);
            std::string cell;
            std::vector<std::string> row;

            // Split the row by commas
            while(std::getline(ss, cell, ',')) {
                row.push_back(cell);
            }

            // Remove the quotes and whitespace from the cells
            for (auto& word : row) {
                word.erase(std::remove(word.begin(), word.end(), '\"'), word.end());
                word.erase(std::remove_if(word.begin(), word.end(), ::isspace), word.end());
            }

            // Add the cells to the unordered set
            for (const auto& word : row) {
                stopWords.insert(word);
            }
        }
    }
    
    // Return the words that exist in the set
    return stopWords.find(word) != stopWords.end();
}

std::string DocumentParser::stemWord(const std::string& word) {
    // Using the Oleander Stemming Library for simplicty
    // Reference: https://github.com/Blake-Madden/OleanderStemmingLibrary
    stemming::english_stem<> stemmer;

    // Need this for the stemmer to work with ASCII strings
    std::wstring wideWord(word.begin(), word.end());
    stemmer(wideWord);

    // Needs to be returned as a string though
    return std::string(wideWord.begin(), wideWord.end());
}