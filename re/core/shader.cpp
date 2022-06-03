//
// Created by william on 2022/5/22.
//

#include "shader.h"

#include "commonMacro.h"
#include "glCommonDefine.h"
#include "material.h"
#include "renderer.h"
#include "texture.h"
#include "worldLights.h"

#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <string>
#include <vector>

namespace re
{
// 匿名命名空间,变量声明对其他文件中的代码不可见
namespace
{
void logCurrentCompileException(GLuint shader, GLenum type)
{
    GLint logSize = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize);
    std::vector<char> errorLog((unsigned long)logSize);
    glGetShaderInfoLog(shader, logSize, &logSize, errorLog.data());
    std::string typeStr;
    switch (type)
    {
    case GL_FRAGMENT_SHADER:
        typeStr = "Fragment shader";
        break;
    case GL_VERTEX_SHADER:
        typeStr = "Vertex shader";
        break;
    default:
        typeStr = std::string("Unknown error type: ") + std::to_string(type);
        break;
    }
    LOG_ERROR("{}\n, {} error", errorLog.data(), typeStr);
}

bool compileShader(std::string_view source, GLenum type, GLuint& shader)
{
    shader = glCreateShader(type);
    auto stringPtr = source.data();
    auto length = (GLint)strlen(stringPtr);
    glShaderSource(shader, 1, &stringPtr, &length);
    glCompileShader(shader);
    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE)
    {
        logCurrentCompileException(shader, type);
        return false;
    }
    return true;
}

bool linkProgram(GLuint shaderProgram)
{
    glBindFragDataLocation(shaderProgram, 0, "fragColor");
    glLinkProgram(shaderProgram);
    GLint linked;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linked);
    if (linked == GL_FALSE)
    {
        GLint logSize;
        glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &logSize);
        std::vector<char> errorLog((size_t)logSize);
        glGetProgramInfoLog(shaderProgram, logSize, nullptr, errorLog.data());
        LOG_ERROR("{}", errorLog.data());
        return false;
    }
    return true;
}
} // namespace

Shader::ShaderBuilder& Shader::ShaderBuilder::withSource(std::string_view vertexShader, std::string_view fragmentShader)
{
    m_vertexShaderStr = vertexShader;
    m_fragmentShaderStr = fragmentShader;
    return *this;
}

Shader::ShaderBuilder& Shader::ShaderBuilder::withSourceStandard()
{
    m_vertexShaderStr = R"(#version 330
        in vec3 position;
        in vec3 normal;
        in vec4 uv;
        out vec3 vNormal;
        out vec2 vUV;
        out vec3 vEyePos;

        uniform mat4 g_model;
        uniform mat4 g_view;
        uniform mat4 g_projection;
        uniform mat3 g_normalMat;

        void main(void) {
            vec4 eyePos = g_view * g_model * vec4(position, 1.0);
            vEyePos = eyePos.xyz;
            vNormal = normalize(g_normalMat * normal);
            vUV = uv.xy;
            gl_Position = g_projection * eyePos;
        }
    )";
    m_fragmentShaderStr = R"(#version 330
        out vec4 fragColor;
        in vec3 vNormal;
        in vec2 vUV;
        in vec3 vEyePos;

        uniform vec3 g_ambientLight;
        uniform vec4 color;
        uniform sampler2D tex;

        uniform vec4 g_lightPosType[4];
        uniform vec4 g_lightColorRange[4];
        uniform float specularity;

        vec3 computeLight()
        {
            vec3 lightColor = g_ambientLight.xyz;
            vec3 normal = normalize(vNormal);

            for (int i = 0; i < 4; i++)
            {
                bool isDirectional = g_lightPosType[i].w == 0.0;
                bool isPoint       = g_lightPosType[i].w == 1.0;
                vec3 lightDirection;
                float att = 1.0;
                if (isDirectional)
                {
                    lightDirection = normalize(g_lightPosType[i].xyz);
                }
                else if (isPoint)
                {
                    vec3 lightVector = g_lightPosType[i].xyz - vEyePos;
                    float lightRange = g_lightColorRange[i].w;
                    float lightVectorLength = length(lightVector);
                    lightDirection = lightVector/lightVectorLength;
                    if (lightRange <= 0.0)
                    {
                        att = 1.0;
                    } else if (lightVectorLength >= lightRange)
                    {
                        att = 0.0;
                    } else
                    {
                        att = pow(1.0-lightVectorLength/lightRange,1.5); // non physical range based attenuation
                    }
                }
                else
                {
                    continue;
                }
                // diffuse light
                float thisDiffuse = max(0.0,dot(lightDirection, normal));
                if (thisDiffuse > 0.0)
                {
                   lightColor += (att * thisDiffuse) * g_lightColorRange[i].xyz;
                }
                // specular light
                if (specularity > 0)
                {
                    vec3 H = normalize(lightDirection - normalize(vEyePos));
                    float nDotHV = dot(normal, H);
                    if (nDotHV > 0)
                    {
                        float pf = pow(nDotHV, specularity);
                        lightColor += vec3(att * pf); // white specular highlights
                    }
               }
            }
            return lightColor;
        }

        void main(void)
        {
            vec4 c = color * texture(tex, vUV);
            vec3 light = computeLight();
            fragColor = c * vec4(light, 1.0);
        }
    )";
    return *this;
}

Shader::ShaderBuilder& Shader::ShaderBuilder::withSourceUnlit()
{
    m_vertexShaderStr = R"(#version 330
        in vec3 position;
        in vec3 normal;
        in vec4 uv;
        out vec2 vUV;

        uniform mat4 g_model;
        uniform mat4 g_view;
        uniform mat4 g_projection;

        void main(void) {
            vUV = uv.xy;
            gl_Position = g_projection * g_view * g_model * vec4(position, 1.0);
        }
    )";
    m_fragmentShaderStr = R"(#version 330
        in vec2 vUV;
        out vec4 fragColor;
        uniform vec4 color;
        uniform sampler2D tex;
        void main()
        {
            fragColor = color * texture(tex, vUV);
        }
    )";
    return *this;
}

Shader::ShaderBuilder& Shader::ShaderBuilder::withSourceUnlitSprite()
{
    m_vertexShaderStr = R"(#version 330
        in vec3 position;
        in vec3 normal;
        in vec4 uv;
        in vec4 color;
        out vec2 vUV;
        out vec4 vColor;

        uniform mat4 g_model;
        uniform mat4 g_view;
        uniform mat4 g_projection;

        void main(void)
        {
            gl_Position = g_projection * g_view * g_model * vec4(position, 1.0);
            vUV = uv.xy;
            vColor = color;
        }
        )";
    m_fragmentShaderStr = R"(#version 330
        out vec4 fragColor;
        in vec2 vUV;
        in vec4 vColor;

        uniform sampler2D tex;

        void main(void)
        {
            fragColor = vColor * texture(tex, vUV);
        }
    )";
    return *this;
}

// 这个着色器中使用的粒子大小取决于屏幕大小的高度(使粒子分辨率独立)
// 对于透视投影，在高度为600的视窗中，粒子的大小以屏幕空间大小定义，距离为1.0。
// 对于正交影图，粒子的大小在高度为600的视窗中以屏幕空间大小定义。
Shader::ShaderBuilder& Shader::ShaderBuilder::withSourceStandardParticles()
{
    m_vertexShaderStr = R"(#version 330
        in vec3 position;
        in vec4 uv;
        in vec4 color;
        in float particleSize;
        out mat3 vUVMat;
        out vec4 vColor;
        out vec3 uvSize;

        uniform mat4 g_model;
        uniform mat4 g_view;
        uniform mat4 g_projection;
        uniform vec4 g_viewport;

        mat3 translate(vec2 p)
        {
         return mat3(1.0,0.0,0.0,
                    0.0,1.0,0.0,
                    p.x,p.y,1.0);
        }

        mat3 rotate(float rad)
        {
          float s = sin(rad);
          float c = cos(rad);
          return mat3(c,s,0.0,
                    -s,c,0.0,
                    0.0,0.0,1.0);
        }

        mat3 scale(float s)
        {
          return mat3(s,0.0,0.0,
                     0.0,s,0.0,
                     0.0,0.0,1.0);
        }

        void main(void) {
            vec4 pos = vec4(position, 1.0);
            vec4 eyeSpacePos = g_view * g_model * pos;
            gl_Position = g_projection * g_view * g_model * pos;
            if (g_projection[2][3] != 0){ // if perspective projection
                 gl_PointSize = ((g_viewport.y / 600.0) * particleSize * 1.0 / -eyeSpacePos.z);
            }
            else
            {
                gl_PointSize = particleSize*(g_viewport.y / 600.0);
            }
            vUVMat = translate(uv.xy)*scale(uv.z) * translate(vec2(0.5,0.5))*rotate(uv.w) * translate(vec2(-0.5,-0.5));
            vColor = color;
            uvSize = uv.xyz;
        }
    )";
    m_fragmentShaderStr = R"(#version 330
        out vec4 fragColor;
        in mat3 vUVMat;
        in vec4 vColor;
        in vec3 uvSize;

        uniform sampler2D tex;
        void main(void)
        {
            vec2 uv = (vUVMat * vec3(gl_PointCoord,1.0)).xy;
            if (uv != clamp(uv, uvSize.xy, uvSize.xy + uvSize.zz))
            {
                discard;
            }
            vec4 c = vColor * texture(tex, uv);
            fragColor = c;
        }
    )";
    return *this;
}
Shader::ShaderBuilder& Shader::ShaderBuilder::withDepthTest(bool enable)
{
    m_depthTest = enable;
    return *this;
}

Shader::ShaderBuilder& Shader::ShaderBuilder::withDepthWrite(bool enable)
{
    m_depthWrite = enable;
    return *this;
}

Shader::ShaderBuilder& Shader::ShaderBuilder::withBlend(Shader::BlendType blendType)
{
    m_blendType = blendType;
    return *this;
}

Shader::ShaderBuilder& Shader::ShaderBuilder::withName(std::string_view name)
{
    m_name = name;
    return *this;
}

std::shared_ptr<Shader> Shader::ShaderBuilder::build()
{
    if (m_name.empty())
    {
        m_name = "Unnamed shader";
    }
    auto* shader = new Shader();
    if (!shader->build(m_vertexShaderStr, m_fragmentShaderStr))
    {
        delete shader;
        return {};
    }
    shader->m_cullFace = m_cullFace;
    shader->m_depthTest = m_depthTest;
    shader->m_depthWrite = m_depthWrite;
    shader->m_blendType = m_blendType;
    shader->m_name = m_name;
    shader->m_offset = m_offset;
    return std::shared_ptr<Shader>(shader);
}

Shader::ShaderBuilder& Shader::ShaderBuilder::withOffset(float factor, float units)
{
    m_offset.x = factor;
    m_offset.y = units;
    return *this;
}

Shader::ShaderBuilder& Shader::ShaderBuilder::withCullFace(Shader::CullFace face)
{
    m_cullFace = face;
    return *this;
}

std::shared_ptr<Shader> Shader::getUnlit()
{
    if (s_unlit != nullptr)
    {
        return s_unlit;
    }
    s_unlit = Shader::create()
                  .withSourceUnlit()
                  .withName("Unlit")
                  .build();
    return s_unlit;
}

std::shared_ptr<Shader> Shader::getUnlitSprite()
{
    if (s_unlitSprite != nullptr)
    {
        return s_unlitSprite;
    }

    s_unlitSprite = Shader::create()
                        .withSourceUnlitSprite()
                        .withBlend(BlendType::AlphaBlending)
                        .withDepthWrite(false)
                        .withName("Unlit Sprite")
                        .build();
    return s_unlitSprite;
}

std::shared_ptr<Shader> Shader::getStandard()
{
    if (s_standard != nullptr)
    {
        return s_standard;
    }
    s_standard = Shader::create()
                     .withSourceStandard()
                     .withName("Standard")
                     .build();
    return s_standard;
}

std::shared_ptr<Shader> Shader::getStandardParticles()
{
    if (s_standardParticles != nullptr)
    {
        return s_standardParticles;
    }
    s_standardParticles = Shader::create()
                              .withSourceStandardParticles()
                              .withBlend(BlendType::AdditiveBlending)
                              .withDepthWrite(false)
                              .withName("Standard Particles")
                              .build();
    return s_standardParticles;
}

Shader::Shader()
{
    if (!Renderer::s_instance)
    {
        LOG_FATAL("Cannot instantiate re::Shader before re::Renderer is created.");
    }
    m_id = glCreateProgram();
    Renderer::s_instance->m_renderStatsCurrent.shaderCount++;
    Renderer::s_instance->m_shaders.emplace_back(this);
}

Shader::~Shader()
{
    auto* r = Renderer::s_instance;
    if (r != nullptr)
    {
        r->m_renderStatsCurrent.shaderCount--;
        r->m_shaders.erase(std::remove(r->m_shaders.begin(), r->m_shaders.end(), this));
        glDeleteShader(m_id);
    }
}

bool Shader::setLights(WorldLights* worldLights, const glm::mat4& viewTransform)
{
    if (worldLights == nullptr)
    {
        glUniform3f(m_uniformLocationAmbientLight, 0, 0, 0);
        static float noLight[4 * 4] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
        glUniform4fv(m_uniformLocationLightPosType, 4, noLight);
        glUniform4fv(m_uniformLocationLightColorRange, 4, noLight);
        return false;
    }
    if (m_uniformLocationAmbientLight != -1)
    {
        glUniform3fv(m_uniformLocationAmbientLight, 1, glm::value_ptr(worldLights->ambientLight));
    }
    if (m_uniformLocationLightPosType != -1 && m_uniformLocationLightColorRange != -1)
    {
        glm::vec4 lightPosType[4];
        glm::vec4 lightColorRange[4];
        for (int i = 0; i < 4; i++)
        {
            auto light = worldLights->getLight(i);
            if (light == nullptr || light->type == Light::Type::Unused)
            {
                lightPosType[i] = glm::vec4(0.0f, 0.0f, 0.0f, 2);
                continue;
            }
            else if (light->type == Light::Type::Directional)
            {
                lightPosType[i] = glm::vec4(light->direction, 0);
            }
            else if (light->type == Light::Type::Point)
            {
                lightPosType[i] = glm::vec4(light->position, 1);
            }
            // transform to eye space
            lightPosType[i] = viewTransform * lightPosType[i];
            lightColorRange[i] = glm::vec4(light->color, light->range);
        }
        if (m_uniformLocationLightPosType != -1)
        {
            glUniform4fv(m_uniformLocationLightPosType, 4, glm::value_ptr(lightPosType[0]));
        }
        if (m_uniformLocationLightColorRange != -1)
        {
            glUniform4fv(m_uniformLocationLightColorRange, 4, glm::value_ptr(lightColorRange[0]));
        }
    }
    return true;
}

void Shader::bind()
{
    glUseProgram(m_id);
    if (m_depthWrite)
    {
        glEnable(GL_DEPTH_TEST);
    }
    else
    {
        glDisable(GL_DEPTH_TEST);
    }
    if (m_cullFace == CullFace::None)
    {
        glDisable(GL_CULL_FACE);
    }
    else
    {
        glEnable(GL_CULL_FACE);
        glCullFace(m_cullFace == CullFace::Back ? GL_BACK : GL_FRONT);
    }
    glDepthMask(m_depthWrite ? GL_TRUE : GL_FALSE);
    switch (m_blendType)
    {
    case BlendType::Disabled:
        glDisable(GL_BLEND);
        break;
    case BlendType::AlphaBlending:
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        break;
    case BlendType::AdditiveBlending:
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        break;
    default:
        LOG_ERROR("Invalid blend value - was {}", (int)m_blendType);
        break;
    }
    // 第一个参数代表：即将绘制的片元因子，也就是源因子
    // 第二个参数代表：framebuffer里面存储的颜色，也就是目标因子
    // glEnable(GL_BLEND);
    // glBlendFunc(GL_ZERO, GL_ONE); // 此时，代表全部用framebuffer里存储的颜色,反之同理

    if (m_offset.x == 0 && m_offset.y == 0)
    {
        glDisable(GL_POLYGON_OFFSET_FILL);
        glDisable(GL_POLYGON_OFFSET_LINE);
        glDisable(GL_POLYGON_OFFSET_POINT);
    }
    else
    {
        glEnable(GL_POLYGON_OFFSET_FILL);
        glEnable(GL_POLYGON_OFFSET_LINE);
        glEnable(GL_POLYGON_OFFSET_POINT);
        glPolygonOffset(m_offset.x, m_offset.y);
    }
}

Shader::Uniform Shader::getUniformType(std::string_view name)
{
    for (const auto& u : m_uniforms)
    {
        if (u.name == name)
            return u;
    }
    return {};
}

void Shader::updateUniformsAndAttributes()
{
    // update uniforms
    m_uniformLocationModel = -1;
    m_uniformLocationView = -1;
    m_uniformLocationProjection = -1;
    m_uniformLocationNormal = -1;
    m_uniformLocationViewport = -1;
    m_uniformLocationAmbientLight = -1;
    m_uniformLocationLightPosType = -1;
    m_uniformLocationLightColorRange = -1;
    m_uniforms.clear();
    GLint uniformCount;
    glGetProgramiv(m_id, GL_ACTIVE_UNIFORMS, &uniformCount);
    UniformType uniformType = UniformType::Invalid;
    for (int i = 0; i < uniformCount; i++)
    {
        const int nameSize = 50;
        GLchar name[nameSize];
        GLsizei nameLength;
        GLint size;
        GLenum type;
        glGetActiveUniform(m_id, i, nameSize, &nameLength, &size, &type, name);
        switch (type)
        {
        case GL_FLOAT:
            uniformType = UniformType::Float;
            break;
        case GL_FLOAT_VEC4:
            uniformType = UniformType::Vec4;
            break;
        case GL_FLOAT_VEC3:
            uniformType = UniformType::Vec3;
            break;
        case GL_INT:
            uniformType = UniformType::Int;
            break;
        case GL_FLOAT_MAT3:
            uniformType = UniformType::Mat3;
            break;
        case GL_FLOAT_MAT4:
            uniformType = UniformType::Mat4;
            break;
        case GL_SAMPLER_2D:
            uniformType = UniformType::Texture;
            break;
        case GL_SAMPLER_CUBE:
            uniformType = UniformType::TextureCube;
            break;
        default:
            LOG_ERROR("Unsupported shader type {}, name {}", type, name);
        }
        // remove [0] if exists
        char* bracketIndex = strchr(name, '[');
        if (bracketIndex != nullptr)
        {
            *bracketIndex = '\0';
        }
        bool isGlobalUniform = name[0] == 'g' && name[1] == '_';
        GLint location = glGetUniformLocation(m_id, name);
        if (!isGlobalUniform)
        {
            m_uniforms.emplace_back(name, location, uniformType, size);
        }
        else
        {
            if (strcmp(name, "g_model") == 0)
            {
                if (uniformType == UniformType::Mat4)
                {
                    m_uniformLocationModel = location;
                }
                else
                {
                    LOG_ERROR("Invalid g_model uniform type. Expected mat4.was {}", c_str(uniformType));
                }
            }
            if (strcmp(name, "g_view") == 0)
            {
                if (uniformType == UniformType::Mat4)
                {
                    m_uniformLocationView = location;
                }
                else
                {
                    LOG_ERROR("Invalid g_view uniform type. Expected mat4.was {}", c_str(uniformType));
                }
            }
            if (strcmp(name, "g_projection") == 0)
            {
                if (uniformType == UniformType::Mat4)
                {
                    m_uniformLocationProjection = location;
                }
                else
                {
                    LOG_ERROR("Invalid g_projection uniform type. Expected mat4.was {}", c_str(uniformType));
                }
            }
            if (strcmp(name, "g_normalMat") == 0)
            {
                if (uniformType == UniformType::Mat3)
                {
                    m_uniformLocationNormal = location;
                }
                else
                {
                    LOG_ERROR("Invalid g_normal uniform type. Expected mat3.was {}", c_str(uniformType));
                }
            }
            if (strcmp(name, "g_viewport") == 0)
            {
                if (uniformType == UniformType::Vec4)
                {
                    m_uniformLocationViewport = location;
                }
                else
                {
                    LOG_ERROR("Invalid g_normal uniform type. Expected vec4.was {}", c_str(uniformType));
                }
            }
            if (strcmp(name, "g_ambientLight") == 0)
            {
                if (uniformType == UniformType::Vec3)
                {
                    m_uniformLocationAmbientLight = location;
                }
                else
                {
                    LOG_ERROR("Invalid g_ambientLight uniform type. Expected vec3.was {}", c_str(uniformType));
                }
            }
            if (strcmp(name, "g_lightPosType") == 0)
            {
                if (uniformType == UniformType::Vec4 && size == 4)
                {
                    m_uniformLocationLightPosType = location;
                }
                else
                {
                    LOG_ERROR("Invalid g_lightPosType uniform type. Expected vec4[4].was {}", c_str(uniformType));
                }
            }
            if (strcmp(name, "g_lightColorRange") == 0)
            {
                if (uniformType == UniformType::Vec4 && size == 4)
                {
                    m_uniformLocationLightColorRange = location;
                }
                else
                {
                    LOG_ERROR("Invalid g_lightPosType uniform type. Expected vec4[4].was {}", c_str(uniformType));
                }
            }
        }
    }

    // update attributes
    m_attributes.clear();
    int attributeCount;
    glGetProgramiv(m_id, GL_ACTIVE_ATTRIBUTES, &attributeCount);

    for (int i = 0; i < attributeCount; ++i)
    {
        const int nameSize = 50;
        GLchar name[nameSize];
        GLsizei nameLength;
        GLint size;
        GLenum type;
        glGetActiveAttrib(m_id, i, nameSize, &nameLength, &size, &type, name);
        auto location = glGetAttribLocation(m_id, name);
        m_attributes[std::string(name)] = { location, type, size };
    }
}

Shader::ShaderBuilder Shader::create()
{
    return {};
}

bool Shader::build(std::string_view vertexShader, std::string_view fragmentShader)
{
    std::vector<std::string> shaderSrc{ vertexShader.data(), fragmentShader.data() };
    std::vector<GLenum> shaderTypes{ GL_VERTEX_SHADER, GL_FRAGMENT_SHADER };
    for (int i = 0; i < 2; i++)
    {
        GLuint s;
        auto ret = compileShader(shaderSrc[i], shaderTypes[i], s);
        if (!ret)
        {
            return false;
        }
        glAttachShader(m_id, s);
    }
    bool linked = linkProgram(m_id);
    if (!linked)
    {
        return false;
    }
    updateUniformsAndAttributes();
    return true;
}

std::vector<std::string> Shader::getAttributeNames()
{
    std::vector<std::string> names;
    for (auto& u : m_attributes)
    {
        names.push_back(u.first);
    }
    return names;
}

std::vector<std::string> Shader::getUniformNames()
{
    std::vector<std::string> names;
    for (auto& u : m_uniforms)
    {
        names.push_back(u.name);
    }
    return names;
}

size_t Texture::getDataSize() const
{
    int size = m_info.width * m_info.height * 4;
    if (m_info.generateMipmap)
    {
        size += (int)((1.0f / 3.0f) * size);
    }
    // six sides
    if (m_info.target == GL_TEXTURE_CUBE_MAP)
    {
        size *= 6;
    }
    return size;
}

bool Shader::validateMesh(Mesh* mesh, std::string& info)
{
    bool valid = true;
    for (auto& shaderVertexAttribute : m_attributes)
    {
        auto meshType = mesh->getType(shaderVertexAttribute.first);
        if (meshType.first == -1)
        {
            valid = false;
            info += "Cannot find vertex attribute '" + shaderVertexAttribute.first + "' in mesh of type ";
            if (shaderVertexAttribute.second.type == GL_FLOAT)
            {
                info += "float";
            }
            else if (shaderVertexAttribute.second.type == GL_FLOAT_VEC2)
            {
                info += "vec2";
            }
            else if (shaderVertexAttribute.second.type == GL_FLOAT_VEC3)
            {
                info += "vec3";
            }
            else if (shaderVertexAttribute.second.type == GL_FLOAT_VEC4)
            {
                info += "vec4";
            }
            else if (shaderVertexAttribute.second.type == GL_INT_VEC4)
            {
                info += "ivec4";
            }
            info += "\n";
        }
        else
        {
        }
    }
    return valid;
}

std::shared_ptr<Material> Shader::createMaterial()
{
    return std::make_shared<Material>(shared_from_this());
}

const char* Shader::c_str(Shader::UniformType u)
{
    switch (u)
    {
    case UniformType::Int:
        return "int";
    case UniformType::Float:
        return "float";
    case UniformType::Mat3:
        return "mat3";
    case UniformType::Mat4:
        return "mat4";
    case UniformType::Vec3:
        return "vec3";
    case UniformType::Vec4:
        return "vec4";
    case UniformType::Texture:
        return "texture";
    case UniformType::TextureCube:
        return "textureCube";
    case UniformType::Invalid:
        return "invalid";
    default:
        return "unknown";
    }
}

std::pair<int, int> Shader::getAttributeType(const std::string& name)
{
    auto res = m_attributes[name];
    return { res.type, res.arraySize };
}

} // namespace re