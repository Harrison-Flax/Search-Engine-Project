#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include "tui/SearchPanel.h"

// Testing the search panel component in terminal with cmake build

int main() {
    UIState state;
    auto screen = ftxui::ScreenInteractive::TerminalOutput();

    auto panel = MakeSearchPanel(state, [&](const std::string& q) {
        state.status = "You searched: " + q;   // fake search for now
    });

    auto root = ftxui::Renderer(panel, [&] {
        return ftxui::vbox({panel->Render(), ftxui::text(state.status)});
    });

    screen.Loop(root);
}