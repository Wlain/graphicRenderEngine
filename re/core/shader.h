//
// Created by william on 2022/5/22.
//

#ifndef SIMPLERENDERENGINE_SHADER_H
#define SIMPLERENDERENGINE_SHADER_H
#include "commonMacro.h"
#include "glCommonDefine.h"
#include "light.h"
#include "material.h"
#include "renderer.h"
#include "texture.h"

#include <glm/gtc/color_space.hpp>
#include <map>
#include <set>
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
        IVec4,
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

    enum class ShaderType
    {
        Vertex,
        Fragment,
        Geometry,
        TessellationControl,
        TessellationEvaluation
    };

    enum class ResourceType
    {
        File,
        Memory
    };

    struct Resource
    {
        ResourceType resourceType;
        std::string value;
    };

    enum class StencilFunc
    {
        Never = GL_NEVER,       // Never pass.
        Less = GL_LESS,         // <
        Equal = GL_EQUAL,       // =
        LEqual = GL_LEQUAL,     // <=
        Greater = GL_GREATER,   // >
        NotEqual = GL_NOTEQUAL, // !=
        GEqual = GL_GEQUAL,     // >=
        Always = GL_ALWAYS,     // Always pass.
        Disabled                // Stencil test disabled
    };

    enum class StencilOp
    {
        Keep = GL_KEEP,          // 保持当前值
        Zero = GL_ZERO,          // stencil buffer value设置为0
        Replace = GL_REPLACE,    // Sets the stencil buffer value to the reference value as specified by WebGLRenderingContext.stencilFunc().
        Incr = GL_INCR,          // Increments the current stencil buffer value. Clamps to the maximum representable unsigned value.
        IncrWrap = GL_INCR_WRAP, // Increments the current stencil buffer value. Wraps stencil buffer value to zero when incrementing the maximum representable unsigned value.
        Decr = GL_DECR,          // Decrements the current stencil buffer value. Clamps to 0.
        DecrWrap = GL_DECR_WRAP, // Decrements the current stencil buffer value. Wraps stencil buffer value to the maximum representable unsigned value when decrementing a stencil buffer value of 0.
        Invert = GL_INVERT,      // Inverts the current stencil buffer value bitwise.
    };

    struct Stencil
    {
        StencilFunc func = StencilFunc::Disabled; // Specifying the test function
        uint16_t ref;                             // Specifying the reference value for the stencil test. This value is clamped to the range 0 to 2n -1 where n is the number of bitplanes in the stencil buffer.
        uint16_t mask;                            // Specifying a bit-wise mask that is used to AND the reference value and the stored stencil value when the test is done.
        StencilOp fail = StencilOp::Keep;         // Specifying the function to use when the stencil test fails.
        StencilOp zfail = StencilOp::Keep;        // Specifying the function to use when the stencil test passes, but the depth test fails
        StencilOp zpass = StencilOp::Keep;        // Specifying the function to use when both the stencil test and the depth test pass, or when the stencil test passes and there is no depth buffer or depth testing is disabled
    };

    class ShaderBuilder
    {
    public:
        ShaderBuilder(const ShaderBuilder&) = default;
        DEPRECATED("Use ShaderType withSourceString() or withSourceFile() instead")
        ShaderBuilder& withSource(std::string_view vertexShader, std::string_view fragmentShader);
        ShaderBuilder& withSourceString(std::string_view shaderSource, ShaderType shaderType);
        ShaderBuilder& withSourceFile(std::string_view shaderFile, ShaderType shaderType);
        ShaderBuilder& withDepthTest(bool enable);
        ShaderBuilder& withDepthWrite(bool enable);
        ShaderBuilder& withColorWrite(glm::bvec4 enable);
        ShaderBuilder& withBlend(BlendType blendType);
        ShaderBuilder& withName(std::string_view name);
        ShaderBuilder& withStencil(Stencil stencil);
        ShaderBuilder& withOffset(float factor, float units); // 设置用于计算深度值的缩放比例和单位
        ShaderBuilder& withCullFace(CullFace face);
        std::shared_ptr<Shader> build();
        std::shared_ptr<Shader> build(std::vector<std::string>& errors);

    private:
        ShaderBuilder() = default;
        explicit ShaderBuilder(Shader* shader);

    private:
        std::map<ShaderType, Resource> m_shaderSources;
        std::map<std::string, std::string> m_specializationConstants;
        std::string m_name;
        BlendType m_blendType{ BlendType::Disabled };
        CullFace m_cullFace{ CullFace::Back };
        glm::vec2 m_offset = { 0.0f, 0.0f };
        Shader* updateShader{ nullptr };
        glm::bvec4 m_colorWrite{ true, true, true, true };
        Stencil m_stencil{};
        bool m_depthTest{ true };
        bool m_depthWrite{ true };
        friend class Shader;
    };

public:
    static ShaderBuilder create();
    // Must end with build()
    ShaderBuilder update();
    static std::shared_ptr<Shader> getUnlit();
    static std::shared_ptr<Shader> getBlit();
    static std::shared_ptr<Shader> getUnlitSprite();
    static std::shared_ptr<Shader> getStandardParticles();
    static std::shared_ptr<Shader> getStandardPBR();
    static std::shared_ptr<Shader> getStandardBlinnPhong();
    static std::shared_ptr<Shader> getStandardPhong();

private:
    static std::string getSource(const Resource& resource);

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
    inline const Stencil& getStencil() const { return m_stencil; }
    inline const glm::bvec4& colorWriteMask() const { return m_colorWrite; }

    std::vector<std::string> getAttributeNames();
    std::vector<std::string> getUniformNames();

    // 验证网格属性。如果无效，则将 info 变量设置为错误消息。此方法只应用于debug
    bool validateMesh(Mesh* mesh, std::string& info);
    const std::map<std::string, std::string>& getCurrentSpecializationConstants() const;
    std::set<std::string> getAllSpecializationConstants();
    std::shared_ptr<Material> createMaterial(std::map<std::string, std::string> specializationConstants = {});
    const char* toStr(UniformType u);
    uint32_t toId(ShaderType st);

private:
    Shader();
    void bind();
    bool build(const std::map<ShaderType, Resource>& shaderSources, std::vector<std::string>& errors);
    std::string precompile(std::string source, std::vector<std::string>& errors, uint32_t shaderType);
    std::string insertPreprocessorDefines(std::string source, std::map<std::string, std::string>& specializationConstants, uint32_t shaderType);
    bool setLights(WorldLights* worldLights) const;
    void updateUniformsAndAttributes();
    bool compileShader(const Resource& resource, GLenum type, GLuint& shader, std::vector<std::string>& errors);

private:
    inline static std::shared_ptr<Shader> s_unlit{ nullptr }; // 无灯光
    inline static std::shared_ptr<Shader> s_blit{ nullptr };
    inline static std::shared_ptr<Shader> s_unlitSprite{ nullptr };
    inline static std::shared_ptr<Shader> s_debugUV{ nullptr };
    inline static std::shared_ptr<Shader> s_debugNormals{ nullptr };
    inline static std::shared_ptr<Shader> s_font{ nullptr };
    inline static std::shared_ptr<Shader> s_standardParticles{ nullptr };
    inline static std::shared_ptr<Shader> s_standardPBR{ nullptr };
    inline static std::shared_ptr<Shader> s_standardPhong{ nullptr };
    inline static std::shared_ptr<Shader> s_standardBlinnPhong{ nullptr };
    inline static long s_globalShaderCounter{ 0 };

private:
    BlendType m_blendType{ BlendType::Disabled };
    CullFace m_cullFace{ CullFace::Back };
    std::vector<Uniform> m_uniforms;
    std::map<std::string, ShaderAttribute> m_attributes;
    std::map<std::string, std::string> m_specializationConstants;
    std::vector<std::weak_ptr<Shader>> m_specializations;
    std::shared_ptr<Shader> m_parent = nullptr;
    std::map<ShaderType, Resource> m_shaderSources;
    glm::vec2 m_offset = glm::vec2(0, 0);
    std::string m_name;
    Stencil m_stencil{};
    glm::bvec4 m_colorWrite{ true, true, true, true };
    unsigned int m_id{ 0 };
    bool m_depthTest{ true };
    bool m_depthWrite{ true };
    long m_shaderUniqueId{ 0 };
    int m_uniformLocationModel{ -1 };
    int m_uniformLocationView{ -1 };
    int m_uniformLocationProjection{ -1 };
    int m_uniformLocationModelViewInverseTranspose{ -1 };
    int m_uniformLocationModelInverseTranspose{ -1 };
    int m_uniformLocationViewport{ -1 };
    int m_uniformLocationCameraPosition{ -1 };
    int m_uniformLocationAmbientLight{ -1 };
    int m_uniformLocationLightPosType{ -1 };
    int m_uniformLocationLightColorRange{ -1 };
    friend class Mesh;
    friend class RenderPass;
    friend class Material;
    friend class Inspector;
};
} // namespace re

#endif //SIMPLERENDERENGINE_SHADER_H
