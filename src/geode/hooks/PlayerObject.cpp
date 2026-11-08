#include "../pch.hpp"
#include "../../shared/hacks/labels/labels.hpp"

#include <Geode/modify/PlayerObject.hpp>

namespace openhack::hooks {
    struct PlayerObjectHook : geode::Modify<PlayerObjectHook, PlayerObject> {
        void pushButton(PlayerButton btn) {
            hacks::Labels::pushButton(this);
            PlayerObject::pushButton(btn);
        }

        void releaseButton(PlayerButton btn) {
            hacks::Labels::releaseButton(this);
            PlayerObject::releaseButton(btn);
        }
    };
}