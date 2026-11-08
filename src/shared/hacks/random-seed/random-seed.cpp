#include "random-seed.hpp"
#include "../../menu/menu.hpp"

namespace openhack::hacks {

    static uint32_t s_editedSeed = 0;

    uint32_t RandomSeed::getCurrentSeed() {
        return static_cast<uint32_t>(GameToolbox::getfast_srand());
    }

    void RandomSeed::setCurrentSeed(uint32_t seed) {
        GameToolbox::fast_srand(seed);
    }

    void RandomSeed::onInit() {
        // Set the default value
        config::setIfEmpty("hack.random_seed.enabled", false);
        config::setIfEmpty("hack.random_seed.seed", 0);
        config::setIfEmpty("hack.random_seed.freeze", false);

        // Initialize keybind
        menu::keybinds::setKeybindCallback("random_seed.enabled", []() {
            bool enabled = !config::get<bool>("hack.random_seed.enabled");
            config::set("hack.random_seed.enabled", enabled);
        });
    }

    void RandomSeed::onDraw() {
        gui::callback([](){
            gui::tooltip("Allows you to set the random seed, which is used by random triggers");
            menu::keybinds::addMenuKeybind("random_seed.enabled", "Random Seed", [](){
                bool enabled = !config::get<bool>("hack.random_seed.enabled");
                config::set("hack.random_seed.enabled", enabled);
            });
        });
        gui::toggleSetting("Random Seed", "hack.random_seed.enabled", []() {
            gui::width(80);
            gui::inputFloat("Seed", "hack.random_seed.seed", 0, FLT_MAX, "%.0f");
            gui::width();
            gui::tooltip("Set the random seed every time the level is restarted.");
            gui::checkbox("Freeze", "hack.random_seed.freeze");
            gui::tooltip("Lock the random seed to the value set above.");

            ImGui::Separator();
            gui::width(80);
            auto fltEditedSeed = static_cast<float>(s_editedSeed);
            gui::inputFloat("##applySeed", &fltEditedSeed, 0, FLT_MAX, "%.0f");
            s_editedSeed = static_cast<uint32_t>(fltEditedSeed);
            gui::width();
            ImGui::SameLine(0, 2);
            if (gui::button("Apply")) {
                setCurrentSeed(s_editedSeed);
            }
            gui::tooltip("Set the random seed value.");
            gui::text("Current seed: %d", getCurrentSeed());
        }, ImVec2(0, 0), 150);
    }

    void RandomSeed::update() {
        if (!isCheating()) return;

        if (config::get<bool>("hack.random_seed.freeze", false)) {
            setCurrentSeed(config::get<uint32_t>("hack.random_seed.seed", 0));
        }
    }

    bool RandomSeed::isCheating() {
        return config::get<bool>("hack.random_seed.enabled", false);
    }

    void RandomSeed::resetLevel() {
        if (config::get<bool>("hack.random_seed.enabled", false)) {
            setCurrentSeed(config::get<uint32_t>("hack.random_seed.seed", 0));
        }
    }
}