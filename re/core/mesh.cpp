
//
// Created by william on 2022/5/22.
//

#include "mesh.h"

#include "glCommonDefine.h"
#include "renderStats.h"
#include "renderer.h"

#include <glm/gtc/constants.hpp>
namespace re
{
Mesh::Mesh(const std::vector<glm::vec3>& vertexPositions, const std::vector<glm::vec3>& normals, const std::vector<glm::vec4>& uvs, const std::vector<glm::vec4>& colors, std::vector<float> particleSize, const std::vector<uint16_t>& indices, Topology meshTopology)
{
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);
    glGenVertexArrays(1, &m_vao);
    update(vertexPositions, normals, uvs, colors, particleSize, indices, meshTopology);
}

Mesh::~Mesh()
{
    RenderStats& renderStats = Renderer::s_instance->m_renderStatsCurrent;
    renderStats.meshBytes -= getDataSize();
    renderStats.meshCount--;
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_ebo);
    glDeleteBuffers(1, &m_vbo);
}

void Mesh::bind() const
{
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indices.empty() ? 0 : m_ebo);
}

void Mesh::update(const std::vector<glm::vec3>& vertexPositions, const std::vector<glm::vec3>& normals, const std::vector<glm::vec4>& uvs, const std::vector<glm::vec4>& colors, std::vector<float> particleSize, const std::vector<uint16_t>& indices, Topology meshTopology)
{
    m_vertexPositions = vertexPositions;
    m_normals = normals;
    m_uvs = uvs;
    m_colors = colors;
    m_particleSize = particleSize;
    m_indices = indices;
    m_topology = meshTopology;
    m_vertexCount = (int32_t)vertexPositions.size();
    bool hasNormals = m_normals.size() == vertexPositions.size();
    bool hasUvs = m_uvs.size() == vertexPositions.size();
    bool hasColors = m_colors.size() == vertexPositions.size();
    bool hasParticleSize = m_particleSize.size() == vertexPositions.size();
    // interleave data
    int floatsPerVertex = 15;
    std::vector<float> interleavedData(m_vertexCount * floatsPerVertex);
    for (int i = 0; i < m_vertexCount; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            // position
            if (j < 3)
            {
                interleavedData[i * floatsPerVertex + j] = vertexPositions[i][j];
            }
            else
            {
                interleavedData[i * floatsPerVertex + j] = hasParticleSize ? particleSize[i] : 1.0f;
            }
            // normals
            if (j < 3)
            {
                interleavedData[i * floatsPerVertex + j + 4] = hasNormals ? normals[i][j] : 0.0f;
            }
            // uv
            interleavedData[i * floatsPerVertex + j + 7] = hasUvs ? uvs[i][j] : (j == 2 ? 1.0f : 0.0f);
            // colors
            interleavedData[i * floatsPerVertex + j + 11] = hasColors ? colors[i][j] : 1.0f;
        }
    }
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * interleavedData.size(), interleavedData.data(), GL_STATIC_DRAW);
    setVertexAttributePointers();
    if (!m_indices.empty())
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
        uint32_t indicesSize = indices.size() * sizeof(uint16_t);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize, indices.data(), GL_STATIC_DRAW);
    }
}

void Mesh::setVertexAttributePointers()
{
    // bind vertex attributes (position, normal, uv, colors)
    int length[4] = { 4, 3, 4, 4 };
    int offset[4] = { 0, 4, 7, 11 };
    int floatsPerVertex = 15;
    for (int i = 0; i < 4; ++i)
    {
        glEnableVertexAttribArray(i);
        glVertexAttribPointer(i, length[i], GL_FLOAT, GL_FALSE, floatsPerVertex * sizeof(float), (void*)(offset[i] * sizeof(float)));
    }
}

Mesh::MeshBuilder Mesh::create()
{
    return {};
}

Mesh::MeshBuilder Mesh::update()
{
    Mesh::MeshBuilder builder;
    builder.m_updateMesh = this;
    builder.m_vertexPositions = m_vertexPositions;
    builder.m_normals = m_normals;
    builder.m_uvs = m_uvs;
    builder.m_particleSize = m_particleSize;
    builder.m_colors = m_colors;
    builder.m_indices = m_indices;
    builder.m_topology = m_topology;
    return builder;
}

size_t Mesh::getDataSize()
{
    size_t size = 0;
    size += m_vertexCount * (sizeof(glm::vec3) + // position
                             sizeof(glm::vec3) + // normals
                             sizeof(glm::vec4) + // uvs
                             sizeof(glm::vec4) + // colors
                             sizeof(float));     // particle size
    size += m_indices.size() * sizeof(uint16_t);
    return size;
}

Mesh::MeshBuilder& Mesh::MeshBuilder::withQuad()
{
    std::vector<glm::vec3> vertices({ { 1, -1, 0 },
                                      { 1, 1, 0 },
                                      { -1, -1, 0 },
                                      { -1, 1, 0 } });

    std::vector<glm::vec3> normals(4, glm::vec3{ 0, 0, 1 });
    std::vector<glm::vec4> uvs({ { 1, 0, 0, 0 },
                                 { 1, 1, 0, 0 },
                                 { 0, 0, 0, 0 },
                                 { 0, 1, 0, 0 } });
    std::vector<uint16_t> indices = {
        0, 1, 2,
        2, 1, 3
    };
    withVertexPosition(vertices);
    withNormal(normals);
    withUvs(uvs);
    withIndices(indices);
    withMeshTopology(Topology::Triangles);
    return *this;
}

Mesh::MeshBuilder& Mesh::MeshBuilder::withCube()
{
    using namespace glm;
    float length = 1.0f;
    //    v5----- v4
    //   /|      /|
    //  v1------v0|
    //  | |     | |
    //  | |v6---|-|v7
    //  |/      |/
    //  v2------v3
    vec3 p[] = { { length, length, length },
                 { -length, length, length },
                 { -length, -length, length },
                 { length, -length, length },

                 { length, length, -length },
                 { -length, length, -length },
                 { -length, -length, -length },
                 { length, -length, -length } };
    std::vector<vec3> positions({
        p[0], p[1], p[2], p[0], p[2], p[3], // v0-v1-v2-v3
        p[4], p[0], p[3], p[4], p[3], p[7], // v4-v0-v3-v7
        p[5], p[4], p[7], p[5], p[7], p[6], // v5-v4-v7-v6
        p[1], p[5], p[6], p[1], p[6], p[2], // v1-v5-v6-v2
        p[4], p[5], p[1], p[4], p[1], p[0], // v1-v5-v6-v2
        p[3], p[2], p[6], p[3], p[6], p[7], // v1-v5-v6-v2
    });
    vec4 u[] = { { 1, 1, 0, 0 },
                 { 0, 1, 0, 0 },
                 { 0, 0, 0, 0 },
                 { 1, 0, 0, 0 } };
    // clang-format off
    std::vector<vec4> uvs({
        u[0], u[1], u[2], u[0], u[2], u[3], u[0], u[1], u[2], u[0], u[2], u[3],
        u[0], u[1], u[2], u[0], u[2], u[3], u[0], u[1], u[2], u[0], u[2], u[3],
        u[0], u[1], u[2], u[0], u[2], u[3], u[0], u[1], u[2], u[0], u[2], u[3],
    });
    std::vector<vec3> normals({
        {0, 0, 1},  {0, 0, 1},  {0, 0, 1},  {0, 0, 1},
        {0, 0, 1},  {0, 0, 1},  {1, 0, 0},  {1, 0, 0},
        {1, 0, 0},  {1, 0, 0},  {1, 0, 0},  {1, 0, 0},
        {0, 0, -1}, {0, 0, -1}, {0, 0, -1}, {0, 0, -1},
        {0, 0, -1}, {0, 0, -1}, {-1, 0, 0}, {-1, 0, 0},
        {-1, 0, 0}, {-1, 0, 0}, {-1, 0, 0}, {-1, 0, 0},
        {0, 1, 0},  {0, 1, 0},  {0, 1, 0},  {0, 1, 0},
        {0, 1, 0},  {0, 1, 0},  {0, -1, 0}, {0, -1, 0},
        {0, -1, 0}, {0, -1, 0}, {0, -1, 0}, {0, -1, 0},
    });
    // clang-format on
    withVertexPosition(positions);
    withNormal(normals);
    withUvs(uvs);
    withMeshTopology(Topology::Triangles);
    return *this;
}

Mesh::MeshBuilder& Mesh::MeshBuilder::withSphere()
{
    using namespace glm;
    int stacks = 8;
    int slices = 16;
    float radius = 1.0f;
    size_t vertexCount = ((stacks + 1) * slices);
    std::vector<vec3> vertices{ vertexCount };
    std::vector<vec3> normals{ vertexCount };
    std::vector<vec4> uvs{ vertexCount };
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
            uvs[index] = vec4{ 1 - (float)i / (float)slices, (float)j / (float)stacks, 0, 0 };
            vertices[index] = normal * radius;
            index++;
        }
    }
    std::vector<vec3> finalPosition;
    std::vector<vec3> finalNormals;
    std::vector<vec4> finalUVs;
    // create indices
    for (int j = 0; j < stacks; j++)
    {
        for (int i = 0; i < slices; i++)
        {
            glm::u8vec2 offset[] = {
                // first triangle
                { i, j },
                { (i + 1) % slices, j + 1 },
                { (i + 1) % slices, j },

                // second triangle
                { i, j },
                { i, j + 1 },
                { (i + 1) % slices, j + 1 }
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
    withVertexPosition(finalPosition);
    withNormal(finalNormals);
    withUvs(finalUVs);
    withMeshTopology(Topology::Triangles);
    return *this;
}

Mesh::MeshBuilder& Mesh::MeshBuilder::withVertexPosition(const std::vector<glm::vec3>& position)
{
    m_vertexPositions = position;
    return *this;
}

Mesh::MeshBuilder& Mesh::MeshBuilder::withNormal(const std::vector<glm::vec3>& normal)
{
    m_normals = normal;
    return *this;
}

Mesh::MeshBuilder& Mesh::MeshBuilder::withUvs(const std::vector<glm::vec4>& uv)
{
    m_uvs = uv;
    return *this;
}

Mesh::MeshBuilder& Mesh::MeshBuilder::withColors(const std::vector<glm::vec4>& colors)
{
    m_colors = colors;
    return *this;
}

Mesh::MeshBuilder& Mesh::MeshBuilder::withParticleSize(const std::vector<float>& particleSize)
{
    m_particleSize = particleSize;
    return *this;
}

Mesh::MeshBuilder& Mesh::MeshBuilder::withMeshTopology(Mesh::Topology topology)
{
    m_topology = topology;
    return *this;
}

Mesh::MeshBuilder& Mesh::MeshBuilder::withIndices(const std::vector<uint16_t>& indices)
{
    m_indices = indices;
    return *this;
}

Mesh* Mesh::MeshBuilder::build()
{
    // update stats
    RenderStats& renderStats = Renderer::s_instance->m_renderStatsCurrent;
    Mesh* mesh;
    if (m_updateMesh != nullptr)
    {
        renderStats.meshBytes -= m_updateMesh->getDataSize();
        m_updateMesh->update(m_vertexPositions, m_normals, m_uvs, m_colors, m_particleSize, m_indices, m_topology);
        mesh = m_updateMesh;
    }
    else
    {
        mesh = new Mesh(m_vertexPositions, m_normals, m_uvs, m_colors, m_particleSize, m_indices, m_topology);
        renderStats.meshCount++;
    }
    renderStats.meshBytes += mesh->getDataSize();
    return mesh;
}

} // namespace re