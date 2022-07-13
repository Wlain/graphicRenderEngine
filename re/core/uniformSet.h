//
// Created by william on 2022/6/25.
//

#ifndef SIMPLERENDERENGINE_UNIFORMSET_H
#define SIMPLERENDERENGINE_UNIFORMSET_H
#include "color.h"
#include "texture.h"

#include <glm/glm.hpp>
#include <vector>

namespace re
{
class UniformSet
{
public:
    void set(int id, const glm::vec4& value);
    void set(int id, float value);
    void set(int id, int value);
    void set(int id, Color value);
    void set(int id, const std::shared_ptr<Texture>& texture);
    void set(int id, const glm::mat3& value);
    void set(int id, const glm::mat4& value);
    void set(int id, const std::shared_ptr<std::vector<glm::mat4>>& value);
    void set(int id, const std::shared_ptr<std::vector<glm::mat3>>& value);
    void clear();
    void bind();
    template <typename T>
    T get(int id);

private:
    std::map<int, std::shared_ptr<Texture>> m_textureValues;
    std::map<int, glm::vec4> m_vectorValues;
    std::map<int, glm::mat3> m_mat3Values;
    std::map<int, glm::mat4> m_mat4Values;
    std::map<int, std::shared_ptr<std::vector<glm::mat4>>> m_mat4ArrayValues;
    std::map<int, std::shared_ptr<std::vector<glm::mat3>>> m_mat3ArrayValues;
    std::map<int, float> m_floatValues;
    std::map<int, int> m_intValues;
    friend class Material;
};
} // namespace re

#endif // SIMPLERENDERENGINE_UNIFORMSET_H
