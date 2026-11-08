#include "../pch.hpp"
#include <imgui-cocos.hpp>

#include <Geode/modify/MenuLayer.hpp>

namespace openhack::hooks {
    struct MenuLayerHook : geode::Modify<MenuLayerHook, MenuLayer> {
        bool init() {
            if (!MenuLayer::init())
                return false;

            if (!config::getGlobal<bool>("guiInited", false)) {
                ImGuiCocos::get()
                        .setup(menu::init)
                        .draw([]() {
                            ImGuiCocos::get().setInputMode(ImGuiCocos::InputMode::Default);

                            menu::draw();
                        });
                config::setGlobal("guiInited", true);
            }

            return true;
        }
    };
}