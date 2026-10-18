#ifndef UI_STATE_H
#define UI_STATE_H

#include <string>
#include <vector>
#include "include/SearchResult.h"

struct UIState {
    std::string query;
    std::vector<SearchResult> results;
    std::string status = "Ready";
};

#endif