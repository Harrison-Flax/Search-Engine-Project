#include "AVLOrganizer.h"   
#include "SearchEngine.h"
#include <iostream>
#include <string>
#include <list>
#include <filesystem>
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/document.h"
#include <sstream>
#include <fstream>
#include <iterator>
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <chrono>

using namespace rapidjson;

SearchEngine::SearchEngine() {
    organizerIndex = new AVLOrganizer<std::string>(); //creates a new AVL tree
}

SearchEngine::~SearchEngine() {
    delete organizerIndex; //deletes the AVL tree
}

void SearchEngine::createIndex(const std::string& directory) {
    namespace fs = std::filesystem; 
    fs::path pathToIndex(directory); //allows us to iterate through the directory

    // Iterate over the files in the specified directory
    for (const auto& entry : fs::recursive_directory_iterator(directory)) {
        // Rregular file or has a .json extension?
        if (entry.is_regular_file() && entry.path().extension().string() == ".json") {
            // Open an ifstream on the file and check if it could be opened
            std::ifstream input(entry.path().string());
            if (!input.is_open()) {
                std::cerr << "Cannot open file: " << entry.path().string() << std::endl;
                continue;  // Move to the next file
            }

            // Create a RapidJSON IStreamWrapper using the file input stream above
            IStreamWrapper isw(input);

            // Create a RapidJSON Document object and use it to parse the IStreamWrapper object
            Document d;
            d.ParseStream(isw);

            // Allows for extraction of information from the JSON document and population of AVL tree
            
            // Close the input file
            input.close();
        }
    }

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

std::list<std::string> SearchEngine::performQuery(const std::string& query) {
    // Tokenize the query string
    std::istringstream iss(query);
    std::vector<std::string> tokens = {std::istream_iterator<std::string>{iss},
                                        std::istream_iterator<std::string>{}};

    std::list<std::string> results;
    std::unordered_set<std::string> resultSet; // Used to store the results of the query

    bool performAND = false; // Used to determine if the next operation is an AND or OR

    // Process the tokens and construct the query
    for (const auto& token : tokens) {
        // Assuming the query has simple AND and OR operators
        if (token == "AND") {
            // Perform AND operation
            performAND = true;
        } else if (token == "OR") {
            // Perform OR operation
            performAND = false;
        } else {
            // Perform a basic search for the token
            std::list<std::string> tokenResults = this->performQuery(token);

            if (performAND) {
                // If previous operation was an AND, perform intersection
                std::unordered_set<std::string> tokenSet(tokenResults.begin(), tokenResults.end());
                std::list<std::string> intersection;
                std::set_intersection(resultSet.begin(), resultSet.end(),
                                      tokenSet.begin(), tokenSet.end(),
                                      std::back_inserter(intersection));
                resultSet = std::unordered_set<std::string>(intersection.begin(), intersection.end());
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
        std::cerr << "Usage: [program name] ui | create_index [directory] | save_index [file] | load_index [file] | query [search terms]\n";
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
        std::cerr << "Usage: [program name] ui | create_index [directory] | save_index [file] | load_index [file] | query [search terms]\n";
    }
}