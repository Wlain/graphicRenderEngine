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

enum class MeshTopology : uint8_t
{
    Points = 0x0000,
    Lines = 0x0001,
    Triangles = 0x0004
};

class Mesh
{
public:
    static Mesh* createQuad();
    static Mesh* createCube();
    static Mesh* createSphere();

public:
    Mesh();
    ~Mesh();
    void bind();
    void updateMesh(std::vector<glm::vec3>& vertexPositions,
                    std::vector<glm::vec3>& normals,
                    std::vector<glm::vec2>& uvs,
                    MeshTopology meshTopology = MeshTopology::Triangles);
    inline size_t getVertexCount() const { return m_vertexCount; }
    inline MeshTopology getMeshTopology() const { return m_meshTopology; }

private:
    MeshTopology m_meshTopology{ MeshTopology::Triangles }; // mesh拓扑结构
    unsigned int m_vbo{ 0 };
    unsigned int m_vao{ 0 };
    size_t m_vertexCount{ 0 };
};
} // namespace re

#endif //SIMPLERENDERENGINE_MESH_H
