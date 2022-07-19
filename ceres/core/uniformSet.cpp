//
// Created by william on 2022/6/25.
//

#include "uniformSet.h"

#include "glCommonDefine.h"

#include <glm/gtc/type_ptr.hpp>
namespace ceres
{
void UniformSet::set(int id, const glm::vec4& value)
{
    m_vectorValues[id] = value;
}

void UniformSet::set(int id, float value)
{
    m_floatValues[id] = value;
}

void UniformSet::set(int id, int value)
{
    m_intValues[id] = value;
}

void UniformSet::set(int id, Color value)
{
    m_vectorValues[id] = value.toLinear();
}

void UniformSet::set(int id, const std::shared_ptr<Texture>& texture)
{
    m_textureValues[id] = texture;
}

void UniformSet::set(int id, const std::shared_ptr<std::vector<glm::mat4>>& value)
{
    m_mat4ArrayValues[id] = value;
}

void UniformSet::set(int id, const std::shared_ptr<std::vector<glm::mat3>>& value)
{
    m_mat3ArrayValues[id] = value;
}

void UniformSet::set(int id, const glm::mat3& value)
{
    m_mat3Values[id] = value;
}

void UniformSet::set(int id, const glm::mat4& value)
{
    m_mat4Values[id] = value;
}

void UniformSet::clear()
{
    m_textureValues.clear();
    m_vectorValues.clear();
    m_floatValues.clear();
    m_intValues.clear();
    m_mat3Values.clear();
    m_mat4Values.clear();
    m_mat4ArrayValues.clear();
    m_mat3ArrayValues.clear();
}

void UniformSet::bind()
{
    uint32_t textureSlot = 0;
    for (const auto& v : m_textureValues)
    {
        glActiveTexture(GL_TEXTURE0 + textureSlot);
        glBindTexture(v.second->m_info.target, v.second->m_info.id);
        glUniform1i(v.first, textureSlot++);
    }
    for (const auto& v : m_vectorValues)
    {
        glUniform4fv(v.first, 1, glm::value_ptr(v.second));
    }
    for (const auto& v : m_floatValues)
    {
        glUniform1f(v.first, v.second);
    }
    for (const auto& v : m_intValues)
    {
        glUniform1i(v.first, v.second);
    }
    for (const auto& v : m_mat3Values)
    {
        glUniformMatrix3fv(v.first, 1, GL_FALSE, glm::value_ptr((v.second)[0]));
    }
    for (const auto& t : m_mat3ArrayValues)
    {
        if (t.second != nullptr)
        {
            glUniformMatrix3fv(t.first, static_cast<GLsizei>(t.second->size()), GL_FALSE, glm::value_ptr((*t.second)[0]));
        }
    }
    for (const auto& v : m_mat4Values)
    {
        glUniformMatrix4fv(v.first, 1, GL_FALSE, glm::value_ptr((v.second)[0]));
    }
    for (const auto& t : m_mat4ArrayValues)
    {
        if (t.second != nullptr)
        {
            glUniformMatrix4fv(t.first, static_cast<GLsizei>(t.second->size()), GL_FALSE, glm::value_ptr((*t.second)[0]));
        }
    }
}

template <>
std::shared_ptr<Texture>& UniformSet::get(int id)
{
    return m_textureValues[id];
}

template <>
Color UniformSet::get(int id)
{
    Color value;
    value.setFromLinear(m_vectorValues[id]);
    return value;
}

template <>
glm::vec4 UniformSet::get(int id)
{
    return m_vectorValues[id];
}

template <>
float UniformSet::get(int id)
{
    return m_floatValues[id];
}

template <>
int UniformSet::get(int id)
{
    return m_intValues[id];
}

template <>
std::shared_ptr<std::vector<glm::mat3>> UniformSet::get(int id)
{
    return m_mat3ArrayValues[id];
}

template <>
std::shared_ptr<std::vector<glm::mat4>> UniformSet::get(int id)
{
    return m_mat4ArrayValues[id];
}
} // namespace ceres