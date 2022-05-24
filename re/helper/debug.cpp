//
// Created by william on 2022/5/23.
//

#include "debug.h"

#include "core/mesh.h"
#include "core/renderer.h"
#include "core/shader.h"

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
    std::vector<glm::vec3> normals = { glm::vec3{ 0 }, glm::vec3{ 0 } };
    std::vector<glm::vec2> uvs = { glm::vec2{ 0 }, glm::vec2{ 0 } };
    auto* mesh = new Mesh(vertices, normals, uvs, Mesh::Topology::Lines);
    auto* shader = Shader::getUnlit();
    shader->setVector("color", s_color);
    if (Renderer::instance() != nullptr)
    {
        Renderer::instance()->render(mesh, glm::mat4(1), shader);
    }
}
} // namespace re