#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

namespace openhack::utils {
    /// @brief Game version string (e.g. "2.2081").
    std::string getGameVersion();

    /// @brief Opens a URL in the default browser.
    void openURL(const char *url);

    /// @brief Directories are not browsable on Android, so this does nothing.
    void openDirectory(const char *path);

    std::string getCurrentDirectory();
    std::string getSaveDirectory();

    /// Keyboard helpers. On a touch device there is no keyboard, so these always report "not pressed".
    std::string getKeyName(uint32_t keycode);
    uint32_t getKeyCode(std::string_view key);
    bool isKeyDown(uint32_t keycode);
    bool isKeyDown(std::string_view key);
    bool isKeyPressed(uint32_t keycode);
    bool isKeyPressed(std::string_view key);
    bool isKeyReleased(uint32_t keycode);
    bool isKeyReleased(std::string_view key);
    void resetKeyStates();
    void pressKey(uint32_t keycode);
    void pressKey(std::string_view key);
}
