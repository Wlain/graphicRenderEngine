//
// Created by william on 2022/5/22.
//

#include "shader.h"

#include "commonMacro.h"
#include "glCommonDefine.h"
#include "renderer.h"
#include "texture.h"

#include <algorithm>
#include <glm/gtc/type_ptr.hpp>
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

bool compileShader(const char* source, GLenum type, GLuint& shader)
{
    shader = glCreateShader(type);
    auto length = (GLint)strlen(source);
    glShaderSource(shader, 1, &source, &length);
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

Shader::ShaderBuilder& Shader::ShaderBuilder::withSource(const char* vertexShader, const char* fragmentShader)
{
    m_vertexShaderStr = vertexShader;
    m_fragmentShaderStr = fragmentShader;
    return *this;
}

Shader::ShaderBuilder& Shader::ShaderBuilder::withSourceStandard()
{
    m_vertexShaderStr = R"(#version 330
        in vec4 position;
        in vec3 normal;
        in vec2 uv;
        out vec3 vNormal;
        out vec2 vUV;
        out vec3 vEyePos;

        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;
        uniform mat3 normalMat;

        void main(void) {
            vec4 eyePos = view * model * position;
            vEyePos = eyePos.xyz;
            vNormal = normalMat * normal;
            vUV = uv;
            gl_Position = projection * eyePos;
        }
    )";
    m_fragmentShaderStr = R"(#version 330
        out vec4 fragColor;
        in vec3 vNormal;
        in vec2 vUV;
        in vec3 vEyePos;

        uniform vec4 ambientLight;
        uniform vec4 color;
        uniform sampler2D tex;

        uniform vec4 lightPosType[4];
        uniform vec4 lightColorRange[4];
        uniform float specularity;

        vec3 computeLight()
        {
            vec3 lightColor = ambientLight.xyz;
            vec3 normal = normalize(vNormal);

            float diffuseFrac = 1.0 - ambientLight.w;
            for (int i = 0; i < 4; i++)
            {
                bool isDirectional = lightPosType[i].w == 0.0;
                bool isPoint       = lightPosType[i].w == 1.0;
                vec3 lightDirection;
                float att = 1.0;
                if (isDirectional)
                {
                    lightDirection = normalize(lightPosType[i].xyz);
                }
                else if (isPoint)
                {
                    vec3 lightVector = lightPosType[i].xyz - vEyePos;
                    float lightRange = lightColorRange[i].w;
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
                   lightColor += (att * diffuseFrac * thisDiffuse) * lightColorRange[i].xyz;
                }
                // specular light
                if (specularity > 0)
                {
                    vec3 H = normalize(lightDirection - normalize(vEyePos));
                    float nDotHV = dot(normal, H);
                    if (nDotHV > 0)
                    {
                        float pf = pow(nDotHV, specularity);
                        lightColor += vec3(att * diffuseFrac * pf); // white specular highlights
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
        in vec4 position;
        in vec3 normal;
        in vec2 uv;
        out vec2 vUV;

        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;

        void main(void) {
            vUV = uv;
            gl_Position = projection * view * model * position;
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
        in vec4 position;
        in vec3 normal;
        in vec2 uv;
        out vec2 vUV;

        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;

        void main(void)
        {
            gl_Position = projection * view * model * position;
            vUV = uv;
        }
        )";
    m_fragmentShaderStr = R"(#version 140
        out vec4 fragColor;
        in vec2 vUV;

        uniform vec4 color;
        uniform sampler2D tex;

        void main(void)
        {
            fragColor = color * texture(tex, vUV);
        }
    )";
    return *this;
}

Shader::ShaderBuilder& Shader::ShaderBuilder::withSourceStandardParticles()
{
    m_vertexShaderStr = R"(#version 140
        in vec4 position;
        in vec3 normal;
        in vec4 uv;
        in vec4 color;
        out mat3 vUVMat;
        out vec4 vColor;
        out vec3 uvSize;

        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;
        uniform float viewHeight;

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
            vec4 pos = vec4( position.xyz, 1.0);
            gl_Position = projection * view * model * pos;
            if (projection[2][3] != 0){ // if perspective projection
                vec3 ndc = gl_Position.xyz / gl_Position.w ; // perspective divide.
                float zDist = 1.0- ndc.z ; // 1 is close (right up in your face,)
                if (zDist < 0.0 || zDist > 1.0)
                {
                    zDist = 0.0;
                }
                gl_PointSize = viewHeight * position.w * zDist;
            }
            else
            {
                gl_PointSize = 0.1 * position.w * viewHeight; // average x,y scale in orthographic projection
            }
            vUVMat = translate(uv.xy)*scale(uv.z) * translate(vec2(0.5,0.5))*rotate(uv.w) * translate(vec2(-0.5,-0.5));
            vColor = color;
            uvSize = uv.xyz;
        }
    )";
    m_fragmentShaderStr = R"(#version 140
        out vec4 fragColor;
        in mat3 vUVMat;
        in vec4 vColor;
        in vec3 uvSize;

        uniform sampler2D tex;
        uniform int isSplit;
        void main(void)
        {
            vec2 uv = (isSplit == 1) ? gl_PointCoord : (vUVMat * vec3(gl_PointCoord,1.0)).xy;
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

Shader* Shader::ShaderBuilder::build()
{
    auto* shader = new Shader();
    if (!shader->build(m_vertexShaderStr, m_fragmentShaderStr))
    {
        delete shader;
        return nullptr;
    }
    shader->m_depthTest = m_depthTest;
    shader->m_depthWrite = m_depthWrite;
    shader->m_blendType = m_blendType;
    return shader;
}

Shader* Shader::getUnlit()
{
    if (s_unlit != nullptr)
    {
        return s_unlit;
    }
    s_unlit = Shader::create()
                  .withSourceUnlit()
                  .build();
    s_unlit->set("color", glm::vec4(1.0f));
    s_unlit->set("tex", Texture::getWhiteTexture());
    return s_unlit;
}

Shader* Shader::getUnlitSprite()
{
    if (s_unlitSprite != nullptr)
    {
        return s_unlitSprite;
    }

    s_unlitSprite = Shader::create()
                        .withSourceUnlitSprite()
                        .withBlend(BlendType::AlphaBlending)
                        .withDepthWrite(false)
                        .build();
    s_unlitSprite->set("color", glm::vec4(1));
    s_unlitSprite->set("tex", Texture::getWhiteTexture());
    return s_unlitSprite;
}

Shader* Shader::getStandard()
{
    if (s_standard != nullptr)
    {
        return s_standard;
    }
    s_standard = Shader::create().withSourceStandard().build();
    s_standard->set("color", glm::vec4(1));
    s_standard->set("tex", Texture::getWhiteTexture());
    s_standard->set("specularity", 0.0f);
    return s_standard;
}

Shader* Shader::getStandardParticles()
{
    if (s_standardParticles != nullptr)
    {
        return s_standardParticles;
    }
    s_standardParticles = Shader::create()
                              .withSourceStandardParticles()
                              .withBlend(BlendType::AdditiveBlending)
                              .withDepthWrite(false)
                              .build();
    s_standardParticles->set("tex", Texture::getSphereTexture());
    return s_standardParticles;
}

Shader::Shader()
{
    m_id = glCreateProgram();
    Renderer::s_instance->m_renderStatsCurrent.shaderCount++;
}

Shader::~Shader()
{
    glDeleteShader(m_id);
    Renderer::s_instance->m_renderStatsCurrent.shaderCount--;
}

bool Shader::set(const char* name, glm::mat4 value)
{
    glUseProgram(m_id);
    auto uniform = getType(name);
    if (uniform.location == -1)
    {
#ifdef DEBUG
        LOG_ERROR("Cannot find shader uniform!");
#endif
        return false;
    }
#ifndef NDEBUG
    if (uniform.type != UniformType::Mat4)
    {
        LOG_ERROR("Invalid shader uniform type for {}", name);
    }
    if (uniform.arrayCount != 1)
    {
        LOG_ERROR("Invalid shader uniform array count for {}", name);
    }
#endif
    glUniformMatrix4fv(uniform.location, 1, GL_FALSE, glm::value_ptr(value));
    return true;
}

bool Shader::set(const char* name, glm::mat3 value)
{
    glUseProgram(m_id);
    auto uniform = getType(name);
    if (uniform.location == -1)
    {
#ifdef DEBUG
        LOG_ERROR("Cannot find shader uniform!");
#endif
        return false;
    }
#ifndef NDEBUG
    if (uniform.type != UniformType::Mat3)
    {
        LOG_ERROR("Invalid shader uniform type for {}", name);
    }
    if (uniform.arrayCount != 1)
    {
        LOG_ERROR("Invalid shader uniform array count for {}", name);
    }
#endif
    glUniformMatrix3fv(uniform.location, 1, GL_FALSE, glm::value_ptr(value));
    return true;
}

bool Shader::set(const char* name, glm::vec4 value)
{
    glUseProgram(m_id);
    auto uniform = getType(name);
    if (uniform.location == -1)
    {
#ifdef DEBUG
        LOG_ERROR("Cannot find shader uniform!");
#endif
        return false;
    }
#ifndef NDEBUG
    if (uniform.type != UniformType::Vec4)
    {
        LOG_ERROR("Invalid shader uniform type for {}", name);
    }
    if (uniform.arrayCount != 1)
    {
        LOG_ERROR("Invalid shader uniform array count for {}", name);
    }
#endif
    glUniform4fv(uniform.location, 1, glm::value_ptr(value));
    return true;
}

bool Shader::set(const char* name, float value)
{
    glUseProgram(m_id);
    auto uniform = getType(name);
    if (uniform.location == -1)
    {
#ifdef DEBUG
        LOG_ERROR("Cannot find shader uniform!");
#endif
        return false;
    }
#ifndef NDEBUG
    if (uniform.type != UniformType::Float)
    {
        LOG_ERROR("Invalid shader uniform type for {}", name);
    }
#endif
    glUniform1f(uniform.location, value);
    return true;
}

bool Shader::set(const char* name, int value)
{
    glUseProgram(m_id);
    auto uniform = getType(name);
    if (uniform.location == -1)
    {
#ifdef DEBUG
        LOG_ERROR("Cannot find shader uniform!");
#endif
        return false;
    }
#ifndef NDEBUG
    if (uniform.type != UniformType::Int)
    {
        LOG_ERROR("Invalid shader uniform type for {}", name);
    }
#endif
    glUniform1i(uniform.location, value);
    return true;
}

bool Shader::set(const char* name, Texture* texture, unsigned int textureSlot)
{
    glUseProgram(m_id);
    auto uniform = getType(name);
    if (uniform.location == -1)
    {
#ifdef DEBUG
        LOG_ERROR("Cannot find shader uniform!");
#endif
        return false;
    }
#ifndef NDEBUG
    if (uniform.type != UniformType::Texture && uniform.type != UniformType::TextureCube)
    {
        LOG_ERROR("Invalid shader uniform type for {}", name);
    }
#endif
    glActiveTexture(GL_TEXTURE0 + textureSlot);
    glBindTexture(texture->m_info.target, texture->m_info.id);
    glUniform1i(uniform.location, textureSlot);
    return true;
}

bool Shader::setLights(Light* value, const glm::vec4& ambient, const glm::mat4& viewTransform)
{
    glUseProgram(m_id);
    auto uniform = getType("ambientLight");
    if (uniform.location != -1)
    {
        glUniform4fv(uniform.location, 1, glm::value_ptr(ambient));
    }

    glm::vec4 lightPosType[4];
    glm::vec4 lightColorRange[4];
    for (int i = 0; i < 4; i++)
    {
        if (value[i].type == Light::Type::Point)
        {
            lightPosType[i] = glm::vec4(value[i].position, 1);
        }
        else if (value[i].type == Light::Type::Directional)
        {
            lightPosType[i] = glm::vec4(value[i].direction, 0);
        }
        else if (value[i].type == Light::Type::Unused)
        {
            lightPosType[i] = glm::vec4(value[i].direction, 2);
            continue;
        }
        // transform to eye space
        lightPosType[i] = viewTransform * lightPosType[i];
        lightColorRange[i] = glm::vec4(value[i].color, value[i].range);
    }
    uniform = getType("lightPosType");
    if (uniform.location != -1)
    {
        if (uniform.arrayCount != 4)
        {
            LOG_ERROR("Invalid shader uniform array count for lightPosType");
        }
        glUniform4fv(uniform.location, 4, glm::value_ptr(lightPosType[0]));
    }
    uniform = getType("lightColorRange");
    if (uniform.location != -1)
    {
        if (uniform.arrayCount != 4)
        {
            LOG_ERROR("Invalid shader uniform array count for lightColorRange");
        }
        glUniform4fv(uniform.location, 4, glm::value_ptr(lightColorRange[0]));
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
        LOG_ERROR("err");
        break;
    }
}

bool Shader::contains(const char* name)
{
    bool result = std::any_of(m_uniforms.begin(), m_uniforms.end(), [&](const auto& u) {
        return u.name == name;
    });
    return result;
}

Shader::Uniform Shader::getType(const char* name)
{
    for (const auto& u : m_uniforms)
    {
        if (u.name == name)
            return u;
    }
    return {};
}

void Shader::updateUniforms()
{
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
        GLint location = glGetUniformLocation(m_id, name);
        m_uniforms.emplace_back(name, location, uniformType, size);
    }
}

Shader::ShaderBuilder Shader::create()
{
    return {};
}

bool Shader::build(const char* vertexShader, const char* fragmentShader)
{
    std::vector<const char*> shaderSrc{ vertexShader, fragmentShader };
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
    std::string attributeNames[4] = { "position", "normal", "uv", "color" };
    for (int i = 0; i < 4; i++)
    {
        glBindAttribLocation(m_id, i, attributeNames[i].c_str());
    }
    bool linked = linkProgram(m_id);
    if (!linked)
    {
        return false;
    }
    updateUniforms();
    return true;
}

size_t Texture::getDataSize()
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
} // namespace re