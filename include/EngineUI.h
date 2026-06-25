#ifndef ENGINE_UI_H
#define ENGINE_UI_H

#include "SearchEngine.h"

class EngineUI : public SearchEngine {
public:
    EngineUI() {}
    ~EngineUI() {}

    // For displaying the CLI UI
    void ui();

    // For handling command-line arguments and executing the search engine
    void go(int argc, char** argv);
};

#endif