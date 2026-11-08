#include "display.hpp"
#include "../../menu/menu.hpp"

namespace openhack::hacks {

    void Display::onInit() {
        config::setIfEmpty("hack.display.tps", 240.0f);
        config::setIfEmpty("hack.display.tps_bypass", false);

        // Create window
        menu::addWindow("Display", [&]() {
            bool needRefresh = false;

            gui::widthF(0.35f);
            needRefresh |= gui::inputFloat("##fps", "hack.display.fps", 5.f, 100000.f, "%.0f FPS");
            ImGui::SameLine(0, 2);
            needRefresh |= gui::checkbox("Unlock FPS", "hack.display.unlock_fps");
            gui::tooltip("Allows to set custom FPS target.");
            gui::width();

            gui::widthF(0.35f);
            needRefresh |= gui::inputFloat("##tps", "hack.display.tps", 0.f, 100000.f, "%.0f TPS");
            ImGui::SameLine(0, 2);
            needRefresh |= gui::checkbox("TPS Bypass", "hack.display.tps_bypass");
            gui::tooltip("Makes the game render less frames, but keep the physics updates at target TPS.\nUseful for low-end devices.");
            gui::width();

            if (needRefresh) {
                refreshRate();
            }
        });
    }

    void Display::onLateInit() {
        // Set default values
        auto *manager = GameManager::sharedState();
        config::set("hack.display.fps", manager->m_customFPSTarget);
        config::set("hack.display.unlock_fps", manager->getGameVariable("0116"));
    }

    void Display::refreshRate() {
        auto *manager = GameManager::sharedState();

        bool unlockFps = config::get<bool>("hack.display.unlock_fps");
        manager->setGameVariable("0116", unlockFps);

        if (unlockFps) {
            auto fps = config::get<float>("hack.display.fps");
            manager->m_customFPSTarget = fps;
        }
        manager->updateCustomFPS();
    }

    void Display::refreshPhysics() {}

    bool Display::isCheating() {
        return false;
    }

    void Display::playLayerInit(GJGameLevel *level) {}

    void Display::playLayerReset() {}

    static double s_extraDeltaTime = 0;

    void Display::schedulerUpdate(float dt, const std::function<void(float)>& original) {
        if (!config::get<bool>("hack.display.tps_bypass") || !PlayLayer::get()) {
            original(dt);
            return;
        }

        auto newDelta = 1.0f / config::get<float>("hack.display.tps");

        // Calculate number of steps
        auto steps = static_cast<int32_t>(dt / newDelta);
        double remainder = fmod(dt, newDelta);

        // Update the game
        for (int i = 0; i < steps; i++) {
            original(newDelta);
        }

        // Update the remainder
        s_extraDeltaTime += remainder;

        // If we have enough time, update the game
        if (s_extraDeltaTime >= newDelta) {
            original(newDelta);
            s_extraDeltaTime -= newDelta;
        }
    }

}