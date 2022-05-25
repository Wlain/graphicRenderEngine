//
// Created by william on 2022/5/22.
//

#ifndef SIMPLERENDERENGINE_MESH_H
#define SIMPLERENDERENGINE_MESH_H
#include "glm/glm.hpp"
#include "shader.h"

#include <cstdlib>
#include <vector>

namespace re
{
class Shader;

class Mesh
{
public:
    enum class Topology : uint8_t
    {
        Points = 0x0000,
        Lines = 0x0001,
        LineStrip = 0x0003,
        Triangles = 0x0004,
        TriangleStrip = 0x0005,
        TriangleFan = 0x0006
    };

public:
    static Mesh* createQuad();
    static Mesh* createCube();
    static Mesh* createSphere();

public:
    Mesh(const std::vector<glm::vec3>& vertexPositions, const std::vector<glm::vec3>& normals, const std::vector<glm::vec2>& uvs, Mesh::Topology topology);
    Mesh(const std::vector<glm::vec3>& vertexPositions, const std::vector<glm::vec3>& normals, const std::vector<glm::vec2>& uvs, const std::vector<uint16_t>& indices, Mesh::Topology meshTopology = Mesh::Topology::Triangles);
    ~Mesh();
    void update(const std::vector<glm::vec3>& vertexPositions, const std::vector<glm::vec3>& normals, const std::vector<glm::vec2>& uvs);
    void update(const std::vector<glm::vec3>& vertexPositions, const std::vector<glm::vec3>& normals, const std::vector<glm::vec2>& uvs, const std::vector<uint16_t>& indices);
    inline int32_t getVertexCount() const { return m_vertexCount; }
    inline Topology topology() const { return m_topology; }
    const std::vector<glm::vec3>& getVertexPositions() const { return m_vertexPositions; }
    const std::vector<glm::vec3>& getNormals() const { return m_normals; }
    const std::vector<glm::vec2>& getUvs() const { return m_uvs; }
    const std::vector<uint16_t>& getIndices() const { return m_indices; }

private:
    void bind() const;
    void setVertexAttributePointers();

private:
    friend class Renderer;
    std::vector<glm::vec3> m_vertexPositions;
    std::vector<glm::vec3> m_normals;
    std::vector<glm::vec2> m_uvs;
    std::vector<uint16_t> m_indices;
    Topology m_topology{ Topology::Triangles }; // mesh拓扑结构
    uint32_t m_vbo{ 0 };
    uint32_t m_vao{ 0 };
    uint32_t m_ebo{ 0 };
    int32_t m_vertexCount{ 0 };
};
} // namespace re

#endif //SIMPLERENDERENGINE_MESH_H
