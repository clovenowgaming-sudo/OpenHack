#include "android.hpp"
#include <Geode/Geode.hpp>

namespace openhack::utils {
    std::string getGameVersion() {
        return GEODE_GD_VERSION_STRING;
    }

    void openURL(const char *url) {
        geode::utils::web::openLinkInBrowser(url);
    }

    void openDirectory(const char *) {}

    std::string getCurrentDirectory() {
        return geode::dirs::getGameDir().string();
    }

    std::string getSaveDirectory() {
        return geode::dirs::getSaveDir().string();
    }

    std::string getKeyName(uint32_t keycode) {
        if (keycode == 0) return "...";
        return "Key " + std::to_string(keycode);
    }

    uint32_t getKeyCode(std::string_view) { return 0; }
    bool isKeyDown(uint32_t) { return false; }
    bool isKeyDown(std::string_view) { return false; }
    bool isKeyPressed(uint32_t) { return false; }
    bool isKeyPressed(std::string_view) { return false; }
    bool isKeyReleased(uint32_t) { return false; }
    bool isKeyReleased(std::string_view) { return false; }
    void resetKeyStates() {}
    void pressKey(uint32_t) {}
    void pressKey(std::string_view) {}
}
