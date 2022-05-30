// Copyright (c) 2022. Lorem ipsum dolor sit amet, consectetur adipiscing elit.
// Morbi non lorem porttitor neque feugiat blandit. Ut vitae ipsum eget quam lacinia accumsan.
// Etiam sed turpis ac ipsum condimentum fringilla. Maecenas magna.
// Proin dapibus sapien vel ante. Aliquam erat volutpat. Pellentesque sagittis ligula eget metus.
// Vestibulum commodo. Ut rhoncus gravida arcu.

//
// Created by william on 2022/5/27.
//

#ifndef SIMPLERENDERENGINE_MATERIAL_H
#define SIMPLERENDERENGINE_MATERIAL_H
#include "shader.h"

#include <glm/glm.hpp>
#include <string>
#include <string_view>
namespace re
{
class Shader;
class Texture;
class Material
{
public:
    Material();
    Material(const std::shared_ptr<Shader>& shader);
    ~Material();
    void bind();
    const std::shared_ptr<Shader>& getShader() const;
    void setShader(const std::shared_ptr<Shader>& shader);
    const std::string& getName() const;
    void setName(std::string_view name);
    // uniform parameters
    glm::vec4 getColor();
    bool setColor(const glm::vec4& color);
    float getSpecularity();
    bool setSpecularity(float specularity);
    Texture* getTexture();
    bool setTexture(Texture* texture);
    bool set(std::string_view uniformName, const glm::vec4& value);
    bool set(std::string_view uniformName, float value);
    bool set(std::string_view uniformName, Texture*);
    template <typename T>
    T get(std::string_view uniformName);

private:
    template <typename T>
    struct Uniform
    {
        int id;
        T value;
    };

private:
    std::string m_name;
    std::shared_ptr<Shader> m_shader;
    std::vector<Uniform<Texture*>> m_textureValues;
    std::vector<Uniform<glm::vec4>> m_vectorValues;
    std::vector<Uniform<float>> m_floatValues;
};

} // namespace re

#endif //SIMPLERENDERENGINE_MATERIAL_H
