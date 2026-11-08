#include "../pch.hpp"

#include <Geode/modify/GameManager.hpp>
#include <Geode/modify/CCTextInputNode.hpp>
#include <Geode/modify/GJGameLevel.hpp>
#include <Geode/modify/GameStatsManager.hpp>

namespace openhack::hooks {
    /// Unlock All Icons & Colors
    struct IconUnlockHook : geode::Modify<IconUnlockHook, GameManager> {
        bool isIconUnlocked(int id, IconType type) {
            if (hacks::isOn("bypass.icons")) return true;
            return GameManager::isIconUnlocked(id, type);
        }

        bool isColorUnlocked(int id, UnlockType type) {
            if (hacks::isOn("bypass.icons")) return true;
            return GameManager::isColorUnlocked(id, type);
        }
    };

    /// Text length bypass + character filter bypass
    struct TextInputHook : geode::Modify<TextInputHook, CCTextInputNode> {
        struct Fields {
            bool saved = false;
            int maxLength = 0;
            gd::string allowed;
            bool filterSwear = false;
        };

        bool onTextFieldInsertText(cocos2d::CCTextFieldTTF *sender, char const *text, int len,
                                   cocos2d::enumKeyCodes key) {
            auto *f = m_fields.self();
            if (!f->saved) {
                f->saved = true;
                f->maxLength = m_maxLabelLength;
                f->allowed = m_allowedChars;
                f->filterSwear = m_filterSwearWords;
            }

            m_maxLabelLength = hacks::isOn("bypass.text") ? 99999 : f->maxLength;

            if (hacks::isOn("bypass.filter")) {
                m_allowedChars = "";
                m_filterSwearWords = false;
            } else {
                m_allowedChars = f->allowed;
                m_filterSwearWords = f->filterSwear;
            }

            return CCTextInputNode::onTextFieldInsertText(sender, text, len, key);
        }
    };

    /// Safe Mode: never save progress
    struct SafeModeLevelHook : geode::Modify<SafeModeLevelHook, GJGameLevel> {
        void savePercentage(int percent, bool practice, int clicks, int attempts, bool valid) {
            if (hacks::isOn("universal.safemode")) return;
            GJGameLevel::savePercentage(percent, practice, clicks, attempts, valid);
        }
    };

    struct SafeModeStatsHook : geode::Modify<SafeModeStatsHook, GameStatsManager> {
        void completedLevel(GJGameLevel *level) {
            if (hacks::isOn("universal.safemode")) return;
            GameStatsManager::completedLevel(level);
        }
    };
}
