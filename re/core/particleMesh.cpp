//
// Created by william on 2022/5/24.
//

#include "particleMesh.h"

#include "glCommonDefine.h"
namespace re
{
ParticleMesh::ParticleMesh(const std::vector<glm::vec3>& vertexPositions, const std::vector<glm::vec4>& colors, const std::vector<glm::vec4>& uvs, const std::vector<float>& particleSizes)
{
    glGenBuffers(1, &m_vbo);
    glGenVertexArrays(1, &m_vao);
    update(vertexPositions, colors, uvs, particleSizes);
}

ParticleMesh::~ParticleMesh()
{
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);
}

void ParticleMesh::update(const std::vector<glm::vec3>& vertexPositions, const std::vector<glm::vec4>& colors, const std::vector<glm::vec4>& uvs, const std::vector<float>& particleSizes)
{
    m_vertexPositions = vertexPositions;
    m_colors = colors;
    m_uvs = uvs;
    m_particleSizes = particleSizes;
    m_vertexCount = (int)vertexPositions.size();
    bool hasColors = colors.size() == vertexPositions.size();
    bool hasUVs = uvs.size() == vertexPositions.size();
    bool hasParticleSizes = particleSizes.size() == vertexPositions.size();
    // interleave data
    int floatsPerVertex = 12;
    std::vector<float> interleavedData(vertexPositions.size() * floatsPerVertex);
    for (int i = 0; i < vertexPositions.size(); i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (j < 3)
            {
                interleavedData[i * floatsPerVertex + j] = vertexPositions[i][j];
            }
            else
            {
                interleavedData[i * floatsPerVertex + j] = hasParticleSizes ? particleSizes[i] : 1.0f;
            }
            interleavedData[i * floatsPerVertex + j + 4] = hasColors ? colors[i][j] : 0.0f;
            // default uv values [0,0,1,1]
            interleavedData[i * floatsPerVertex + j + 8] = hasUVs ? uvs[i][j] : (j < 2 ? 0.0f : 1.0f);
        }
    }
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * interleavedData.size(), interleavedData.data(), GL_STATIC_DRAW);
    // bind vertex attributes (position+size, color, uvs)
    int length[3] = { 4, 4, 4 };
    int offset[3] = { 0, 4, 8 };
    for (int i = 0; i < 3; i++)
    {
        glEnableVertexAttribArray(i);
        glVertexAttribPointer(i, length[i], GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(offset[i] * sizeof(float)));
    }
}

void ParticleMesh::bind()
{
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
} // namespace re