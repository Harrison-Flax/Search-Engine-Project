#include "../../include/tui/SearchPanel.h"
#include <ftxui/dom/elements.hpp>

// Reference: https://github.com/ArthurSonzogni/FTXUI/blob/main/examples/component/input.cpp

ftxui::Component MakeSearchPanel(UIState& state, std::function<void(const std::string&)> onSearch) {
    // Handling input
    ftxui::InputOption opt = ftxui::InputOption::Default();
    opt.content = &state.query;
    opt.placeholder = "Enter search query...";
    opt.on_enter = [&state, onSearch] { onSearch(state.query); };
    ftxui::Component input = Input(opt);

    // Making a search button
    ftxui::Component button = ftxui::Button("Search", [&state, onSearch] {
        onSearch(state.query);
    });

    // Handling navigation
    ftxui::Component layout = ftxui::Container::Horizontal({input, button});

    // Render the panel in the TUI
    return Renderer(layout, [input, button] {
        return ftxui::hbox({
            ftxui::text("Search: "),
            input->Render() | ftxui::flex,
            ftxui::text(" "),
            button->Render(),
        }) | ftxui::border;
    });
}