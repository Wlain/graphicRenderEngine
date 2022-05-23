//
// Created by william on 2022/5/22.
//

#include "mesh.h"

#include <OpenGL/gl3.h>
#include <glm/gtc/constants.hpp>
namespace re
{
Mesh::Mesh()
{
    glGenBuffers(1, &m_vbo);
    glGenVertexArrays(1, &m_vao);
}

Mesh::~Mesh()
{
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);
}

void Mesh::bind() const
{
    glBindVertexArray(m_vao);
}

void Mesh::updateMesh(std::vector<glm::vec3>& vertexPositions, std::vector<glm::vec3>& normals, std::vector<glm::vec2>& uvs, Topology topology)
{
    m_topology = topology;
    m_vertexCount = (int32_t)vertexPositions.size();
    if (normals.size() < m_vertexCount)
    {
        normals.resize(m_vertexCount, { 0.0f, 0.0f, 0.0f });
    }
    if (uvs.size() < m_vertexCount)
    {
        uvs.resize(m_vertexCount, { 0.0f, 0.0f });
    }
    // interleave data
    int floatsPerVertex = 8;
    std::vector<float> interleavedData(m_vertexCount * floatsPerVertex);
    for (int i = 0; i < m_vertexCount; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            interleavedData[i * 8 + j] = vertexPositions[i][j];
            interleavedData[i * 8 + j + 3] = normals[i][j];
            if (j < 2)
            {
                interleavedData[i * 8 + j + 6] = uvs[i][j];
            }
        }
    }
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * interleavedData.size(), interleavedData.data(), GL_STATIC_DRAW);
    // bind vertex attributes (position, normal, uv)
    int length[3] = { 3, 3, 2 };
    int offset[3] = { 0, 3, 6 };
    for (int i = 0; i < 3; i++)
    {
        glEnableVertexAttribArray(i);
        glVertexAttribPointer(i, length[i], GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(offset[i] * sizeof(float)));
    }
}

Mesh* Mesh::createQuad()
{
    Mesh* mesh = new Mesh();
    std::vector<glm::vec3> vertices({ glm::vec3{ 1, -1, 0 }, glm::vec3{ 1, 1, 0 },
                                      glm::vec3{ -1, -1, 0 }, glm::vec3{ -1, -1, 0 },
                                      glm::vec3{ 1, 1, 0 }, glm::vec3{ -1, 1, 0 } });
    std::vector<glm::vec3> normals(6, glm::vec3{ 0, 0, 1 });
    std::vector<glm::vec2> uvs({ glm::vec2{ 1, 0 }, glm::vec2{ 1, 1 }, glm::vec2{ 0, 0 },
                                 glm::vec2{ 0, 0 }, glm::vec2{ 1, 1 },
                                 glm::vec2{ 0, 1 } });
    mesh->updateMesh(vertices, normals, uvs, Topology::Triangles);
    return mesh;
}

Mesh* Mesh::createCube()
{
    using namespace glm;
    float length = 1.0f;
    Mesh* mesh = new Mesh();
    //    v5----- v4
    //   /|      /|
    //  v1------v0|
    //  | |     | |
    //  | |v6---|-|v7
    //  |/      |/
    //  v2------v3
    vec3 p[] = { vec3{ length, length, length },
                 vec3{ -length, length, length },
                 vec3{ -length, -length, length },
                 vec3{ length, -length, length },

                 vec3{ length, length, -length },
                 vec3{ -length, length, -length },
                 vec3{ -length, -length, -length },
                 vec3{ length, -length, -length } };
    std::vector<vec3> positions({
        p[0], p[1], p[2], p[0], p[2], p[3], // v0-v1-v2-v3
        p[4], p[0], p[3], p[4], p[3], p[7], // v4-v0-v3-v7
        p[5], p[4], p[7], p[5], p[7], p[6], // v5-v4-v7-v6
        p[1], p[5], p[6], p[1], p[6], p[2], // v1-v5-v6-v2
        p[4], p[5], p[1], p[4], p[1], p[0], // v1-v5-v6-v2
        p[3], p[2], p[6], p[3], p[6], p[7], // v1-v5-v6-v2
    });
    vec2 u[] = { vec2(1, 1), vec2(0, 1), vec2(0, 0), vec2(1, 0) };
    // clang-format off
    std::vector<vec2> uvs({
        u[0], u[1], u[2], u[0], u[2], u[3], u[0], u[1], u[2], u[0], u[2], u[3],
        u[0], u[1], u[2], u[0], u[2], u[3], u[0], u[1], u[2], u[0], u[2], u[3],
        u[0], u[1], u[2], u[0], u[2], u[3], u[0], u[1], u[2], u[0], u[2], u[3],
    });
    std::vector<vec3> normals({
        vec3{0, 0, 1},  vec3{0, 0, 1},  vec3{0, 0, 1},  vec3{0, 0, 1},
        vec3{0, 0, 1},  vec3{0, 0, 1},  vec3{1, 0, 0},  vec3{1, 0, 0},
        vec3{1, 0, 0},  vec3{1, 0, 0},  vec3{1, 0, 0},  vec3{1, 0, 0},
        vec3{0, 0, -1}, vec3{0, 0, -1}, vec3{0, 0, -1}, vec3{0, 0, -1},
        vec3{0, 0, -1}, vec3{0, 0, -1}, vec3{-1, 0, 0}, vec3{-1, 0, 0},
        vec3{-1, 0, 0}, vec3{-1, 0, 0}, vec3{-1, 0, 0}, vec3{-1, 0, 0},
        vec3{0, 1, 0},  vec3{0, 1, 0},  vec3{0, 1, 0},  vec3{0, 1, 0},
        vec3{0, 1, 0},  vec3{0, 1, 0},  vec3{0, -1, 0}, vec3{0, -1, 0},
        vec3{0, -1, 0}, vec3{0, -1, 0}, vec3{0, -1, 0}, vec3{0, -1, 0},
    });
    // clang-format on
    mesh->updateMesh(positions, normals, uvs, Topology::Triangles);
    return mesh;
}

Mesh* Mesh::createSphere()
{
    using namespace glm;
    Mesh* mesh = new Mesh();
    int stacks = 16;
    int slices = 32; // 片
    float radius = 1.0f;
    size_t vertexCount = ((stacks + 1) * slices);
    std::vector<vec3> vertices{ vertexCount };
    std::vector<vec3> normals{ vertexCount };
    std::vector<vec2> uvs{ vertexCount };
    int index = 0;
    // create vertices
    for (int j = 0; j <= stacks; j++)
    {
        float latitude1 = (glm::pi<float>() / (float)stacks) * (float)j - (glm::pi<float>() / 2);
        float sinLat1 = sin(latitude1);
        float cosLat1 = cos(latitude1);
        for (int i = 0; i < slices; i++)
        {
            float longitude = ((glm::pi<float>() * 2) / (float)slices) * (float)i;
            float sinLong = sin(longitude);
            float cosLong = cos(longitude);
            vec3 normal{ cosLong * cosLat1, sinLat1, sinLong * cosLat1 };
            normal = normalize(normal);
            normals[index] = normal;
            uvs[index] = vec2{ 1 - (float)i / (float)slices, (float)j / (float)stacks };
            vertices[index] = normal * radius;
            index++;
        }
    }
    std::vector<vec3> finalPosition;
    std::vector<vec3> finalNormals;
    std::vector<vec2> finalUVs;
    // create indices
    for (int j = 0; j < stacks; j++)
    {
        for (int i = 0; i <= slices; i++)
        {
            glm::u8vec2 offset[] = {
                // first triangle
                glm::u8vec2{ i, j },
                glm::u8vec2{ (i + 1) % slices, j },
                glm::u8vec2{ (i + 1) % slices, j + 1 },
                // second triangle
                glm::u8vec2{ i, j },
                glm::u8vec2{ (i + 1) % slices, j + 1 },
                glm::u8vec2{ i, j + 1 },
            };
            for (const auto& o : offset)
            {
                index = o[1] * slices + o[0];
                finalPosition.push_back(vertices[index]);
                finalNormals.push_back(normals[index]);
                finalUVs.push_back(uvs[index]);
            }
        }
    }
    mesh->updateMesh(finalPosition, finalNormals, finalUVs, Topology::Triangles);
    return mesh;
}
} // namespace re