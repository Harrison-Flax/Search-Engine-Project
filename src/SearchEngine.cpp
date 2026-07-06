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

    // Vector to store the final results                                      
    std::vector<SearchResult> finalResults;
    // Used to determine if the next operation is an AND or OR
    bool performAND = false; 

    // Process the tokens and construct the query
    for (const auto& token : tokens) {

        // Detect person and org prefixes
        // Need two variables to hold if excluded and the type of prefix
        bool isExcluded = false;
        std::string prefixType = "NONE";
        // Mutable copy of token
        std::string cleanToken = token;

        // Logic operators
        if (cleanToken == "AND") {
            performAND = true;
            continue;
        } else if (cleanToken == "OR") {
            performAND = false;
            continue;
        }

        // Exclusion
        if (cleanToken.front() == '-') {
            isExcluded = true;
            // Removes first character
            cleanToken = cleanToken.substr(1);
        }

        // Prefixes
        if (cleanToken.find("PERSON:") == 0) {
            prefixType = "PERSON";
            // Removes PERSON:
            cleanToken = cleanToken.substr(7);
        } else if (cleanToken.find("ORG:") == 0) {
            prefixType = "ORG";
            // Removes ORG:
            cleanToken = cleanToken.substr(4);
        }

        // Handling stop words
        // Call on functions from DocumentParser
        DocumentParser parser;

        if (parser.isStopWord(cleanToken)) {
            std::cout << "Skipping stop word: " << cleanToken << std::endl;
            continue;
        } else {
            // Pass through Oleander stemmer
            cleanToken = parser.stemWord(cleanToken);
        }

        // Call searchToken for person and org tokens instead of the regular search
        std::vector<SearchResult> tokenResults = this->searchToken(cleanToken, prefixType, isExcluded);
        
        // Merge the results into a final list
        if (finalResults.empty()) {
            // tokenResults is the base case for the first token
            finalResults = tokenResults;
        } else {
            std::string operationType = performAND ? "AND" : "OR";
            finalResults = compareResults(finalResults, tokenResults, operationType);
        }
    }

    std::cout << "Query performed successfully." << std::endl;

    // The vector becomes a list for the method header to match
    return std::list<SearchResult>(finalResults.begin(), finalResults.end());
}

// Need the rest of the query functions
std::vector<SearchResult> SearchEngine::compareResults(std::vector<SearchResult> listA, std::vector<SearchResult> listB, const std::string& operationType) {
    // Handles AND intersection and OR union

    // Need to sort the lists by lowest to highest ID
    std::sort(listA.begin(), listA.end());
    std::sort(listB.begin(), listB.end());

    // Holding the merged results
    std::vector<SearchResult> mergedResults;

    // Merging the two pointers so there are no lost exclusions or scores
    auto iteratorA = listA.begin();
    auto iteratorB = listB.begin();

    while (iteratorA != listA.end() && iteratorB != listB.end()) {
        if (*iteratorA < *iteratorB) {
            if (operationType == "OR") {
                mergedResults.push_back(*iteratorA);
            }
            // A is moved forward
            ++iteratorA;
        } else if (*iteratorB < *iteratorA) {
            if (operationType == "OR") {
                mergedResults.push_back(*iteratorB);
            }
            // B is moved forward
            ++iteratorB;
        } else {
            // Both are equal, so we can merge them
            // A copy of A is created
            SearchResult mergedResult = *iteratorA; 

            // Add exclusion check
            if (iteratorA->score < 0 || iteratorB->score < 0) {
                mergedResult.score = -1;
            } else {
                // Sum the scores if both are not excluded
                mergedResult.score += iteratorB->score;
            }
            
            mergedResults.push_back(mergedResult);
            ++iteratorA;
            ++iteratorB;
        }
    }

    // If on OR operation, need to catch any remaining elements in either list
    if (operationType == "OR") {
        while (iteratorA != listA.end()) {
            mergedResults.push_back(*iteratorA);
            ++iteratorA;
        }
        while (iteratorB != listB.end()) {
            mergedResults.push_back(*iteratorB);
            ++iteratorB;
        }
    }

    // Handling exclusions
    // Using a lambda function to do this in a single line
    mergedResults.erase(std::remove_if(mergedResults.begin(), mergedResults.end(),
        [](const SearchResult& result) {
            // Negative scores are the exclusion flag
            return result.score < 0; 
        }), mergedResults.end());

    // Sorting the list based on score 
    std::sort(mergedResults.begin(), mergedResults.end(),
              [](const SearchResult& a, const SearchResult& b) {
                  // Sort in descending order
                  return a.score > b.score; 
              });
    
    return mergedResults;
}

std::vector<SearchResult> SearchEngine::searchToken(const std::string& token, const std::string& prefixType, const bool isExcluded) {
    // Performs the search and handle exclusions as needed
    // Tokens should looked up in organizerIndex
    // Checks for excluded flags and returns matching IDs
    
    // Storing the ids as matches from tree as a list
    std::list<int> rawMatches = organizerIndex->search(token);

    // Creating a vector to hold the data to return
    std::vector<SearchResult> finalMatches;

    // Iterating through the IDs
    for (int id : rawMatches) {
        // ID needs to be a string like before
        std::string idStr = std::to_string(id);

        // Check if the ID exists in articleInfo
        if (articleInfo.find(idStr) == articleInfo.end()) {
            continue;
        }

        SearchResult currentResult = articleInfo[idStr];

        // Prefixes
        if (prefixType == "PERSON" && currentResult.title.find("PERSON:") == std::string::npos) {
            continue; 
        } else if (prefixType == "ORG" && currentResult.title.find("ORG:") == std::string::npos) {
            continue; 
        }

        // Handle exclusions
        if (isExcluded) {
            currentResult.score = -1;
        }

        // Pushed into vector
        finalMatches.push_back(currentResult);
    }

    return finalMatches;
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