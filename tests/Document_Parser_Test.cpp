#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "DocumentParser.h"
#include "AVLOrganizer.h"
#include "doctest.h"

// 1) parseDocument for a single document
DOCTEST_TEST_CASE("DocumentParser Create Index Test") {
    DocumentParser parser;
    AVLOrganizer<std::string> organizerIndex;

    // Test parsing a single document
    std::string testFilePath = "/build/sample_data/coll_2/news_0064570.json";
    // ID is arbitrary for testing purposes
    int documentId = 1;

    SearchResult result = parser.parseDocument(testFilePath, documentId, &organizerIndex);

    // Validating the result
    REQUIRE(result.getTitle() == "Eaton Vance Closed-End Funds Release Estimated Sources Of Distributions");
    REQUIRE(result.getUuid() == "4ad75456ae0bc42beeee2ff65fa58fa1cc503be2"); 
    REQUIRE(result.getPublication() == "CNBC");
    REQUIRE(result.getDate() == "2018-02-28");
}

// 2) isStopWord for known stop words
DOCTEST_TEST_CASE("DocumentParser Stop Words Test") {
    DocumentParser parser;

    // Test known stop words
    REQUIRE(parser.isStopWord("the") == true);
    REQUIRE(parser.isStopWord("and") == true);
    REQUIRE(parser.isStopWord("is") == true);
    // Test non-stop words
    REQUIRE(parser.isStopWord("example") == false);
    REQUIRE(parser.isStopWord("search") == false);
}

// 3) stemWord for known words
DOCTEST_TEST_CASE("DocumentParser Stem Words Test") {
    DocumentParser parser;

    // Test known words and their expected stems
    REQUIRE(parser.stemWord("running") == "run");
    REQUIRE(parser.stemWord("jumps") == "jump");
    REQUIRE(parser.stemWord("easily") == "easili");
    REQUIRE(parser.stemWord("fairly") == "fair");
}