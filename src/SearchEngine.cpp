#include "../include/AVLOrganizer.h"   
#include "../include/SearchEngine.h"
#include <iostream>
#include <string>
#include <list>
#include <filesystem>
#include "../include/rapidjson/istreamwrapper.h"
#include "../include/rapidjson/document.h"
#include "../include/DocumentParser.h"
#include <sstream>
#include <fstream>
#include <iterator>
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <chrono>
#include <map>
// For reading in API keys
#include <cstdlib>

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

std::string SearchEngine::getKaggleCredentials() {
    // Navigate to home directory on local machine
    const char* homeDir = std::getenv("HOME");
    if (!homeDir) {
        std::cerr << "HOME environment variable not set." << std::endl;
        return "";
    }

    // Path to kaggle.json
    std::string kagglePath = std::string(homeDir) + "/.kaggle/kaggle.json";

    // File stream
    std::ifstream ifs(kagglePath);
    if (!ifs.is_open()) {
        std::cerr << "Could not open kaggle.json at: " << kagglePath << std::endl;
        return "";
    }

    // Parsing JSON file
    rapidjson::IStreamWrapper isw(ifs);
    rapidjson::Document doc;
    doc.ParseStream(isw);

    // Extracting username and API key
    // Then put it in libcurl format to read properly
    if (doc.HasMember("username") && doc["username"].IsString() &&
        doc.HasMember("key") && doc["key"].IsString()) {
        
        std::string username = doc["username"].GetString();
        std::string key = doc["key"].GetString();
        
        // libcurl format
        return username + ":" + key;
    }

    std::cerr << "Invalid kaggle.json format." << std::endl;
    return "";
}

// Concurrent query processing functions
void SearchEngine::createIndexFromKaggle() {
    CURL* curl;
	CURLcode res; 
	std::string readBuffer;

	// Reference: https://curl.se/libcurl/c/libcurl-tutorial.html
	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl = curl_easy_init();

	if (curl) {
		// Kaggle URL from dataset
		curl_easy_setopt(curl, CURLOPT_URL, "https://www.kaggle.com/api/v1/datasets/download/jeet2016/us-financial-news-articles");
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");

        // Get the credentials from the environment variable
        std::string credentials = getKaggleCredentials();
        if (!credentials.empty()) {
            curl_easy_setopt(curl, CURLOPT_USERPWD, credentials.c_str());
        } else {
            std::cerr << "Kaggle credentials not found. Please ensure kaggle.json is in the ~/.kaggle/ directory." << std::endl;
        }

        // libcurl should follow HTTP redirects just in case
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

		// Written to callback function and buffer memory address
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

		// Execution of request over network
		res = curl_easy_perform(curl);

		// Transfer check
		if (res != CURLE_OK) {
			std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
		} else {
			std::cout << "Download Complete! Size: " << readBuffer.size() << " bytes." << std::endl;
            std::cout << "Spawning consumer threads and extracting zip files..." << std::endl;

            // Spawning the consumer threads
            std::vector<std::thread> workers;
            // CPU cores are detected and used to spawn threads
            unsigned int numThreads = std::thread::hardware_concurrency();
            for (unsigned int i = 0; i < numThreads; ++i) {
                // Thread belongs to this class and calls the consumerWorker function
                workers.emplace_back(&SearchEngine::consumerWorker, this);
            }

			// Unzip via producer
			DecompressZipFromMemory(readBuffer);
            
            // Total number of files in queue
            size_t totalFilesToParse = 0;
            {
                std::lock_guard<std::mutex> lock(queueMutex);
                totalFilesToParse = jsonQueue.size();
            }

            // Setting up terminal progress bar for parsing
            // With thread safety to avoid race conditions
            while (true) {
                size_t remainingFiles = 0;

                // Mutex will be locked to read the size
                {
                    std::lock_guard<std::mutex> lock(queueMutex);
                    remainingFiles = jsonQueue.size();
                }

                if (remainingFiles == 0) {
                    break;
                }

                // Calculate progress
                double progress = 1.0 - (static_cast<double>(remainingFiles) / totalFilesToParse);

                // Visual aspects of bar
                int barWidth = 50;
                int filledWidth = static_cast<int>(barWidth * progress);

                // Green color switch and opening of bracket
                // Using the ANSI escape code for green text (terminal needs ANSI support)
                std::cout << "\r\033[32mParsing [";

                // Progression loop
                for (int i = 0; i < barWidth; ++i) {
                    if (i < filledWidth) {
                        // Filled
                        std::cout << "█";
                    } else {
                        // Empty
                        std::cout << " ";
                    }
                }

                // Closing bracket, displaying percent, and resetting to normal color
                std::cout << "] " << static_cast<int>(progress * 100.0) << "%\033[0m" << std::flush;
                
                // 50 ms is enough to prevent excessive CPU usage while still being responsive
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }

            // Final state
            std::cout << "\r\033[32mParsing [";
            for (int i = 0; i < 50; ++i) {
                std::cout << "█";
            }
            std::cout << "] 100%\033[0m" << std::flush;

            // Final message to indicate all files have been processed to user
            std::cout << "\rParsing: 0 files remaining. All files have been processed." << std::endl;

            // Tell consumer threads that no more files are coming
            {
                std::lock_guard<std::mutex> lock(queueMutex);
                extractionComplete = true;
            }
            // Notify all waiting threads
            cv.notify_all();

            // All threads need to finish before continuing
            for (auto& t : workers) {
                if (t.joinable()) {
                    t.join();
                }
            }

            std::cout << "All consumer threads have completed processing." << std::endl;
		}

		// Cleaning up easy resources
		curl_easy_cleanup(curl);
	}

	// Cleaning uo global resources
	curl_global_cleanup();
}

// Main concurrency function
void SearchEngine::consumerWorker() {
    while (true) {
		std::string rawJson;

		// Mutex needs to be locked and unlocked
		std::unique_lock<std::mutex> lock(queueMutex);

		// Lambda statement for wait where if sleeping, mutex is unlocked
		// Otherwise, if woken up, mutex is relocked
		cv.wait(lock, [this] { return !jsonQueue.empty() || extractionComplete; });

		// Exit the queue
		if (jsonQueue.empty() && extractionComplete) {
			return;
		}

		// Data is grabbed and popped from queue
		rawJson = std::move(jsonQueue.front());
		jsonQueue.pop();

        // After leaving queue, unlock mutex
        lock.unlock();

		// Parsing the JSON
		rapidjson::Document d;
		d.Parse(rawJson.c_str());

        // Thread safety ID
        // Using atomic to ensure that each document gets a unique ID even when multiple threads are running
        static std::atomic<int> documentIdCounter{0};
        int currentId = ++documentIdCounter;

		// Pass the popped string to DocumentParser
        DocumentParser parser;
        SearchResult result = parser.parseDocument(d, currentId, organizerIndex, treeMutex);

        // Locking the map mutex to store the result and increment counter
        // Maps are also fast and don't need a queue here
        {
            std::lock_guard<std::mutex> mapLock(mapMutex);
            articleInfo[std::to_string(currentId)] = result;
            ++totalArticles;
        }
	}
}

// Reference: https://gist.github.com/alghanmi/c5d7b761b2c9ab199157#file-curl_example-cpp
size_t SearchEngine::WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t totalSize = size * nmemb;

	// Storing in a string for the data
	std::string* responseString = static_cast<std::string*>(userp);
	responseString->append(static_cast<char*>(contents), totalSize);
	return totalSize;
}

// Reference: https://libzip.org/documentation/
void SearchEngine::DecompressZipFromMemory(const std::string& zipData) {
    zip_error_t error;
	zip_error_init(&error);

	// Libzip source is created
	zip_source_t* src = zip_source_buffer_create(zipData.data(), zipData.size(), 0, &error);

	// Zip is opened
	zip_t* archive = zip_open_from_source(src, ZIP_RDONLY, &error);
    // Error handling to know fail point
    if (!archive) {
        std::cerr << "Failed to open zip archive in memory!" << std::endl;
        // Free the source and finalize the error
        zip_source_free(src);
        zip_error_fini(&error);
        return;
    }

	// Entries in zip
	zip_int64_t numEntries = zip_get_num_entries(archive, 0);
	std::cout << "Total files in ZIP: " << numEntries << std::endl;

	// Extracting each JSON file
	for (zip_int64_t i = 0; i < numEntries; ++i) {
		const char* name = zip_get_name(archive, i, 0);
		if (!name) continue;

		// Open each of the JSON files in the ZIP
		zip_file_t* file = zip_fopen_index(archive, i, 0);
        // Error handling for file opening
        if (!file) continue;

		// Listing the file information for uncompressed size
		zip_stat_t stat;
		if (zip_stat_index(archive, i, 0, &stat) == 0 && (stat.valid & ZIP_STAT_SIZE)) {
			// Decompressed file is read into the buffer
            std::vector<char> buffer(stat.size);
			zip_int64_t bytesRead = zip_fread(file, buffer.data(), stat.size);

			if (bytesRead > 0) {
                // Raw byte buffer is converted into a C++ string
                std::string jsonString(buffer.begin(), buffer.begin() + bytesRead);

                // Locking queue and pushing the extracted string
                {
                    std::lock_guard<std::mutex> lock(queueMutex);
                    jsonQueue.push(std::move(jsonString));
                }
                
                // Notify the consumer thread that new data is available
                cv.notify_one();
            }
		}

		// Closing the zip once done
		zip_fclose(file);

        // Terminal progress bar for extraction
        // Should only update for every 1,000 files or number of entries and calculate the percentage
        if ((i + 1) % 1000 == 0 || (i + 1) == numEntries) {
            // Calculating progress
            double progress = ((i + 1) / static_cast<double>(numEntries));
            
            // Visual aspects of bar
            int barWidth = 50;
            int filledWidth = static_cast<int>(barWidth * progress);

            // Green color
            std::cout << "\r\033[32mExtracting [";

            // Progression loop
            for (int j = 0; j < barWidth; ++j) {
                if (j < filledWidth) {
                    std::cout << "█";
                } else {
                    std::cout << " ";
                }
            }
            std::cout << "] " << static_cast<int>(progress * 100) << "%\033[0m" << std::flush;

            // Next progress bar starts below after finished
            if ((i + 1) == numEntries) {
                std::cout << std::endl;
            }
        }
	}

	// Cleaning up functions in memory for libzip
	zip_close(archive);
	zip_error_fini(&error);
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
        std::cout << "1. Create index from Kaggle API\n";
        std::cout << "2. Save index to file\n";
        std::cout << "3. Load index from file\n";
        std::cout << "4. Perform a search query\n";
        std::cout << "5. Exit\n";
        std::cout << "Enter your choice: ";
        std::getline(std::cin, input);

        if (input == "1") {
            std::cout << "Starting download over network and local memory indexing...\n";
            createIndexFromKaggle();
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
        createIndexFromKaggle();
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