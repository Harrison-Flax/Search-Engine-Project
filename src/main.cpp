#include "SearchEngine.h"
#include <iostream>

int main(int argc, char **argv) {
    SearchEngine searchEngine;

    // Display usage information if no arguments are provided
    if (argc < 2) {
        std::cout << "Usage:\n"
                  << "\tIndex all files in <directory> and store the index in one or several files:\n"
                  << "\tsupersearch index <directory>\n\n"
                  << "\tLoad the existing index and perform the following query:\n"
                  << "\tsupersearch query \"social network PERSON:cramer\"\n\n";

        return 1;  // Exit with a non-zero status to indicate no operation was performed
    }

    // Delegate the command processing to the SearchEngine instance
    searchEngine.go(argc, argv);

    return 0;
}