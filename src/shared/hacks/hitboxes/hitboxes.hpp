#pragma once

#include "../hacks.hpp"

namespace openhack::hacks {

    /// @brief Enables you to see the hitboxes of objects in the game.
    class Hitboxes : public EmbeddedHack {
    public:
        Hitboxes() : EmbeddedHack("Show Hitboxes", "hitboxes") {}

        void onInit() override;
        void onDraw() override;
        void update() override {}
        bool isCheating() override;

    public:
        /// @brief CCDrawNode::drawPolygon and CCDrawNode::drawCircle hook
        static void modifyDraw(cocos2d::CCDrawNode* node, gui::Color &color, float &borderWidth, gui::Color &borderColor);

        /// @brief Called before PlayLayer::postUpdate to force the game to run its debug draw
        static void forceDebugDraw(PlayLayer *playLayer);

        /// @brief PlayLayer::postUpdate hook
        static void postUpdate();

        /// @brief PlayLayer::resetLevel hook
        static void resetLevel();

        /// @brief PlayLayer::fullReset hook
        static void fullReset();

        /// @brief PlayLayer::destroyPlayer hook
        static void destroyPlayer();

        /// @brief GJBaseGameLayer::processCommands hook
        static void processCommands();
    };

}