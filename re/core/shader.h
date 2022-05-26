// Copyright (c) 2022. Lorem ipsum dolor sit amet, consectetur adipiscing elit.
// Morbi non lorem porttitor neque feugiat blandit. Ut vitae ipsum eget quam lacinia accumsan.
// Etiam sed turpis ac ipsum condimentum fringilla. Maecenas magna.
// Proin dapibus sapien vel ante. Aliquam erat volutpat. Pellentesque sagittis ligula eget metus.
// Vestibulum commodo. Ut rhoncus gravida arcu.

//
// Created by william on 2022/5/22.
//

#ifndef SIMPLERENDERENGINE_SHADER_H
#define SIMPLERENDERENGINE_SHADER_H
#include "light.h"
#include "texture.h"

#include <string>
#include <vector>
namespace re
{
class Texture;
class Shader
{
public:
    enum class BlendType
    {
        Disabled,
        AlphaBlending,   // 根据 alpha值将表面与背景混合（src alpha ,one minus src alpha）
        AdditiveBlending // 根据 alpha 值添加光线（src alpha,one）
    };

    enum class UniformType
    {
        Int,
        Float,
        Mat3,
        Mat4,
        Vec4,
        Texture,
        Invalid
    };

    struct Uniform
    {
        Uniform(const char* _name, int32_t _location, UniformType _type, int32_t _count) :
            name(_name), location(_location), type(_type), arrayCount(_count) {}
        Uniform() = default;
        std::string name;
        int32_t location{ -1 };
        UniformType type{ UniformType::Invalid };
        // 1 means not array
        int32_t arrayCount{ -1 };
    };

    class ShaderBuilder
    {
    public:
        ShaderBuilder& withSource(const char* vertexShader, const char* fragmentShader);
        ShaderBuilder& withSourceStandard();
        ShaderBuilder& withSourceUnlit();
        ShaderBuilder& withSourceUnlitSprite();
        ShaderBuilder& withSourceStandardParticles();
        ShaderBuilder& withDepthTest(bool enable);
        ShaderBuilder& withDepthWrite(bool enable);
        ShaderBuilder& withBlend(BlendType blendType);
        ShaderBuilder& withParticleLayout(bool enable);
        Shader* build();

    private:
        ShaderBuilder() = default;
        const char* m_vertexShaderStr{ nullptr };
        const char* m_fragmentShaderStr{ nullptr };
        BlendType m_blendType{ BlendType::Disabled };
        unsigned int m_id{ 0 };
        bool m_depthTest{ true };
        bool m_depthWrite{ true };
        bool m_particleLayout{ false };
        friend class Shader;
    };

public:
    static ShaderBuilder create();
    static Shader* createShader(const char* vertexShader, const char* fragmentShader, bool particleLayout = false);
    /// Unlit model.
    // Attributes
    // "color" vec4 (default (1,1,1,1))
    // "tex" Texture* (default white texture)
    static Shader* getUnlit();
    // Attributes
    // "color" vec4 (default (1,1,1,1))
    // "tex" Texture* (default white texture)
    static Shader* getUnlitSprite();
    /// Phong Light Model. Uses light objects and ambient light set in simpleRenderEngine.
    // Attributes
    // "color" vec4 (default (1,1,1,1))
    // "tex" Texture* (default white texture)
    // "specularity" float (default 0 = no specularity)
    static Shader* getStandard();
    static Shader* getStandardParticles();

public:
    ~Shader();
    bool contains(const char* name);
    Uniform getType(const char* name);
    bool set(const char* name, glm::mat4 value);
    bool set(const char* name, glm::mat3 value);
    bool set(const char* name, glm::vec4 value);
    bool set(const char* name, float value);
    bool set(const char* name, int value);
    bool set(const char* name, Texture* texture, unsigned int textureSlot = 0);
    //    inline void setDepthTest(bool enable) { m_depthTest = enable; }
    inline bool isDepthTest() const { return m_depthTest; }
    //    inline void setDepthWrite(bool enable) { m_depthWrite = enable; }
    inline bool isDepthWrite() const { return m_depthWrite; }
    //    inline void setBlend(BlendType type) { m_blending = type; }
    inline BlendType getBlend() const { return m_blendType; }

private:
    Shader();
    bool build(const char* vertexShader, const char* fragmentShader, bool particleLayout);
    void bind();
    bool setLights(Light value[4], const glm::vec4& ambient, const glm::mat4& viewTransform);
    void updateUniforms();

private:
    inline static Shader* s_unlit{ nullptr }; // 无灯光
    inline static Shader* s_unlitSprite{ nullptr };
    inline static Shader* s_debugUV{ nullptr };
    inline static Shader* s_debugNormals{ nullptr };
    inline static Shader* s_standard{ nullptr };
    inline static Shader* s_font{ nullptr };
    inline static Shader* s_standardParticles{ nullptr };

private:
    friend class Mesh;
    friend class Renderer;
    BlendType m_blendType{ BlendType::Disabled };
    std::vector<Uniform> m_uniforms;
    unsigned int m_id{ 0 };
    bool m_depthTest{ true };
    bool m_depthWrite{ true };
    bool m_particleLayout{ false };
};
} // namespace re

#endif //SIMPLERENDERENGINE_SHADER_H
