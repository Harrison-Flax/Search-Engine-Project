#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "SearchEngine.h"
#include "AVLOrganizer.h"
#include "doctest.h"

DOCTEST_TEST_CASE("SearchEngine Index Tests") {
    SearchEngine searchEngine;

    SUBCASE("Create Index from Directory") {
        std::string testDirectory = "sample_data/coll_2";

        // Test creating an index from a directory
        REQUIRE_NOTHROW(searchEngine.createIndex(testDirectory));
    }

    SUBCASE("Save Index To File") {
        std::string testDirectory = "sample_data/coll_2"; 
        std::string indexPath = "temp_direct/index_file.txt"; 

        // Create index first (assumption is that createIndex has been tested)
        REQUIRE_NOTHROW(searchEngine.createIndex(testDirectory));

        // Test saving the index to a file
        REQUIRE_NOTHROW(searchEngine.saveIndexToFile(indexPath));
    }

    SUBCASE("Load Index From File") {
        std::string indexPath = "temp_direct/index_file.txt";

        // Test loading the index from a file
        REQUIRE_NOTHROW(searchEngine.loadIndexFromFile(indexPath));

        // Validate by performing a query after loading
        auto queryResult = searchEngine.performQuery("sample_query");
        REQUIRE_FALSE(queryResult.empty());
    }

    SUBCASE("Perform Query") {
        std::string testDirectory = "sample_data/coll_2"; 

        // Assuming the index is already created (assumption is that createIndex has been tested)
        REQUIRE_NOTHROW(searchEngine.createIndex(testDirectory));

        // Test performing a query
        auto queryResult = searchEngine.performQuery("sample_query");
        REQUIRE_FALSE(queryResult.empty());
    }
}