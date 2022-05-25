//
// Created by william on 2022/5/24.
//

#ifndef SIMPLERENDERENGINE_PARTICLEMESH_H
#define SIMPLERENDERENGINE_PARTICLEMESH_H
#include <glm/glm.hpp>
#include <vector>
namespace re
{
class ParticleMesh
{
public:
    ParticleMesh(const std::vector<glm::vec3>& vertexPositions, const std::vector<glm::vec4>& colors, const std::vector<glm::vec2>& uv, const std::vector<float>& uvSize, const std::vector<float>& uvRotation, const std::vector<float>& particleSizes);
    ~ParticleMesh();
    void update(const std::vector<glm::vec3>& vertexPositions, const std::vector<glm::vec4>& colors, const std::vector<glm::vec2>& uv, const std::vector<float>& uvSize, const std::vector<float>& uvRotation, const std::vector<float>& particleSizes);
    int getVertexCount() const { return m_vertexCount; }

    inline const std::vector<glm::vec3>& getVertexPositions() const { return m_vertexPositions; }
    inline const std::vector<glm::vec4>& getColors() const { return m_colors; }
    inline const std::vector<float>& getParticleSizes() const { return m_particleSizes; }
    const std::vector<glm::vec2>& getUv() const { return m_uv; }
    const std::vector<float>& getUVSize() const { return m_uvSize; }
    const std::vector<float>& getUVRotation() const { return m_uvRotation; }

private:
    void bind();
    void setVertexAttributePointers();

private:
    uint32_t m_vbo{ 0 };
    uint32_t m_vao{ 0 };
    std::vector<glm::vec3> m_vertexPositions;
    std::vector<glm::vec4> m_colors;
    std::vector<glm::vec2> m_uv;
    std::vector<float> m_uvSize;
    std::vector<float> m_uvRotation;
    std::vector<float> m_particleSizes;
    int m_vertexCount;
    friend class Renderer;
};
} // namespace re

#endif //SIMPLERENDERENGINE_PARTICLEMESH_H
