//
// Created by william on 2022/5/24.
//

#include "particleMesh.h"

#include "glCommonDefine.h"
namespace re
{
ParticleMesh::ParticleMesh(const std::vector<glm::vec3>& vertexPositions, const std::vector<glm::vec4>& colors, const std::vector<glm::vec2>& uvCenter, const std::vector<float>& uvSize, const std::vector<float>& uvRotation, const std::vector<float>& particleSizes)
{
    glGenBuffers(1, &m_vbo);
    glGenVertexArrays(1, &m_vao);
    update(vertexPositions, colors, uvCenter, uvSize, uvRotation, particleSizes);
}

ParticleMesh::~ParticleMesh()
{
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);
}

void ParticleMesh::update(const std::vector<glm::vec3>& vertexPositions, const std::vector<glm::vec4>& colors, const std::vector<glm::vec2>& uvCenter, const std::vector<float>& uvSize, const std::vector<float>& uvRotation, const std::vector<float>& particleSizes)
{
    m_vertexPositions = vertexPositions;
    m_colors = colors;
    m_uvCenter = uvCenter;
    m_uvSize = uvSize;
    m_uvRotation = uvRotation;
    m_particleSizes = particleSizes;
    m_vertexCount = (int)vertexPositions.size();
    bool hasColors = colors.size() == m_vertexCount;
    bool hasUvSize = uvSize.size() == m_vertexCount;
    bool hasUvCenter = uvCenter.size() == m_vertexCount;
    bool hasUvRotation = uvRotation.size() == m_vertexCount;
    bool hasParticleSizes = particleSizes.size() == m_vertexCount;
    // interleave data
    int floatsPerVertex = 12;
    std::vector<float> interleavedData(m_vertexCount * floatsPerVertex);
    for (int i = 0; i < m_vertexCount; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            // position
            if (j < 3)
            {
                interleavedData[i * floatsPerVertex + j] = vertexPositions[i][j];
            }
            else
            {
                interleavedData[i * floatsPerVertex + j] = hasParticleSizes ? particleSizes[i] : 1.0f;
            }
            // color
            interleavedData[i * floatsPerVertex + j + 4] = hasColors ? colors[i][j] : 1.0f;
            // uv
            if (j < 2)
            {
                interleavedData[i * floatsPerVertex + j + 8] = hasUvCenter ? uvCenter[i][j] : 0.5f;
            }
            else if (j == 2)
            {
                interleavedData[i * floatsPerVertex + j + 8] = hasUvSize ? uvSize[i] : 1.0f;
            }
            else if (j == 3)
            {
                interleavedData[i * floatsPerVertex + j + 8] = hasUvRotation ? uvRotation[i] : 0.0f;
            }
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