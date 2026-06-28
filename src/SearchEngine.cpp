#include "AVLOrganizer.h"   
#include "SearchEngine.h"
#include <iostream>
#include <string>
#include <list>
#include <filesystem>
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/document.h"
#include "DocumentParser.h"
#include <sstream>
#include <fstream>
#include <iterator>
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <chrono>
#include <map>

using namespace rapidjson;

SearchEngine::SearchEngine() {
    organizerIndex = new AVLOrganizer<std::string>();
}

SearchEngine::~SearchEngine() {
    delete organizerIndex; 
}

// Need to tie in DocumentParser here
void SearchEngine::createIndex(const std::string& directory) {
    namespace fs = std::filesystem; 
    // Iterates through the whole directory
    fs::path pathToIndex(directory);

    // DocumentParser instance
    // Every document needs to be parsed with the path, document ID, and organizerIndex
    DocumentParser parser;

    // Iterate over the files in the specified directory
    // Recursive directory iterator to go through all subdirectories as well
    for (const auto& entry : fs::recursive_directory_iterator(directory)) {
        
        // Rregular file or has a .json extension?
        if (entry.is_regular_file() && entry.path().extension().string() == ".json") {
            // Open an ifstream on the file and check if it could be opened
            std::ifstream input(entry.path().string());
            
            if (!input.is_open()) {
                std::cerr << "Cannot open file: " << entry.path().string() << std::endl;
                // Move to the next file
                continue;
            }

            // DocumentId should be incremented (a counter)
            // Making it static so that it persists across function calls
            static int documentId = 0; 
            documentId++; 
            
            // Store SearchResult into articleInfo
            SearchResult result = parser.parseDocument(entry.path().string(), documentId, organizerIndex);

            // Increment totalArticles
            totalArticles++;

            // Store the SearchResult in the articleInfo map
            articleInfo[std::to_string(documentId)] = result;

            // Close the input file
            input.close();
        }
    }

    // Printing a message so the user knows
    std::cout << "Index created successfully." << std::endl;
}

void SearchEngine::saveIndexToFile(const std::string& fileName) {
    // Open an ofstream to write the index to the specified file
    std::ofstream outputFile(fileName);

    // Check if the file is opened successfully
    if (!outputFile.is_open()) {
        std::cerr << "Cannot open file for writing: " << fileName << std::endl;
        return;
    }

    // Traverse the AVL tree and serialize the data to the file
    std::string serializedIndex = organizerIndex->serialize();
    
    // Write the serialized data to the file
    outputFile << serializedIndex;

    // Close the file
    outputFile.close();

    std::cout << "Index saved to file: " << fileName << std::endl;
}

void SearchEngine::loadIndexFromFile(const std::string& fileName) {
    // Open an ifstream to read the index from the specified file
    std::ifstream inputFile(fileName);

    // Check if the file is opened successfully
    if (!inputFile.is_open()) {
        std::cerr << "Cannot open file for reading: " << fileName << std::endl;
        return;
    }

    // Read the content of the file into a string
    std::string fileContent((std::istreambuf_iterator<char>(inputFile)),
                            std::istreambuf_iterator<char>());

    // Close the file
    inputFile.close();

    // Use AVLOrganizer's method to load the index from the string
    organizerIndex->deserialize(fileContent);

    std::cout << "Index loaded from file: " << fileName << std::endl;
}

// Handles search query
std::list<SearchResult> SearchEngine::performQuery(const std::string& query) {
    // Tokenize the query string
    std::istringstream iss(query);
    std::vector<std::string> tokens = {std::istream_iterator<std::string>{iss},
                                        std::istream_iterator<std::string>{}};

    // List to store the final results                                      
    std::list<SearchResult> results;

    // Used to store the results of the query
    // Using unordered set since its fast and no need for order
    std::unordered_set<std::string> resultSet; 

    // For performing intersection of results when AND is used
    std::list<SearchResult> intersection;

    // Used to determine if the next operation is an AND or OR
    bool performAND = false; 

    // Process the tokens and construct the query
    for (const auto& token : tokens) {

        // FIX: Detect person and org prefixes


        // FIX: Call searchToken for person and org tokens

        
        // Check if AND/OR and no infinite loop
        if (token == "AND" && performAND) {
            std::cerr << "Error: Consecutive AND operators are not allowed." << std::endl;
            return {};
        } else if (token == "OR" && !performAND) {
            std::cerr << "Error: Consecutive OR operators are not allowed." << std::endl;
            return {};
        }

        // Determine the operation based on the token
        if (token == "AND") {
            performAND = true;
        } else if (token == "OR") {
            performAND = false;
        } else {
            // Perform a basic search for the token
            std::list<SearchResult> tokenResults = this->performQuery(token);

            if (performAND) {
                // If previous operation was an AND, perform intersection
                // Using a multimap to store the results with their scores for sorting and greater efficiency
                std::multimap<int, SearchResult> tokenSet(tokenResults.begin(), tokenResults.end());
                std::set_intersection(resultSet.begin(), resultSet.end(),
                                      tokenSet.begin(), tokenSet.end(),
                                      std::back_inserter(intersection));

                // Need to call compareResults to sort and filter the intersection results based on scores
                compareResults(std::vector<SearchResult>(intersection.begin(), intersection.end()),
                               std::vector<SearchResult>(tokenResults.begin(), tokenResults.end()));
            } else {
                // If previous operation was an OR, perform union
                resultSet.insert(tokenResults.begin(), tokenResults.end());
            }

            // Incorporate the results into the overall results
            results.insert(results.end(), tokenResults.begin(), tokenResults.end());
        }
    }

    // Remove duplicate results (if needed)
    results.sort();
    results.unique();

    // Copy the results from the set to the list
    results.assign(resultSet.begin(), resultSet.end());
    std::cout << "Query performed successfully." << std::endl;

    return results;
}

// Need the rest of the query functions
void SearchEngine::compareResults(const std::vector<SearchResult>& results, const std::vector<SearchResult>& otherResults) {
    // Handles AND intersection and OR union
    
}

void SearchEngine::searchToken(const std::vector<std::string>& names, const std::string& token, const bool excluded) {
    // Performs the search and handle exclusions as needed
    // Tokens should looked up in organizerIndex
    // Checks for exlcuded flags and returns matching IDs
    organizerIndex->search(token);

}

// Easy getters for member variables
std::unordered_map<std::string, SearchResult> SearchEngine::getArticleInfo() const {
    return articleInfo;
}

AVLOrganizer<std::string>* SearchEngine::getOrganizerIndex() const {
    return organizerIndex;
}

int SearchEngine::getTotalArticles() const {
    return totalArticles;
}

// UI Function for the search engine
void SearchEngine::ui() {
    std::string input;
    while (true) {
        std::cout << "\n--- Search Engine UI ---\n";
        std::cout << "1. Create index from directory\n";
        std::cout << "2. Save index to file\n";
        std::cout << "3. Load index from file\n";
        std::cout << "4. Perform a search query\n";
        std::cout << "5. Exit\n";
        std::cout << "Enter your choice: ";
        std::getline(std::cin, input);

        if (input == "1") {
            std::cout << "Enter directory path: ";
            std::getline(std::cin, input);
            createIndex(input);
        } else if (input == "2") {
            std::cout << "Enter filename to save index: ";
            std::getline(std::cin, input);
            saveIndexToFile(input);
        } else if (input == "3") {
            std::cout << "Enter filename to load index: ";
            std::getline(std::cin, input);
            loadIndexFromFile(input);
        } else if (input == "4") {
            std::cout << "Enter search query: ";
            std::getline(std::cin, input);

            auto start = std::chrono::high_resolution_clock::now();
            auto results = performQuery(input);
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> duration = end - start;

            // Display the results and timing
            std::cout << "Search results (" << results.size() << " articles, " 
                      << duration.count() << " seconds):\n";
            for (const auto& result : results) {
                std::cout << result << std::endl;
            }
        } else if (input == "5") {
            break;
        } else {
            std::cout << "Invalid option. Please try again.\n";
        }
    }
}

// Function to execute the UI or other command-line arguments
void SearchEngine::go(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: [program name] ui | create_index [directory] | save_index [file] | load_index [file]" 
                     "| query [search terms]\n";
        return;
    }

    std::string command = argv[1];

    if (command == "ui") {
        ui();
    } else if (command == "create_index" && argc == 3) {
        createIndex(argv[2]);
    } else if (command == "save_index" && argc == 3) {
        saveIndexToFile(argv[2]);
    } else if (command == "load_index" && argc == 3) {
        loadIndexFromFile(argv[2]);
    } else if (command == "query" && argc >= 3) {
        std::string query;
        for (int i = 2; i < argc; ++i) {
            query += argv[i];
            if (i < argc - 1) query += " ";  // Add space between words
        }
        auto results = performQuery(query);
        for (const auto& result : results) {
            std::cout << result << std::endl;
        }
    } else {
        std::cerr << "Invalid command or insufficient arguments\n";
        std::cerr << "Usage: [program name] ui | create_index [directory] | save_index [file] | load_index [file]" 
                     "| query [search terms]\n";
    }
}