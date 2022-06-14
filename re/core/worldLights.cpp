//
// Created by william on 2022/5/26.
//

#include "worldLights.h"
namespace re
{
WorldLights::WorldLights()
{
    setAmbientLight(glm::vec3(0.2, 0.2, 0.2));
}

void WorldLights::clear()
{
}
} // namespace re