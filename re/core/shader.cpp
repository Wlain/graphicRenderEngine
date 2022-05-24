//
// Created by william on 2022/5/22.
//

#include "shader.h"

#include "commonMacro.h"
#include "texture.h"

#include <OpenGL/gl3.h>
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

GLuint compileShader(const char* source, GLenum type)
{
    GLuint shader = glCreateShader(type);
    auto length = (GLint)strlen(source);
    glShaderSource(shader, 1, &source, &length);
    glCompileShader(shader);
    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE)
    {
        logCurrentCompileException(shader, type);
    }
    return shader;
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
        glGetProgramInfoLog(shaderProgram, logSize, NULL, errorLog.data());
        LOG_ERROR("{}", errorLog.data());
        return false;
    }
    return true;
}
} // namespace

Shader* Shader::getDebugUV()
{
    return nullptr;
}

Shader* Shader::getDebugNormals()
{
    return nullptr;
}

Shader* Shader::getStandard()
{
    if (s_standard != nullptr)
    {
        return s_standard;
    }
    const char* vertexShader = R"(#version 330
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
    const char* fragmentShader = R"(#version 330
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
            float diffuse = 0;
            float specular = 0;
            for (int i = 0; i < 4; i++)
            {
                bool isDirectional = lightPosType[i].w == 0.0;
                bool isPoint       = lightPosType[i].w == 1.0;
                vec3 lightDirection;
                float att = 1.0;
                if (isDirectional)
                {
                   lightDirection = lightPosType[i].xyz;
                }
                else if (isPoint)
                {
                   vec3 lightVector = lightPosType[i].xyz - vEyePos;
                   float lightVectorLength = length(lightVector);
                   lightDirection = lightVector/lightVectorLength;
                   att = pow(1.0-1/lightColorRange[i].w,2.0); // non physical range based attenuation
               } else {
                   continue;
               }
                vec3 H = normalize(lightDirection - vEyePos);
                // diffuse light
                float thisDiffuse = max(0.0,dot(lightDirection, normal));
                if (thisDiffuse > 0.0)
                {
                   lightColor += (att * diffuseFrac * thisDiffuse) * lightColorRange[i].xyz;
                }
                // specular light
                if (specularity > 0)
                {
                    float nDotHV = dot(normal, H);
                    if (nDotHV > 0)
                    {
                       float pf = pow(nDotHV, specularity);
                       lightColor += vec3(att * diffuseFrac * diffuseFrac * pf); // white specular highlights
                    }
               }
            }
            return lightColor;
        }

        void main(void)
        {
            vec4 color = color * texture(tex, vUV);
            vec3 light = computeLight();
            fragColor = color * vec4(light, 1.0);
        }
    )";
    s_standard = createShader(vertexShader, fragmentShader);
    s_standard->setVector("color", glm::vec4(1));
    s_standard->setTexture("tex", Texture::getWhiteTexture());
    s_standard->setFloat("specularity", 0);
    return s_standard;
}

Shader* Shader::getFont()
{
    if (s_font != nullptr)
    {
        return s_font;
    }
    const char* vertexShader = R"(#version 330
        in vec4 position;
        in vec3 normal;
        in vec2 uv;
        out vec2 vUV;

        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;

        void main(void) {
            gl_Position = projection * view * model * position;
            vUV = uv;
        }
        )";
    const char* fragmentShader = R"(#version 330
        out vec4 fragColor;
        in vec2 vUV;

        uniform vec4 color;
        uniform sampler2D tex;

        void main(void)
        {
            fragColor = color * texture(tex, vUV);
        }
    )";
    s_font = createShader(vertexShader, fragmentShader);
    s_font->setVector("color", glm::vec4(1));
    s_font->setTexture("tex", Texture::getFontTexture());
    s_font->setBlend(BlendType::AlphaBlending);
    return s_font;
}

Texture::~Texture()
{
    glDeleteTextures(1, &m_id);
}

Shader* Shader::createShader(const char* vertexShader, const char* fragmentShader)
{
    auto* shader = new Shader();
    std::vector<const char*> shaderSrc{ vertexShader, fragmentShader };
    std::vector<GLenum> shaderTypes{ GL_VERTEX_SHADER, GL_FRAGMENT_SHADER };
    for (int i = 0; i < 2; i++)
    {
        GLuint s = compileShader(shaderSrc[i], shaderTypes[i]);
        glAttachShader(shader->m_id, s);
    }
    // enforce layout
    std::string attributeNames[3] = { "position", "normal", "uv" };
    for (int i = 0; i < 3; i++)
    {
        glBindAttribLocation(shader->m_id, i, attributeNames[i].c_str());
    }
    bool linked = linkProgram(shader->m_id);
    if (!linked)
    {
        delete shader;
        return nullptr;
    }
    return shader;
}

Shader::Shader()
{
    m_id = glCreateProgram();
}

Shader::~Shader()
{
    glDeleteShader(m_id);
}

bool Shader::setMatrix(const char* name, glm::mat4 value)
{
    glUseProgram(m_id);
    GLint location = glGetUniformLocation(m_id, name);
    if (location == -1)
    {
        return false;
    }
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
    return true;
}

bool Shader::setMatrix(const char* name, glm::mat3 value)
{
    glUseProgram(m_id);
    GLint location = glGetUniformLocation(m_id, name);
    if (location == -1)
    {
        return false;
    }
    glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
    return true;
}

bool Shader::setVector(const char* name, glm::vec4 value)
{
    glUseProgram(m_id);
    GLint location = glGetUniformLocation(m_id, name);
    if (location == -1)
    {
        return false;
    }
    glUniform4fv(location, 1, glm::value_ptr(value));
    return true;
}

bool Shader::setFloat(const char* name, float value)
{
    glUseProgram(m_id);
    GLint location = glGetUniformLocation(m_id, name);
    if (location == -1)
    {
        return false;
    }
    glUniform1f(location, value);
    return true;
}

bool Shader::setInt(const char* name, int value)
{
    glUseProgram(m_id);
    GLint location = glGetUniformLocation(m_id, name);
    if (location == -1)
    {
        return false;
    }
    glUniform1i(location, value);
    return true;
}

bool Shader::setLights(Light* value, const glm::vec4& ambient, const glm::mat4& viewTransform)
{
    glUseProgram(m_id);
    GLint location = glGetUniformLocation(m_id, "ambientLight");
    if (location == -1)
    {
        return false;
    }
    glUniform4fv(location, 1, glm::value_ptr(ambient));
    location = glGetUniformLocation(m_id, "lightPosType");
    GLint location2 = glGetUniformLocation(m_id, "lightColorRange");
    if (location == -1 || location2 == -1)
    {
        LOG_ERROR("Set light not implemented!");
        return false;
    }
    glm::vec4 lightPosType[4];
    glm::vec4 lightColorRange[4];
    for (int i = 0; i < 4; ++i)
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
    glUniform4fv(location, 4, glm::value_ptr(lightPosType[0]));
    glUniform4fv(location2, 4, glm::value_ptr(lightColorRange[0]));
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
    switch (m_blending)
    {
    case BlendType::Disabled:
        glDisable(GL_BLEND);
        break;
    case BlendType::AlphaBlending:
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        break;
    }
}

Shader* Shader::getUnlit()
{
    if (s_unlit != nullptr)
    {
        return s_unlit;
    }
    const char* vertexShader = R"(#version 330
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
    const char* fragmentShader = R"(#version 330
        in vec2 vUV;
        out vec4 fragColor;
        uniform vec4 color;
        uniform sampler2D tex;
        void main()
        {
            fragColor = color * texture(tex, vUV);
        }
    )";
    s_unlit = createShader(vertexShader, fragmentShader);
    s_unlit->setVector("color", glm::vec4(1.0f));
    s_unlit->setTexture("tex", Texture::getWhiteTexture());
    return s_unlit;
}

Shader* Shader::getUnlitSprite()
{
    if (s_unlitSprite != nullptr)
    {
        return s_unlitSprite;
    }
    const char* vertexShader = R"(#version 330
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
    const char* fragmentShader = R"(#version 140
        out vec4 fragColor;
        in vec2 vUV;

        uniform vec4 color;
        uniform sampler2D tex;

        void main(void)
        {
            fragColor = color * texture(tex, vUV);
        }
    )";
    s_unlitSprite = createShader(vertexShader, fragmentShader);
    s_unlitSprite->setVector("color", glm::vec4(1));
    s_unlitSprite->setTexture("tex", Texture::getWhiteTexture());
    s_unlitSprite->setBlend(BlendType::AlphaBlending);
    s_unlitSprite->setDepthTest(false);
    return s_unlitSprite;
}

bool Shader::setTexture(const char* name, Texture* texture, unsigned int textureSlot)
{
    glUseProgram(m_id);
    GLint location = glGetUniformLocation(m_id, name);
    if (location == -1)
    {
        return false;
    }

    glActiveTexture(GL_TEXTURE0 + textureSlot);
    glBindTexture(GL_TEXTURE_2D, texture->m_id);
    glUniform1i(location, textureSlot);
    return true;
}
} // namespace re