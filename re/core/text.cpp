//
// Created by william on 2022/5/23.
//

#include "text.h"

#include "mesh.h"

#include <glm/glm.hpp>
#include <vector>
namespace re
{
Mesh* Text::createTextMesh(const char* text)
{
    size_t size = strlen(text);
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uvs;
    int sizeOfChar = 32;
    int charsPerRow = 16;
    for (int i = 0; i < size; i++)
    {
        int c = text[i];
        int posX = i * sizeOfChar;
        int posY = 0;
        vertices.emplace_back(posX, posY, 0);
        vertices.emplace_back(posX + sizeOfChar, posY, 0);
        vertices.emplace_back(posX + sizeOfChar, posY + sizeOfChar, 0);
        vertices.emplace_back(posX, posY, 0);
        vertices.emplace_back(posX + sizeOfChar, posY + sizeOfChar, 0);
        vertices.emplace_back(posX, posY + sizeOfChar, 0);

        for (int j = 0; j < 6; j++)
        {
            normals.emplace_back(0, 0, 1);
        }
        float delta = 1.0f / charsPerRow;
        float u = (c % charsPerRow) / (float)charsPerRow;
        float v = 1.0f - delta - ((c / charsPerRow) / (float)charsPerRow);
        uvs.emplace_back(u, v);
        uvs.emplace_back(u + delta, v);
        uvs.emplace_back(u + delta, v + delta);

        uvs.emplace_back(u, v);
        uvs.emplace_back(u + delta, v + delta);
        uvs.emplace_back(u, v + delta);
    }
    Mesh* mesh = new Mesh(vertices, normals, uvs, Mesh::Topology::Triangles);
    return mesh;
}
} // namespace re