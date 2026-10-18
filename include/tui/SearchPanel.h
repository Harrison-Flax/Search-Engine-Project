#ifndef SEARCH_PANEL_H
#define SEARCH_PANEL_H

#include <functional>
#include <string>
#include <ftxui/component/component.hpp>
#include "UIState.h"

// For building the search box and button
ftxui::Component MakeSearchPanel(UIState& state, std::function<void(const std::string&)> onSearch);

#endif