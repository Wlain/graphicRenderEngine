//
// Created by william on 2022/5/22.
//

#ifndef SIMPLERENDERENGINE_SHADER_H
#define SIMPLERENDERENGINE_SHADER_H
#include "light.h"
#include "texture.h"

#include <glm/glm.hpp>
namespace re
{
class Texture;
class Shader
{
public:
    enum class BlendType
    {
        Disabled,
        AlphaBlending
    };

public:
    static Shader* createShader(const char* vertexShader, const char* fragmentShader);
    static Shader* getUnlit();
    static Shader* getDebugUV();
    static Shader* getDebugNormals();
    static Shader* getStandard();

    ~Shader();
    bool setMatrix(const char* name, glm::mat4 value);
    bool setMatrix(const char* name, glm::mat3 value);
    bool setVector(const char* name, glm::vec4 value);
    bool setFloat(const char* name, float value);
    bool setInt(const char* name, int value);
    inline void setDepthTest(bool enable) { m_depthTest = enable; }
    inline bool isDepthTest() const { return m_depthTest; }
    inline void setDepthWrite(bool enable) { m_depthWrite = enable; }
    inline bool isDepthWrite() const { return m_depthWrite; }
    inline void setBlend(BlendType type) { m_blending = type; }
    inline BlendType getBlend() const { return m_blending; }
    bool setTexture(const char* name, Texture* texture, unsigned int textureSlot = 0);

private:
    Shader();
    void bind();
    bool setLights(Light value[4], const glm::vec4& ambient, const glm::mat4& viewTransform);
    friend class Mesh;
    friend class Renderer;

private:
    inline static Shader* s_unlit{ nullptr }; // 无灯光
    inline static Shader* s_debugUV{ nullptr };
    inline static Shader* s_debugNormals{ nullptr };
    inline static Shader* s_standard{ nullptr };

private:
    BlendType m_blending{ BlendType::Disabled };
    unsigned int m_id{ 0 };
    bool m_depthTest{ true };
    bool m_depthWrite{ true };
};
} // namespace re

#endif //SIMPLERENDERENGINE_SHADER_H
