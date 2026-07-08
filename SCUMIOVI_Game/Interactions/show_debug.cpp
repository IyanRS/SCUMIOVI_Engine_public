#include "EngineAPI.hpp"

bool debug_enabled = false;

void show_debug(EventInfo e) {
    auto scene = e.game.getScene();
    if (!debug_enabled) {
        for (auto& obj : scene->getGameObjects()) {
            auto pos  = obj->getPosition();
            auto size = obj->getSize();

            scene->addDebugLine(pos,                         pos + Vector2<>(size.x, 0));
            scene->addDebugLine(pos + Vector2<>(size.x, 0),  pos + size);
            scene->addDebugLine(pos + size,                  pos + Vector2<>(0, size.y));
            scene->addDebugLine(pos + Vector2<>(0, size.y),  pos);
        }
        debug_enabled = true;
    } else {
        scene->getDebugPoints().clear();
        scene->getDebugLines().clear();
        debug_enabled = false;
    }
    
}
