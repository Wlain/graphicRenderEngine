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
    ParticleMesh(const std::vector<glm::vec3>& vertexPositions, const std::vector<glm::vec4>& colors, const std::vector<glm::vec4>& uvs, const std::vector<float>& particleSizes);
    ~ParticleMesh();
    void update(const std::vector<glm::vec3>& vertexPositions, const std::vector<glm::vec4>& colors, const std::vector<glm::vec4>& uvs, const std::vector<float>& particleSizes);
    int getVertexCount() const { return m_vertexCount; }

    inline const std::vector<glm::vec3>& getVertexPositions() const { return m_vertexPositions; }
    inline const std::vector<glm::vec4>& getColors() const { return m_colors; }
    inline const std::vector<glm::vec4>& getUVs() const { return m_uvs; }
    inline const std::vector<float>& getParticleSizes() const { return m_particleSizes; }

private:
    void bind();

private:
    uint32_t m_vbo{ 0 };
    uint32_t m_vao{ 0 };
    std::vector<glm::vec3> m_vertexPositions;
    std::vector<glm::vec4> m_colors;
    std::vector<glm::vec4> m_uvs;
    std::vector<float> m_particleSizes;
    int m_vertexCount;
    friend class Renderer;
};
} // namespace re

#endif //SIMPLERENDERENGINE_PARTICLEMESH_H
