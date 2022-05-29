//
// Created by william on 2022/5/26.
//

#ifndef SIMPLERENDERENGINE_WORLDLIGHTS_H
#define SIMPLERENDERENGINE_WORLDLIGHTS_H
#include "commonMacro.h"
#include "light.h"

#include <glm/glm.hpp>
#include <vector>
namespace re
{
class Light;

struct WorldLights
{
    WorldLights();
    inline size_t addLight(const Light& light)
    {
        lights.push_back(light);
        return lights.size() - 1;
    }

    inline void removeLight(int index)
    {
        ASSERT(index < lights.size());
        lights.erase(lights.begin() + index);
    }

    inline Light* getLight(int index)
    {
        if (index >= lights.size())
        {
            return nullptr;
        }
        return &lights[index];
    }
    inline size_t lightCount() const { return lights.size(); }
    inline void setAmbientLight(const glm::vec3& light)
    {
        float maxAmbient = std::max(light.x, std::max(light.y, light.z));
        ambientLight = glm::vec4(light, maxAmbient);
    }

public:
    glm::vec4 ambientLight;
    std::vector<Light> lights;
};
} // namespace re

#endif //SIMPLERENDERENGINE_WORLDLIGHTS_H