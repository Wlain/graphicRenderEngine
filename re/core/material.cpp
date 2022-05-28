// Copyright (c) 2022. Lorem ipsum dolor sit amet, consectetur adipiscing elit.
// Morbi non lorem porttitor neque feugiat blandit. Ut vitae ipsum eget quam lacinia accumsan.
// Etiam sed turpis ac ipsum condimentum fringilla. Maecenas magna.
// Proin dapibus sapien vel ante. Aliquam erat volutpat. Pellentesque sagittis ligula eget metus.
// Vestibulum commodo. Ut rhoncus gravida arcu.

//
// Created by william on 2022/5/27.
//

#include "material.h"

#include "glCommonDefine.h"

#include <glm/gtc/type_ptr.hpp>
namespace re
{
Material::Material()
{
    m_shader = Shader::create()
                   .withSourceStandard()
                   .build();
    setShader(m_shader);
    m_name = "Undefined material";
}

Material::Material(Shader* shader)
{
    setShader(shader);
    m_name = "Undefined material";
}

Material::~Material()
{
    delete m_shader;
};

void Material::bind()
{
    uint32_t slot = 0;
    for (const auto& v : m_textureValues)
    {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(v.value->m_info.target, v.value->m_info.id);
        glUniform1i(v.id, slot);
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

Shader* Material::getShader() const
{
    return m_shader;
}

void Material::setShader(Shader* shader)
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
            Uniform<Texture*> uniform{};
            uniform.id = u.id;
            uniform.value = Texture::getWhiteTexture();
            m_textureValues.push_back(uniform);
        }
        break;
        case Shader::UniformType::TextureCube: {
            Uniform<Texture*> uniform{};
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
            LOG_ERROR("Unsupported type");
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
Texture* Material::get(std::string_view uniformName)
{
    auto t = m_shader->getType(uniformName.data());
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
glm::vec4 Material::get(std::string_view uniformName)
{
    auto t = m_shader->getType(uniformName.data());
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
    auto t = m_shader->getType(uniformName.data());
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

glm::vec4 Material::getColor()
{
    return get<glm::vec4>("color");
}

bool Material::setColor(const glm::vec4& color)
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

Texture* Material::getTexture()
{
    return get<Texture*>("tex");
}

bool Material::setTexture(Texture* texture)
{
    return set("tex", texture);
}

bool Material::set(std::string_view uniformName, const glm::vec4& value)
{
    auto type = m_shader->getType(uniformName.data());
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

bool Material::set(std::string_view uniformName, float value)
{
    auto type = m_shader->getType(uniformName.data());
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

bool Material::set(std::string_view uniformName, Texture* value)
{
    auto type = m_shader->getType(uniformName.data());
    for (auto& v : m_textureValues)
    {
        if (v.id == type.id)
        {
            v.value = value;
            return true;
        }
    }
    return false;
}
} // namespace re