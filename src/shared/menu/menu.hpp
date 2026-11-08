#pragma once

#include <functional>
#include <string>
#include <map>

#include "../gui/gui.hpp"
#include "../gui/window.hpp"

namespace openhack::menu {
    /// @brief Open/close the menu
    void toggle();

    /// @return True if the menu is opened
    bool isOpen();

    /// @brief Function for menu initialization. Creates all required windows.
    void init();

    /// @brief Function which is called every frame to draw the menu.
    void draw();

    /// @brief Adds specified window to the draw stack
    /// @param window Window to add
    void addWindow(std::string_view title, const std::function<void()> &onDraw);

}
