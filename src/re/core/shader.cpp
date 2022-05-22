//
// Created by william on 2022/5/22.
//

#include "shader.h"

#include "commonMacro.h"

#include <OpenGL/gl3.h>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <vector>
namespace re
{
// 匿名命名空间,变量声明对其他文件中的代码不可见
namespace
{
void logCurrentCompileException(GLuint shader, GLenum type, const char* source)
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
    }
    LOG_ERROR("%s\n, %s error", errorLog.data(), typeStr);
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
        logCurrentCompileException(shader, type, source);
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
        LOG_ERROR("%s", errorLog.data())
        return false;
    }
    return true;
}
} // namespace

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

Shader* Shader::createUnlitColor()
{
    if (s_unlitColor != nullptr)
    {
        return s_unlitColor;
    }
    const char* vertexShader = R"(#version 330
        in vec4 position;
        in vec3 normal;
        in vec2 uv;

        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;

        void main(void) {
            gl_Position = projection * view * model * position;
        }
    )";
    const char* fragmentShader = R"(#version 330
        out vec4 fragColor;
        uniform vec4 color;
        void main(void)
        {
            fragColor = color;
        }
    )";
    s_unlitColor = createShader(vertexShader, fragmentShader);
    return s_unlitColor;
}

Shader* Shader::createDebugUV()
{
    return nullptr;
}

Shader* Shader::createDebugNormals()
{
    return nullptr;
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

bool Shader::setLights(Light* value)
{
    glUseProgram(m_id);
    GLint location = glGetUniformLocation(m_id, "lights");
    if (location == -1)
    {
        return false;
    }

    LOG_ERROR("Set light not implemented!");
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
    case BlendingType::Disabled:
        glDisable(GL_BLEND);
        break;
    case BlendingType::AlphaBlending:
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        break;
    }
}
} // namespace re