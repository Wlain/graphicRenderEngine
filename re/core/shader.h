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
    /// Unlit model.
    // Attributes
    // "color" vec4 (default (1,1,1,1))
    // "tex" Texture* (default white texture)
    static Shader* getUnlit();
    // Attributes
    // "color" vec4 (default (1,1,1,1))
    // "tex" Texture* (default white texture)
    static Shader* getUnlitSprite();
    static Shader* getDebugUV();
    static Shader* getDebugNormals();
    /// Phong Light Model. Uses light objects and ambient light set in simpleRenderEngine.
    // Attributes
    // "color" vec4 (default (1,1,1,1))
    // "tex" Texture* (default white texture)
    // "specularity" float (default 0 = no specularity)
    static Shader* getStandard();
    /// Creates a shader for font rendering
    // "color" vec4 (default (1,1,1,1))
    // "tex" Texture* (default font texture)
    static Shader* getFont();

    ~Shader();
    bool set(const char* name, glm::mat4 value);
    bool set(const char* name, glm::mat3 value);
    bool set(const char* name, glm::vec4 value);
    bool set(const char* name, float value);
    bool set(const char* name, int value);
    bool set(const char* name, Texture* texture, unsigned int textureSlot = 0);
    inline void setDepthTest(bool enable) { m_depthTest = enable; }
    inline bool isDepthTest() const { return m_depthTest; }
    inline void setDepthWrite(bool enable) { m_depthWrite = enable; }
    inline bool isDepthWrite() const { return m_depthWrite; }
    inline void setBlend(BlendType type) { m_blending = type; }
    inline BlendType getBlend() const { return m_blending; }

private:
    Shader();
    void bind();
    bool setLights(Light value[4], const glm::vec4& ambient, const glm::mat4& viewTransform);
    friend class Mesh;
    friend class Renderer;

private:
    inline static Shader* s_unlit{ nullptr }; // 无灯光
    inline static Shader* s_unlitSprite{ nullptr };
    inline static Shader* s_debugUV{ nullptr };
    inline static Shader* s_debugNormals{ nullptr };
    inline static Shader* s_standard{ nullptr };
    inline static Shader* s_font{ nullptr };

private:
    BlendType m_blending{ BlendType::Disabled };
    unsigned int m_id{ 0 };
    bool m_depthTest{ true };
    bool m_depthWrite{ true };
};
} // namespace re

#endif //SIMPLERENDERENGINE_SHADER_H
