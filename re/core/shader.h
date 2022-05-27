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
class WorldLights;
class Material;
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
        Invalid,
        TextureCube
    };

    struct Uniform
    {
        Uniform(const char* _name, int32_t _location, UniformType _type, int32_t _count) :
            name(_name), id(_location), type(_type), arrayCount(_count) {}
        Uniform() = default;
        std::string name;
        int32_t id{ -1 };
        UniformType type{ UniformType::Invalid };
        // 1 means not array
        int32_t arrayCount{ -1 };
    };

    class ShaderBuilder
    {
    public:
        ShaderBuilder& withSource(std::string_view vertexShader, std::string_view fragmentShader);
        ShaderBuilder& withSourceStandard();
        ShaderBuilder& withSourceUnlit();
        ShaderBuilder& withSourceUnlitSprite();
        ShaderBuilder& withSourceStandardParticles();
        ShaderBuilder& withDepthTest(bool enable);
        ShaderBuilder& withDepthWrite(bool enable);
        ShaderBuilder& withBlend(BlendType blendType);
        Shader* build();

    private:
        ShaderBuilder() = default;
        std::string m_vertexShaderStr;
        std::string m_fragmentShaderStr;
        BlendType m_blendType{ BlendType::Disabled };
        unsigned int m_id{ 0 };
        bool m_depthTest{ true };
        bool m_depthWrite{ true };
        friend class Shader;
    };

public:
    static ShaderBuilder create();
    /// Unlit model.
    // Attributes
    // "color" vec4 (default (1,1,1,1))
    // "tex" Texture* (default white texture)
    // "specular" float (default 0.0) (means no specular)
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
    bool contains(std::string_view name);
    Uniform getType(std::string_view name);
    //    inline void setDepthTest(bool enable) { m_depthTest = enable; }
    inline bool isDepthTest() const { return m_depthTest; }
    //    inline void setDepthWrite(bool enable) { m_depthWrite = enable; }
    inline bool isDepthWrite() const { return m_depthWrite; }
    //    inline void setBlend(BlendType type) { m_blending = type; }
    inline BlendType getBlend() const { return m_blendType; }

private:
    Shader();
    bool build(std::string_view vertexShader, std::string_view fragmentShader);
    void bind();
    bool setLights(WorldLights* worldLights, const glm::mat4& viewTransform);
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
    BlendType m_blendType{ BlendType::Disabled };
    std::vector<Uniform> m_uniforms;
    unsigned int m_id{ 0 };
    bool m_depthTest{ true };
    bool m_depthWrite{ true };
    int m_uniformLocationModel{ -1 };
    int m_uniformLocationView{ -1 };
    int m_uniformLocationProjection{ -1 };
    int m_uniformLocationNormal{ -1 };
    int m_uniformLocationViewport{ -1 };
    int m_uniformLocationAmbientLight{ -1 };
    int m_uniformLocationLightPosType{ -1 };
    int m_uniformLocationLightColorRange{ -1 };
    friend class Mesh;
    friend class RenderPass;
    friend class Material;
};

} // namespace re

#endif //SIMPLERENDERENGINE_SHADER_H
