#ifndef SEARCH_RESULT_H
#define SEARCH_RESULT_H

#include <string>
#include <iostream>

// Using a struct to represent a search result
// It should be its own data structure since it represents a series of attributes
struct SearchResult {
    std::string uuid;
    std::string title;
    std::string publication;
    std::string date;
    double score;
    std::string text;

    // Default constructor
    SearchResult() : score(0.0) {}

    // Full constructor
    SearchResult(const std::string& uuid,
                 const std::string& title,
                 const std::string& publication,
                 const std::string& date,
                 double score,
                 const std::string& text)
                 : uuid(uuid), title(title), publication(publication),
                   date(date), score(score), text(text) {}
    
    // Overloading the output stream operator for easy printing of SearchResult objects
    friend std::ostream& operator<<(std::ostream& os, const SearchResult& result) {
        os << "Title: " << result.title << "\n"
           << "Publication: " << result.publication << "\n"
           << "Date: " << result.date << "\n"
           << "Score: " << result.score << "\n"
           << "UUID: " << result.uuid << "\n";
        return os;
    }

    // Operator overloads
    bool operator<(const SearchResult& other) const {
        return this->uuid < other.uuid; 
    }

    bool operator==(const SearchResult& other) const {
        return this->uuid == other.uuid; 
    }

    // Getters for the attributes
    std::string getUuid() const { return uuid; }
    std::string getTitle() const { return title; }
    std::string getPublication() const { return publication; }
    std::string getDate() const { return date; }
    double getScore() const { return score; }
    std::string getText() const { return text; }

};

#endif