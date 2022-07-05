//
// Created by william on 2022/5/27.
//

#ifndef SIMPLERENDERENGINE_MATERIAL_H
#define SIMPLERENDERENGINE_MATERIAL_H
#include "color.h"
#include "shader.h"
#include "uniformSet.h"

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
    Material() = delete;
    explicit Material(const std::shared_ptr<Shader>& shader);
    ~Material();
    [[nodiscard]] const std::shared_ptr<Shader>& getShader() const;
    void setShader(const std::shared_ptr<Shader>& shader);
    [[nodiscard]] const std::string& getName() const;
    void setName(std::string_view name);
    // uniform parameters
    Color getColor();
    bool setColor(const Color& color);
    Color getSpecularity();
    bool setSpecularity(Color specularity);
    glm::vec2 getMetallicRoughness();
    // 金属粗糙度
    bool setMetallicRoughness(glm::vec2 metallicRoughness);
    std::shared_ptr<Texture> getMetallicRoughnessTexture();
    [[maybe_unused]] bool setMetallicRoughnessTexture(std::shared_ptr<Texture> texture);
    std::shared_ptr<Texture> getTexture();
    bool setTexture(const std::shared_ptr<Texture>& texture);
    bool set(std::string_view uniformName, const glm::vec4& value);
    bool set(std::string_view uniformName, Color value);
    bool set(std::string_view uniformName, float value);
    bool set(std::string_view uniformName, int value);
    bool set(std::string_view uniformName, const glm::mat3& value);
    bool set(std::string_view uniformName, const glm::mat4& value);
    bool set(std::string_view uniformName, const std::shared_ptr<Texture>& texture);
    bool set(std::string_view uniformName, const std::shared_ptr<std::vector<glm::mat3>>& value);
    bool set(std::string_view uniformName, const std::shared_ptr<std::vector<glm::mat4>>& value);
    template <typename T>
    T get(std::string_view uniformName);

private:
    void bind();

private:
    std::string m_name;
    std::shared_ptr<Shader> m_shader;
    UniformSet m_uniformMap;
    friend class Shader;
    friend class RenderPass;
};
} // namespace re

#endif //SIMPLERENDERENGINE_MATERIAL_H
