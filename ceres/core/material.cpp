//
// Created by william on 2022/5/27.
//

#include "material.h"

#include "glCommonDefine.h"
#include "renderer.h"

#include <glm/gtc/type_ptr.hpp>

namespace ceres
{
Material::Material(const std::shared_ptr<Shader>& shader)
{
    setShader(shader);
    m_name = "Undefined material";
}

Material::~Material() = default;

void Material::bind()
{
    m_uniformMap.bind();
}

const std::shared_ptr<Shader>& Material::getShader() const
{
    return m_shader;
}

void Material::setShader(const std::shared_ptr<Shader>& shader)
{
    m_shader = shader;
    m_uniformMap.clear();
    for (auto& u : shader->m_uniforms)
    {
        switch (u.type)
        {
        case Shader::UniformType::Vec4: {
            m_uniformMap.set(u.id, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
        }
        break;
        case Shader::UniformType::Texture: {
            m_uniformMap.set(u.id, Texture::getWhiteTexture());
        }
        break;
        case Shader::UniformType::TextureCube: {
            m_uniformMap.set(u.id, Texture::getCubeMapTexture());
        }
        break;
        case Shader::UniformType::Float: {
            m_uniformMap.set(u.id, 0.0f);
        }
        break;
        case Shader::UniformType::Int: {
            m_uniformMap.set(u.id, 0);
        }
        break;
        case Shader::UniformType::Mat3: {
            m_uniformMap.set(u.id, std::shared_ptr<std::vector<glm::mat3>>());
        }
        break;
        case Shader::UniformType::Mat4: {
            m_uniformMap.set(u.id, std::shared_ptr<std::vector<glm::mat4>>());
        }
        break;
        default:
            LOG_ERROR("'{}' Unsupported uniform type: {}. Only Vec4, Texture, TextureCube and Float is supported.", u.name.c_str(), (int)u.type);
            break;
        }
    }
}

const std::string& Material::getName() const
{
    return m_name;
}

void Material::setName(std::string_view name)
{
    m_name = name;
}

template <>
std::shared_ptr<Texture> Material::get(std::string_view uniformName)
{
    auto t = m_shader->getUniformType(uniformName.data());
    if (t.type != Shader::UniformType::Texture && t.type != Shader::UniformType::TextureCube)
    {
        return nullptr;
    }
    for (auto& tv : m_uniformMap.m_textureValues)
    {
        auto res = m_uniformMap.m_textureValues.find(t.id);
        if (res != m_uniformMap.m_textureValues.end())
        {
            return res->second;
        }
    }
    ASSERT(0);
    return nullptr;
}

template <>
Color Material::get(std::string_view uniformName)
{
    auto t = m_shader->getUniformType(uniformName.data());
    for (auto& v : m_uniformMap.m_vectorValues)
    {
        auto res = m_uniformMap.m_vectorValues.find(t.id);
        if (res != m_uniformMap.m_vectorValues.end())
        {
            Color value;
            value.setFromLinear(res->second);
            return value;
        }
    }
    ASSERT(0);
    return { 0.0, 0.0f, 0.0f, 0.0f };
}

template <>
glm::vec4 Material::get(std::string_view uniformName)
{
    auto t = m_shader->getUniformType(uniformName.data());
    auto res = m_uniformMap.m_vectorValues.find(t.id);
    if (res != m_uniformMap.m_vectorValues.end())
    {
        return res->second;
    }
    ASSERT(0);
    return glm::vec4{ 0.0 };
}

template <>
float Material::get(std::string_view uniformName)
{
    auto t = m_shader->getUniformType(uniformName.data());
    auto res = m_uniformMap.m_floatValues.find(t.id);
    if (res != m_uniformMap.m_floatValues.end())
    {
        return res->second;
    }
    ASSERT(0);
    return 0.0f;
}

template <>
int Material::get(std::string_view uniformName)
{
    auto t = m_shader->getUniformType(uniformName.data());
    auto res = m_uniformMap.m_intValues.find(t.id);
    if (res != m_uniformMap.m_intValues.end())
    {
        return res->second;
    }
    ASSERT(0);
    return 0;
}

template <>
std::shared_ptr<std::vector<glm::mat3>> Material::get(std::string_view uniformName)
{
    auto t = m_shader->getUniformType(uniformName.data());
    auto res = m_uniformMap.m_mat3ArrayValues.find(t.id);
    if (res != m_uniformMap.m_mat3ArrayValues.end())
    {
        return res->second;
    }
    ASSERT(0);
    return {};
}

template <>
std::shared_ptr<std::vector<glm::mat4>> Material::get(std::string_view uniformName)
{
    auto t = m_shader->getUniformType(uniformName.data());
    auto res = m_uniformMap.m_mat4ArrayValues.find(t.id);
    if (res != m_uniformMap.m_mat4ArrayValues.end())
    {
        return res->second;
    }
    ASSERT(0);
    return {};
}

Color Material::getColor()
{
    return get<Color>("color");
}

bool Material::setColor(const Color& color)
{
    return set("color", color);
}

Color Material::getSpecularity()
{
    return get<Color>("specularity");
}

bool Material::setSpecularity(Color specularity)
{
    return set("specularity", specularity);
}

std::shared_ptr<Texture> Material::getTexture()
{
    return get<std::shared_ptr<Texture>>("tex");
}

glm::vec2 Material::getMetallicRoughness()
{
    return (glm::vec2)get<glm::vec4>("metallicRoughness");
}

bool Material::setMetallicRoughness(glm::vec2 metallicRoughness)
{
    return set("metallicRoughness", glm::vec4(metallicRoughness, 0, 0));
}

std::shared_ptr<Texture> Material::getMetallicRoughnessTexture()
{
    return get<std::shared_ptr<Texture>>("mrTex");
}

[[maybe_unused]] bool Material::setMetallicRoughnessTexture(std::shared_ptr<Texture> texture)
{
    return set("mrTex", texture);
}

bool Material::setTexture(const std::shared_ptr<Texture>& texture)
{
    return set("tex", texture);
}

bool Material::set(std::string_view uniformName, const glm::vec4& value)
{
    auto type = m_shader->getUniformType(uniformName.data());
    m_uniformMap.set(type.id, value);
    return true;
}

bool Material::set(std::string_view uniformName, Color value)
{
    auto type = m_shader->getUniformType(uniformName.data());
    m_uniformMap.set(type.id, value);
    return true;
}

bool Material::set(std::string_view uniformName, float value)
{
    auto type = m_shader->getUniformType(uniformName.data());
    m_uniformMap.set(type.id, value);
    return true;
}

bool Material::set(std::string_view uniformName, int value)
{
    auto type = m_shader->getUniformType(uniformName.data());
    m_uniformMap.set(type.id, value);
    return true;
}

bool Material::set(std::string_view uniformName, const std::shared_ptr<Texture>& texture)
{
    auto type = m_shader->getUniformType(uniformName.data());
    m_uniformMap.set(type.id, texture);
    return true;
}

bool Material::set(std::string_view uniformName, const std::shared_ptr<std::vector<glm::mat3>>& value)
{
    auto type = m_shader->getUniformType(uniformName);
    m_uniformMap.set(type.id, value);
    return true;
}

bool Material::set(std::string_view uniformName, const std::shared_ptr<std::vector<glm::mat4>>& value)
{
    auto type = m_shader->getUniformType(uniformName);
    m_uniformMap.set(type.id, value);
    return true;
}

bool Material::set(std::string_view uniformName, const glm::mat3& value)
{
    auto type = m_shader->getUniformType(uniformName);
    m_uniformMap.set(type.id, value);
    return true;
}

bool Material::set(std::string_view uniformName, const glm::mat4& value)
{
    auto type = m_shader->getUniformType(uniformName);
    m_uniformMap.set(type.id, value);
    return true;
}

} // namespace ceres