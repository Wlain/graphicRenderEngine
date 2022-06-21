//
// Created by william on 2022/5/22.
//

#include "shader.h"

#include "commonMacro.h"
#include "glCommonDefine.h"
#include "material.h"
#include "renderer.h"
#include "texture.h"
#include "utils/utils.h"
#include "worldLights.h"

#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <regex>
#include <string>
#include <vector>

namespace re
{
// 匿名命名空间,变量声明对其他文件中的代码不可见
namespace
{
const std::regex SPECIALIZATION_CONSTANT_PATTERN("(S_[A-Z_0-9]+)");

// 容器对比
template <typename Map>
bool mapCompare(Map const& lhs, Map const& rhs)
{
    return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

// 分割
template <typename Out>
void split(const std::string& s, char delim, Out result)
{
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim))
    {
        *(result++) = item;
    }
}

// 分割
std::vector<std::string> split(const std::string& s, char delim)
{
    std::vector<std::string> elems;
    // std::back_inserter:在末尾插入元素。
    split(s, delim, std::back_inserter(elems));
    return elems;
}

// 正则替换，在shader里面实现include的功能
std::string pragmaInclude(std::string source, std::vector<std::string>& errors, uint32_t shaderType)
{
    if (source.find("#pragma include") == -1)
    {
        return source;
    }
    std::stringstream sstream;

    std::regex e(R"_(#pragma\s+include\s+"([^"]*)")_", std::regex::ECMAScript);
    int lineNumber = 0;
    std::vector<std::string> lines = split(source, '\n');
    int includes = 0;
    for (auto& s : lines)
    {
        lineNumber++;
        std::smatch m;
        if (std::regex_search(s, m, e))
        {
            std::string match = m[1];
            auto res = getFileContents(GET_EMBEDDED(match));
            if (res.empty())
            {
                errors.push_back(std::string("0:") + std::to_string(lineNumber) + " cannot find include file " + match + "##" + std::to_string(shaderType));
                sstream << s << "\n";
            }
            else
            {
                includes++;
                sstream << "#line " << (includes * 10000 + 1) << "\n";
                sstream << res << "\n";
                sstream << "#line " << lineNumber << "\n";
            }
        }
        else
        {
            sstream << s << "\n";
        }
    }
    return sstream.str();
}

// 获取当前shader编译信息
void logCurrentCompileInfo(GLuint& shader, GLenum type, std::vector<std::string>& errors, std::string_view name)
{
    GLint logSize = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize);
    if (logSize > 0)
    {
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
        case GL_GEOMETRY_SHADER:
            typeStr = "Geometry shader";
            break;
        case GL_TESS_CONTROL_SHADER:
            typeStr = "Tessellation control shader";
            break;
        case GL_TESS_EVALUATION_SHADER:
            typeStr = "Tessellation eval shader";
            break;
        default:
            typeStr = std::string("Unknown error type: ") + std::to_string(type);
            break;
        }
        LOG_ERROR("Shader compile error in {} {}: {}", name.data(), typeStr.c_str(), errorLog.data());
        errors.push_back(std::string(errorLog.data()) + "##" + std::to_string(type));
    }
}

bool linkProgram(GLuint shaderProgram, std::vector<std::string>& errors)
{
    glBindFragDataLocation(shaderProgram, 0, "fragColor");
    glLinkProgram(shaderProgram);
    GLint linked;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linked);
    if (linked == GL_FALSE)
    {
        GLint logSize;
        glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &logSize);
        if (logSize > 1)
        { // log size of 1 is empty, since it includes \0
            std::vector<char> errorLog((size_t)logSize);
            glGetProgramInfoLog(shaderProgram, logSize, nullptr, errorLog.data());
            errors.emplace_back(errorLog.data());
            LOG_ERROR("{}", errorLog.data());
        }
        return false;
    }
    return true;
}
} // namespace

Shader::ShaderBuilder& Shader::ShaderBuilder::withSource(std::string_view vertexShader, std::string_view fragmentShader)
{
    withSourceString(vertexShader, ShaderType::Vertex);
    withSourceString(fragmentShader, ShaderType::Fragment);
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
    std::vector<std::string> errors;
    return build(errors);
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

Shader::ShaderBuilder& Shader::ShaderBuilder::withSourceString(std::string_view shaderSource, Shader::ShaderType shaderType)
{
    m_shaderSources[shaderType] = { ResourceType::Memory, shaderSource.data() };
    return *this;
}

Shader::ShaderBuilder& Shader::ShaderBuilder::withSourceFile(std::string_view shaderFile, Shader::ShaderType shaderType)
{
    m_shaderSources[shaderType] = { ResourceType::File, shaderFile.data() };
    return *this;
}

std::shared_ptr<Shader> Shader::ShaderBuilder::build(std::vector<std::string>& errors)
{
    std::shared_ptr<Shader> shader;
    if (updateShader)
    {
        shader = updateShader->shared_from_this();
    }
    else
    {
        if (m_name.empty())
        {
            m_name = "Unnamed shader";
        }
        shader = std::shared_ptr<Shader>(new Shader());
        shader->m_specializationConstants = m_specializationConstants;
    }
    bool compileSuccess = shader->build(m_shaderSources, errors);
    if (!compileSuccess)
    {
        if (!updateShader)
        {
            shader.reset();
        }
        return shader;
    }
    shader->m_cullFace = m_cullFace;
    shader->m_depthTest = m_depthTest;
    shader->m_depthWrite = m_depthWrite;
    shader->m_blendType = m_blendType;
    shader->m_name = m_name;
    shader->m_offset = m_offset;
    shader->m_shaderSources = m_shaderSources;
    shader->m_shaderUniqueId = ++s_globalShaderCounter;
    return shader;
}

Shader::ShaderBuilder::ShaderBuilder(Shader* shader) :
    updateShader(shader)
{
}

std::string Shader::getSource(const Shader::Resource& resource)
{
    std::string source = resource.value;
    if (resource.resourceType == ResourceType::File)
    {
        source = getFileContents(GET_EMBEDDED(source));
    }
    return source;
}

bool Shader::compileShader(const Shader::Resource& resource, GLenum type, GLuint& shader, std::vector<std::string>& errors)
{
    auto source = getSource(resource);
    std::string source_ = precompile(source, errors, type);
    shader = glCreateShader(type);
    const auto* stringPtr = source_.c_str();
    auto length = (GLint)source_.size();
    glShaderSource(shader, 1, &stringPtr, &length);
    glCompileShader(shader);
    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    logCurrentCompileInfo(shader, type, errors, resource.value);

    return success == 1;
}

std::shared_ptr<Shader> Shader::getUnlit()
{
    if (s_unlit != nullptr)
    {
        return s_unlit;
    }
    s_unlit = create()
                  .withSourceFile("unlit_vert.glsl", ShaderType::Vertex)
                  .withSourceFile("unlit_frag.glsl", ShaderType::Fragment)
                  .withName("Unlit")
                  .build();
    return s_unlit;
}

std::shared_ptr<Shader> Shader::getBlit()
{
    if (s_blit != nullptr)
    {
        return s_blit;
    }
    s_blit = create()
                 .withSourceFile("blit_vert.glsl", ShaderType::Vertex)
                 .withSourceFile("blit_frag.glsl", ShaderType::Fragment)
                 .withName("Blit")
                 .build();
    return s_blit;
}

std::shared_ptr<Shader> Shader::getUnlitSprite()
{
    if (s_unlitSprite != nullptr)
    {
        return s_unlitSprite;
    }

    s_unlitSprite = create()
                        .withSourceFile("sprite_vert.glsl", ShaderType::Vertex)
                        .withSourceFile("sprite_frag.glsl", ShaderType::Fragment)
                        .withBlend(BlendType::AlphaBlending)
                        .withDepthWrite(false)
                        .withName("Unlit Sprite")
                        .build();
    return s_unlitSprite;
}

std::shared_ptr<Shader>& Shader::getStandard()
{
    if (s_standardPhong != nullptr)
    {
        return s_standardPhong;
    }
    s_standardPhong = create()
                          .withSourceFile("standard_vert.glsl", ShaderType::Vertex)
                          .withSourceFile("standard_frag.glsl", ShaderType::Fragment)
                          .withName("Standard")
                          .build();
    return s_standardPhong;
}

std::shared_ptr<Shader> Shader::getStandardParticles()
{
    if (s_standardParticles != nullptr)
    {
        return s_standardParticles;
    }
    s_standardParticles = Shader::create()
                              .withSourceFile("particles_vert.glsl", ShaderType::Vertex)
                              .withSourceFile("particles_frag.glsl", ShaderType::Fragment)
                              .withBlend(BlendType::AdditiveBlending)
                              .withDepthWrite(false)
                              .withName("Standard Particles")
                              .build();
    return s_standardParticles;
}

Shader::Shader()
{
    auto* r = Renderer::s_instance;
    if (r == nullptr)
    {
        LOG_FATAL("Cannot instantiate re::Shader before sre::Renderer is created.");
    }
    r->m_renderStatsCurrent.shaderCount++;
    r->m_shaders.emplace_back(this);
}

Shader::~Shader()
{
    auto* r = Renderer::s_instance;
    if (r != nullptr)
    {
        r->m_renderStatsCurrent.shaderCount--;
        if (!r->m_shaders.empty())
        {
            r->m_shaders.erase(std::remove(r->m_shaders.begin(), r->m_shaders.end(), this));
        }
        glDeleteShader(m_id);
    }
}

bool Shader::setLights(WorldLights* worldLights)
{
    int maxSceneLights = Renderer::s_instance->getMaxSceneLights();
    if (worldLights == nullptr)
    {
        glUniform4f(m_uniformLocationAmbientLight, 0, 0, 0, 0);
        const int vec4Elements = 4;
        std::vector<float> noLight(maxSceneLights * 4, 0);
        glUniform4fv(m_uniformLocationLightPosType, maxSceneLights, noLight.data());
        glUniform4fv(m_uniformLocationLightColorRange, maxSceneLights, noLight.data());
        return false;
    }
    if (m_uniformLocationAmbientLight != -1)
    {
        glUniform3fv(m_uniformLocationAmbientLight, 1, glm::value_ptr(worldLights->ambientLight));
    }
    if (m_uniformLocationLightPosType != -1 && m_uniformLocationLightColorRange != -1)
    {
        std::vector<glm::vec4> lightPosType(maxSceneLights, glm::vec4{ 0 });
        std::vector<glm::vec4> lightColorRange(maxSceneLights, glm::vec4{ 0 });
        for (int i = 0; i < maxSceneLights; i++)
        {
            auto light = worldLights->getLight(i);
            if (light == nullptr || light->type == Light::Type::Unused)
            {
                lightPosType[i] = glm::vec4(0.0f, 0.0f, 0.0f, 2);
                continue;
            }
            else if (light->type == Light::Type::Directional)
            {
                lightPosType[i] = glm::vec4(glm::normalize(light->direction), 0);
            }
            else if (light->type == Light::Type::Point)
            {
                lightPosType[i] = glm::vec4(light->position, 1);
            }
            // transform to eye space
            lightPosType[i] = lightPosType[i];
            lightColorRange[i] = glm::vec4(light->color, light->range);
        }
        if (m_uniformLocationLightPosType != -1)
        {
            glUniform4fv(m_uniformLocationLightPosType, maxSceneLights, glm::value_ptr(lightPosType[0]));
        }
        if (m_uniformLocationLightColorRange != -1)
        {
            glUniform4fv(m_uniformLocationLightColorRange, maxSceneLights, glm::value_ptr(lightColorRange[0]));
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
    m_uniformLocationModelViewInverseTranspose = -1;
    m_uniformLocationModelInverseTranspose = -1;
    m_uniformLocationViewport = -1;
    m_uniformLocationAmbientLight = -1;
    m_uniformLocationLightPosType = -1;
    m_uniformLocationLightColorRange = -1;
    m_uniformLocationCameraPosition = -1;
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
                    LOG_ERROR("Invalid g_model uniform type. Expected mat4.was {}", toStr(uniformType));
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
                    LOG_ERROR("Invalid g_view uniform type. Expected mat4.was {}", toStr(uniformType));
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
                    LOG_ERROR("Invalid g_projection uniform type. Expected mat4.was {}", toStr(uniformType));
                }
            }
            if (strcmp(name, "g_model_it") == 0)
            {
                if (uniformType == UniformType::Mat3)
                {
                    m_uniformLocationModelInverseTranspose = location;
                }
                else
                {
                    LOG_ERROR("Invalid g_model_it uniform type. Expected mat3 - was {}.", toStr(uniformType));
                }
            }
            if (strcmp(name, "g_model_view_it") == 0)
            {
                if (uniformType == UniformType::Mat3)
                {
                    m_uniformLocationModelViewInverseTranspose = location;
                }
                else
                {
                    LOG_ERROR("Invalid g_model_view_it uniform type. Expected mat3.was {}", toStr(uniformType));
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
                    LOG_ERROR("Invalid g_viewport uniform type. Expected vec4.was {}", toStr(uniformType));
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
                    LOG_ERROR("Invalid g_ambientLight uniform type. Expected vec3.was {}", toStr(uniformType));
                }
            }
            if (strcmp(name, "g_lightPosType") == 0)
            {
                if (uniformType == UniformType::Vec4 && size == Renderer::s_instance->getMaxSceneLights())
                {
                    m_uniformLocationLightPosType = location;
                }
                else
                {
                    LOG_ERROR("Invalid g_lightPosType uniform type. Expected vec4[Renderer::s_maxSceneLights].was {}", toStr(uniformType));
                }
            }
            if (strcmp(name, "g_lightColorRange") == 0)
            {
                if (uniformType == UniformType::Vec4 && size == Renderer::s_instance->getMaxSceneLights())
                {
                    m_uniformLocationLightColorRange = location;
                }
                else
                {
                    LOG_ERROR("Invalid g_lightPosType uniform type. Expected vec4[Renderer::s_maxSceneLights].was {}", toStr(uniformType));
                }
            }
            if (strcmp(name, "g_cameraPos") == 0)
            {
                if (uniformType == UniformType::Vec4)
                {
                    m_uniformLocationCameraPosition = location;
                }
                else
                {
                    LOG_ERROR("Invalid g_cameraPos uniform type. Expected vec4 - was {}[{}].", toStr(uniformType), size);
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

bool Shader::build(std::map<ShaderType, Resource> shaderSources, std::vector<std::string>& errors)
{
    unsigned int oldShaderProgramId = m_id;
    m_id = glCreateProgram();
    assert(m_id != 0);
    for (const auto& shaderSource : shaderSources)
    {
        GLuint s;
        GLenum shader = toId(shaderSource.first);
        auto ret = compileShader(shaderSource.second, shader, s, errors);
        if (!ret)
        {
            glDeleteProgram(m_id);
            m_id = oldShaderProgramId;
            for (const auto& e : errors)
            {
                LOG_ERROR("{}", e.c_str());
            }
            return false;
        }
        glAttachShader(m_id, s);
    }
    bool linked = linkProgram(m_id, errors);
    if (!linked)
    {
        glDeleteProgram(m_id);
        m_id = oldShaderProgramId;
        for (const auto& e : errors)
        {
            LOG_ERROR("{}", e.c_str());
        }
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

std::shared_ptr<Material> Shader::createMaterial(std::map<std::string, std::string> specializationConstants)
{
    if (m_parent)
    {
        return m_parent->createMaterial(std::move(specializationConstants));
    }
    if (!specializationConstants.empty())
    {
        for (auto& s : m_specializations)
        {
            if (auto ptr = s.lock())
            {
                if (mapCompare(specializationConstants, ptr->m_specializationConstants))
                    return std::make_shared<Material>(ptr);
            }
        }
        // no specialization shader found
        auto res = Shader::ShaderBuilder();
        res.m_depthTest = m_depthTest;
        res.m_depthWrite = m_depthWrite;
        res.m_blendType = m_blendType;
        res.m_name = m_name;
        res.m_offset = m_offset;
        res.m_shaderSources = m_shaderSources;
        res.m_specializationConstants = std::move(specializationConstants);
        auto specializedShader = res.build();
        if (specializedShader == nullptr)
        {
            LOG_WARN("Cannot create specialized shader. Using shader without specialization.");
            return std::make_shared<Material>(shared_from_this());
        }
        specializedShader->m_parent = shared_from_this();
        m_specializations.push_back(std::weak_ptr<Shader>(specializedShader));
        return std::make_shared<Material>(specializedShader);
    }
    return std::make_shared<Material>(shared_from_this());
}

const char* Shader::toStr(Shader::UniformType u)
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

uint32_t Shader::toId(Shader::ShaderType st)
{
    switch (st)
    {
    case ShaderType::Vertex:
        return GL_VERTEX_SHADER;
    case ShaderType::Fragment:
        return GL_FRAGMENT_SHADER;
    case ShaderType::Geometry:
        return GL_GEOMETRY_SHADER;
    case ShaderType::TessellationEvaluation:
        return GL_TESS_EVALUATION_SHADER;
    case ShaderType::TessellationControl:
        return GL_TESS_CONTROL_SHADER;
    default:
        LOG_ERROR("Invalid shader type. Was %d", (int)st);
        ASSERT(0);
        return GL_VERTEX_SHADER;
    }
}

Shader::ShaderBuilder Shader::update()
{
    return Shader::ShaderBuilder(Shader::ShaderBuilder());
}

std::shared_ptr<Shader> Shader::getStandardPBR()
{
    if (s_standardPBR != nullptr)
    {
        return s_standardPBR;
    }
    s_standardPBR = create()
                        .withSourceFile("standard_pbr_vert.glsl", ShaderType::Vertex)
                        .withSourceFile("standard_pbr_frag.glsl", ShaderType::Fragment)
                        .withName("Standard")
                        .build();
    return s_standardPBR;
}

std::shared_ptr<Shader> Shader::getStandardBlinnPhong()
{
    if (s_standardBlinnPhong != nullptr)
    {
        return s_standardBlinnPhong;
    }
    s_standardBlinnPhong = Shader::create()
                               .withSourceFile("standard_blinn_phong_vert.glsl", ShaderType::Vertex)
                               .withSourceFile("standard_blinn_phong_frag.glsl", ShaderType::Fragment)
                               .withName("Standard Blinn Phong")
                               .build();
    return s_standardBlinnPhong;
}

std::shared_ptr<Shader> Shader::getStandardPhong()
{
    if (s_standardPhong != nullptr)
    {
        return s_standardPhong;
    }
    s_standardPhong = create()
                          .withSourceFile("standard_phong_vert.glsl", ShaderType::Vertex)
                          .withSourceFile("standard_phong_frag.glsl", ShaderType::Fragment)
                          .withName("StandardPhong")
                          .build();
    return s_standardPhong;
}

const std::map<std::string, std::string>& Shader::getCurrentSpecializationConstants() const
{
    return m_specializationConstants;
}

std::set<std::string> Shader::getAllSpecializationConstants()
{
    if (m_parent)
    {
        return m_parent->getAllSpecializationConstants();
    }
    std::set<std::string> res;
    for (auto& source : m_shaderSources)
    {
        std::string s = getSource(source.second);
        std::smatch m;
        while (std::regex_search(s, m, SPECIALIZATION_CONSTANT_PATTERN))
        {
            std::string match = m.str();
            res.insert(match);
            s = m.suffix();
        }
    }
    return res;
}

std::string Shader::precompile(std::string source, std::vector<std::string>& errors, uint32_t shaderType)
{
    // Replace includes with content
    // for each occurrence of #pragma include replace with substitute
    source = pragmaInclude(source, errors, shaderType);
    // Insert preprocessor define symbols
    source = insertPreprocessorDefines(source, m_specializationConstants, shaderType);
    return source;
}

std::string Shader::insertPreprocessorDefines(std::string source, std::map<std::string, std::string>& specializationConstants, uint32_t shaderType)
{
    LOG_INFO("before insertPreprocessorDefines:source is:\n {}", source.c_str());
    std::stringstream ss;
    ss << "#define SI_LIGHTS " << Renderer::s_instance->getMaxSceneLights() << "\n";
    // add shader type
    switch (shaderType)
    {
    case GL_FRAGMENT_SHADER:
        ss << "#define SI_FRAGMENT 1\n";
        break;
    case GL_VERTEX_SHADER:
        ss << "#define SI_VERTEX 1\n";
        break;
    case GL_GEOMETRY_SHADER:
        ss << "#define SI_GEOMETRY 1\n";
        break;
    case GL_TESS_CONTROL_SHADER:
        ss << "#define SI_TESS_CTRL 1\n";
        break;
    case GL_TESS_EVALUATION_SHADER:
        ss << "#define SI_TESS_EVAL 1\n";
        break;
    default:
        LOG_WARN("Unknown shader type");
        break;
    }
    for (auto& sc : specializationConstants)
    {
        ss << "#define " << sc.first << " " << sc.second << "\n";
    }
    if (Renderer::s_instance->renderInfo().useFramebufferSRGB)
    {
        ss << "#define SI_FRAMEBUFFER_SRGB 1\n";
    }
    if (Renderer::s_instance->renderInfo().supportTextureSamplerSRGB)
    {
        ss << "#define SI_TEX_SAMPLER_SRGB 1\n";
    }

    // If the shader contains any #version or #extension statements, the defines are added after them.
    auto version = static_cast<int>(source.rfind("#version"));
    auto extension = static_cast<int>(source.rfind("#extension"));
    auto last = std::max(version, extension);
    if (last == -1)
    {
        ss << "#line 1\n";
        return ss.str() + source;
    }
    auto insertPos = source.find('\n', last);
    int lines = 0;
    for (int i = 0; i < insertPos; i++)
    {
        if (source.at(i) == '\n')
        {
            lines++;
        }
    }
    ss << "#line " << (lines + 1) << "\n";
    LOG_INFO("after insertPreprocessorDefines:source is:\n {}", source.substr(0, insertPos + 1) + ss.str() + source.substr(insertPos + 1).c_str());
    return source.substr(0, insertPos + 1) +
        ss.str() +
        source.substr(insertPos + 1);
}
} // namespace re