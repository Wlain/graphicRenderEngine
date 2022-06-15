//
// Created by william on 2022/5/27.
//

#include "material.h"

#include "glCommonDefine.h"
#include "renderer.h"

#include <glm/gtc/type_ptr.hpp>

namespace re
{
Material::Material(const std::shared_ptr<Shader>& shader)
{
    setShader(shader);
    m_name = "Undefined material";
}

Material::~Material() = default;

void Material::bind()
{
    uint32_t slot = 0;
    for (const auto& v : m_textureValues)
    {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(v.value->m_info.target, v.value->m_info.id);
        glUniform1i(v.id, slot++);
    }
    for (const auto& v : m_vectorValues)
    {
        glUniform4fv(v.id, 1, glm::value_ptr(v.value));
    }
    for (const auto& v : m_floatValues)
    {
        glUniform1f(v.id, v.value);
    }
}

const std::shared_ptr<Shader>& Material::getShader() const
{
    return m_shader;
}

void Material::setShader(const std::shared_ptr<Shader>& shader)
{
    m_shader = shader;
    m_textureValues.clear();
    m_vectorValues.clear();
    m_floatValues.clear();

    for (auto& u : shader->m_uniforms)
    {
        switch (u.type)
        {
        case Shader::UniformType::Vec4: {
            Uniform<glm::vec4> uniform{};
            uniform.id = u.id;
            uniform.value = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
            m_vectorValues.push_back(uniform);
        }
        break;
        case Shader::UniformType::Texture: {
            Uniform<std::shared_ptr<Texture>> uniform{};
            uniform.id = u.id;
            uniform.value = Texture::getWhiteTexture();
            m_textureValues.push_back(uniform);
        }
        break;
        case Shader::UniformType::TextureCube: {
            Uniform<std::shared_ptr<Texture>> uniform{};
            uniform.id = u.id;
            uniform.value = Texture::getCubeMapTexture();
            m_textureValues.push_back(uniform);
        }
        break;
        case Shader::UniformType::Float: {
            Uniform<float> uniform{};
            uniform.id = u.id;
            uniform.value = 0.0f;
            m_floatValues.push_back(uniform);
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
    for (auto& tv : m_textureValues)
    {
        if (tv.id == t.id)
        {
            return tv.value;
        }
    }
    return nullptr;
}

template <>
Color Material::get(std::string_view uniformName)
{
    auto t = m_shader->getUniformType(uniformName.data());
    for (auto& v : m_vectorValues)
    {
        if (v.id == t.id)
        {
            Color value;
            value.setFromLinear(v.value);
            return value;
        }
    }
    return { 0.0, 0.0f, 0.0f, 0.0f };
}

template <>
glm::vec4 Material::get(std::string_view uniformName)
{
    auto t = m_shader->getUniformType(uniformName.data());
    if (t.type != Shader::UniformType::Vec4)
    {
        return glm::vec4{ 0.0 };
    }
    for (auto& v : m_vectorValues)
    {
        if (v.id == t.id)
        {
            return v.value;
        }
    }
    return glm::vec4{ 0.0 };
}

template <>
float Material::get(std::string_view uniformName)
{
    auto t = m_shader->getUniformType(uniformName.data());
    if (t.type != Shader::UniformType::Float)
    {
        return 0.0f;
    }
    for (auto& v : m_floatValues)
    {
        if (v.id == t.id)
        {
            return v.value;
        }
    }
    return 0.0f;
}

Color Material::getColor()
{
    return get<Color>("color");
}

bool Material::setColor(const Color& color)
{
    return set("color", color);
}

float Material::getSpecularity()
{
    return get<float>("specularity");
}

bool Material::setSpecularity(float specularity)
{
    return set("specularity", specularity);
}

std::shared_ptr<Texture> Material::getTexture()
{
    return get<std::shared_ptr<Texture>>("tex");
}

bool Material::setTexture(std::shared_ptr<Texture> texture)
{
    return set("tex", texture);
}

bool Material::set(std::string_view uniformName, const glm::vec4& value)
{
    auto type = m_shader->getUniformType(uniformName.data());
    for (auto& v : m_vectorValues)
    {
        if (v.id == type.id)
        {
            v.value = value;
            return true;
        }
    }
    return false;
}

bool Material::set(std::string_view uniformName, Color value)
{
    auto type = m_shader->getUniformType(uniformName.data());
    for (auto& v : m_vectorValues)
    {
        if (v.id == type.id)
        {
            v.value = value.toLinear();
        }
        return true;
    }
    return false;
}

bool Material::set(std::string_view uniformName, float value)
{
    auto type = m_shader->getUniformType(uniformName.data());
    for (auto& v : m_floatValues)
    {
        if (v.id == type.id)
        {
            v.value = value;
            return true;
        }
    }
    return false;
}

bool Material::set(std::string_view uniformName, std::shared_ptr<Texture> texture)
{
    auto type = m_shader->getUniformType(uniformName.data());
    for (auto& v : m_textureValues)
    {
        if (v.id == type.id)
        {
            v.value = texture;
            return true;
        }
    }
    return false;
}
} // namespace re