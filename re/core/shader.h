//
// Created by william on 2022/5/22.
//

#ifndef SIMPLERENDERENGINE_SHADER_H
#define SIMPLERENDERENGINE_SHADER_H
#include "light.h"
#include "texture.h"

#include <map>
#include <string>
#include <vector>
namespace re
{
class Texture;
class WorldLights;
class Material;
class Mesh;
class Shader : public std::enable_shared_from_this<Shader>
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
        Vec3,
        Texture,
        Invalid,
        TextureCube
    };

    enum class CullFace
    {
        Front,
        Back,
        None
    };

    struct Uniform
    {
        Uniform(const char* _name, int32_t _location, UniformType _type, int32_t _count) :
            name(_name), id(_location), type(_type), arraySize(_count) {}
        Uniform() = default;
        std::string name;
        int32_t id{ -1 };
        UniformType type{ UniformType::Invalid };
        // 1 means not array
        int32_t arraySize{ -1 };
    };

    struct ShaderAttribute
    {
        int32_t position;
        unsigned int type;
        int32_t arraySize;
    };

    class ShaderBuilder
    {
    public:
        ShaderBuilder(const ShaderBuilder&) = delete;
        ShaderBuilder& withSource(std::string_view vertexShader, std::string_view fragmentShader);
        ShaderBuilder& withSourceStandard();
        ShaderBuilder& withSourceUnlit();
        ShaderBuilder& withSourceUnlitSprite();
        ShaderBuilder& withSourceStandardParticles();
        ShaderBuilder& withDepthTest(bool enable);
        ShaderBuilder& withDepthWrite(bool enable);
        ShaderBuilder& withBlend(BlendType blendType);
        ShaderBuilder& withName(std::string_view name);
        ShaderBuilder& withOffset(float factor, float units); // 设置用于计算深度值的缩放比例和单位
        ShaderBuilder& withCullFace(CullFace face);
        std::shared_ptr<Shader> build();

    private:
        ShaderBuilder() = default;

    private:
        std::string m_vertexShaderStr;
        std::string m_fragmentShaderStr;
        std::string m_name;
        BlendType m_blendType{ BlendType::Disabled };
        CullFace m_cullFace{ CullFace::Back };
        glm::vec2 m_offset = { 0, 0 };
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
    static std::shared_ptr<Shader> getUnlit();
    // Attributes
    // "color" vec4 (default (1,1,1,1))
    // "tex" Texture* (default white texture)
    static std::shared_ptr<Shader> getUnlitSprite();
    /// Phong Light Model. Uses light objects and ambient light set in simpleRenderEngine.
    // Attributes
    // "color" vec4 (default (1,1,1,1))
    // "tex" Texture* (default white texture)
    // "specularity" float (default 0 = no specularity)
    static std::shared_ptr<Shader> getStandard();
    // StandardParticles
    // Attributes
    // "tex" Texture* (default alpha sphere texture)
    static std::shared_ptr<Shader> getStandardParticles();

public:
    ~Shader();
    Uniform getUniformType(std::string_view name);
    //return type sizeof attribute
    std::pair<int, int> getAttributeType(const std::string& name);
    inline bool isDepthTest() const { return m_depthTest; }
    inline bool isDepthWrite() const { return m_depthWrite; }
    inline BlendType getBlend() const { return m_blendType; }
    inline const glm::vec2& getOffset() const { return m_offset; }
    inline const std::string& name() const { return m_name; }

    std::vector<std::string> getAttributeNames();
    std::vector<std::string> getUniformNames();

    // 验证网格属性。如果无效，则将 info 变量设置为错误消息。此方法只应用于debug
    bool validateMesh(Mesh* mesh, std::string& info);
    std::shared_ptr<Material> createMaterial();
    const char* c_str(UniformType u);

private:
    Shader();
    bool build(std::string_view vertexShader, std::string_view fragmentShader);
    void bind();
    bool setLights(WorldLights* worldLights, const glm::mat4& viewTransform);
    void updateUniformsAndAttributes();

private:
    inline static std::shared_ptr<Shader> s_unlit{ nullptr }; // 无灯光
    inline static std::shared_ptr<Shader> s_unlitSprite{ nullptr };
    inline static std::shared_ptr<Shader> s_debugUV{ nullptr };
    inline static std::shared_ptr<Shader> s_debugNormals{ nullptr };
    inline static std::shared_ptr<Shader> s_standard{ nullptr };
    inline static std::shared_ptr<Shader> s_font{ nullptr };
    inline static std::shared_ptr<Shader> s_standardParticles{ nullptr };

private:
    BlendType m_blendType{ BlendType::Disabled };
    CullFace m_cullFace{ CullFace::Back };
    std::vector<Uniform> m_uniforms;
    std::map<std::string, ShaderAttribute> m_attributes;
    glm::vec2 m_offset = glm::vec2(0, 0);
    std::string m_name;
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
    friend class Profiler;
};

} // namespace re

#endif //SIMPLERENDERENGINE_SHADER_H
