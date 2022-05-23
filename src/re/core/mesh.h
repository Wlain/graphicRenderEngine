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
        Triangles = 0x0004
    };

public:
    static Mesh* createQuad();
    static Mesh* createCube();
    static Mesh* createSphere();

public:
    Mesh();
    ~Mesh();
    void updateMesh(std::vector<glm::vec3>& vertexPositions,
                    std::vector<glm::vec3>& normals,
                    std::vector<glm::vec2>& uvs,
                    Topology topology = Topology::Triangles);
    inline int32_t getVertexCount() const { return m_vertexCount; }
    inline Topology topology() const { return m_topology; }

private:
    void bind() const;
    friend class Renderer;
    Topology m_topology{ Topology::Triangles }; // mesh拓扑结构
    uint32_t m_vbo{ 0 };
    uint32_t m_vao{ 0 };
    int32_t m_vertexCount{ 0 };
};
} // namespace re

#endif //SIMPLERENDERENGINE_MESH_H
