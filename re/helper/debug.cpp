//
// Created by william on 2022/5/23.
//

#include "debug.h"

#include "commonMacro.h"
#include "core/mesh.h"
#include "core/renderer.h"
#include "core/shader.h"
#include "core/texture.h"
#include "glCommonDefine.h"

#include <vector>

namespace re
{
glm::vec4 Debug::getColor()
{
    return s_color;
}

void Debug::setColor(const glm::vec4& color)
{
    s_color = color;
}

void Debug::drawLine(glm::vec3 from, glm::vec3 to)
{
    std::vector<glm::vec3> vertices = { from, to };
    auto mesh = std::unique_ptr<Mesh>(Mesh::create()
                                          .withVertexPosition(vertices)
                                          .withMeshTopology(Mesh::Topology::Lines)
                                          .build());
    auto* shader = Shader::getUnlit();
    shader->set("color", s_color);
    shader->set("tex", Texture::getWhiteTexture());
    if (Renderer::s_instance != nullptr)
    {
        Renderer::s_instance->render(mesh.get(), glm::mat4(1), shader);
    }
}

void Debug::checkGLError()
{
    for (GLenum err; (err = glGetError()) != GL_NO_ERROR;)
    {
        switch (err)
        {
        case GL_INVALID_ENUM:
            LOG_ERROR("GL_INVALID_ENUM");
            break;
        case GL_INVALID_VALUE:
            LOG_ERROR("GL_INVALID_VALUE");
            break;
        case GL_INVALID_OPERATION:
            LOG_ERROR("GL_INVALID_OPERATION");
            break;
        case GL_OUT_OF_MEMORY:
            LOG_ERROR("GL_OUT_OF_MEMORY");
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            LOG_ERROR("GL_INVALID_FRAMEBUFFER_OPERATION");
            break;
        }
    }
}

void Debug::drawLineStrip(const std::vector<glm::vec3>& vertices)
{
    auto mesh = std::unique_ptr<Mesh>(Mesh::create()
                                          .withVertexPosition(vertices)
                                          .withMeshTopology(Mesh::Topology::Lines)
                                          .build());
    auto* shader = Shader::getUnlit();
    shader->set("color", s_color);
    shader->set("tex", Texture::getWhiteTexture());
    if (Renderer::s_instance != nullptr)
    {
        Renderer::s_instance->render(mesh.get(), glm::mat4(1), shader);
    }
}
} // namespace re