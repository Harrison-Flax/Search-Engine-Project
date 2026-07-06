#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "SearchEngine.h"
#include "AVLOrganizer.h"
#include "doctest.h"

DOCTEST_TEST_CASE("SearchEngine Index Tests") {
    SearchEngine searchEngine;

    SUBCASE("Create Index from Directory") {
        std::string testDirectory = "build/sample_data/coll_2";

        // Test creating an index from a directory
        REQUIRE_NOTHROW(searchEngine.createIndex(testDirectory));
    }

    SUBCASE("Save Index To File") {
        std::string testDirectory = "build/sample_data/coll_2"; 
        std::string indexPath = "assets/temp_direct/index_file.txt"; 

        // Create index first (assumption is that createIndex has been tested)
        REQUIRE_NOTHROW(searchEngine.createIndex(testDirectory));

        // Test saving the index to a file
        REQUIRE_NOTHROW(searchEngine.saveIndexToFile(indexPath));
    }

    SUBCASE("Load Index From File") {
        std::string indexPath = "assets/temp_direct/index_file.txt";

        // Test loading the index from a file
        REQUIRE_NOTHROW(searchEngine.loadIndexFromFile(indexPath));

        // Validate by performing a query after loading
        auto queryResult = searchEngine.performQuery("sample_query");
        REQUIRE_FALSE(queryResult.empty());

        // Validate that the loaded index contains expected data
        auto searchResult = searchEngine.searchToken("sample_query", "word", false);
        REQUIRE_FALSE(searchResult.empty());

        // Delete the temporary index file after testing
        std::remove(indexPath.c_str());

    }

    SUBCASE("Perform Query") {
        std::string testDirectory = "build/sample_data/coll_2";         

        // Assuming the index is already created (assumption is that createIndex has been tested)
        REQUIRE_NOTHROW(searchEngine.createIndex(testDirectory));

        // Creating SearchResult objects to simulate expected results
        SearchResult result1("uuid1", "Title 1", "Publication 1", "2023-01-01", 0.9, "Sample text 1");
        SearchResult result2("uuid2", "Title 2", "Publication 2", "2023-02-01", 0.8, "Sample text 2");

        // Add to articleInfo map
        searchEngine.getArticleInfo().insert({"1", result1});
        searchEngine.getArticleInfo().insert({"2", result2});

        // Make sure the map isn't empty
        REQUIRE_FALSE(searchEngine.getArticleInfo().empty());

        // Add IDs to AVLTree for the tokens
        searchEngine.getOrganizerIndex()->insert("sample_query", 1);
        searchEngine.getOrganizerIndex()->insert("sample_query", 2);

        // Test performing a query
        auto queryResult = searchEngine.performQuery("sample_query");
        REQUIRE_FALSE(queryResult.empty());
    }
}