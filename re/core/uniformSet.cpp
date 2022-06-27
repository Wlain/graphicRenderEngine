//
// Created by william on 2022/6/25.
//

#include "uniformSet.h"
namespace re
{
void UniformSet::set(int id, const glm::vec4& value)
{
}
void UniformSet::set(int id, float value)
{
}
void UniformSet::set(int id, int value)
{
}
void UniformSet::set(int id, Color value)
{
}
void UniformSet::set(int id, const std::shared_ptr<Texture>& texture)
{
}
void UniformSet::set(int id, const std::shared_ptr<std::vector<glm::mat4>>& value)
{
}
void UniformSet::set(int id, const std::shared_ptr<std::vector<glm::mat3>>& value)
{
}
void UniformSet::clear()
{
}
void UniformSet::bind()
{
}
template <typename T>
T UniformSet::get(int id)
{
    return nullptr;
}

template<>
Color UniformSet::get(int id)
{

}
} // namespace re