// Copyright (c) 2022-2022. Lorem ipsum dolor sit amet, consectetur adipiscing elit.
// Morbi non lorem porttitor neque feugiat blandit. Ut vitae ipsum eget quam lacinia accumsan.
// Etiam sed turpis ac ipsum condimentum fringilla. Maecenas magna.
// Proin dapibus sapien vel ante. Aliquam erat volutpat. Pellentesque sagittis ligula eget metus.
// Vestibulum commodo. Ut rhoncus gravida arcu.

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

    class MeshBuilder
    {
    public:
        MeshBuilder& withQuad();
        MeshBuilder& withCube();
        MeshBuilder& withSphere();
        MeshBuilder& withVertexPosition(const std::vector<glm::vec3>& position);
        MeshBuilder& withNormal(const std::vector<glm::vec3>& normal);
        MeshBuilder& withUvs(const std::vector<glm::vec4>& uv);
        MeshBuilder& withColors(const std::vector<glm::vec4> &colors);
        MeshBuilder& withParticleSize(const std::vector<float> &particleSize);
        MeshBuilder& withMeshTopology(Topology topology);
        MeshBuilder& withIndices(const std::vector<uint16_t>& indices);
        Mesh* build();

    private:
        MeshBuilder() = default;
        std::vector<glm::vec3> m_vertexPositions;
        std::vector<glm::vec3> m_normals;
        std::vector<glm::vec4> m_uvs;
        std::vector<glm::vec4> m_colors;
        std::vector<float> m_particleSize;
        std::vector<uint16_t> m_indices;
        Topology m_topology{ Topology::Triangles }; // mesh拓扑结构
        Mesh* m_updateMesh = nullptr;
        friend class Mesh;
    };

public:
    static MeshBuilder create();
    MeshBuilder update();

public:
    Mesh(const std::vector<glm::vec3> &vertexPositions, const std::vector<glm::vec3> &normals, const std::vector<glm::vec4> &uvs, const std::vector<glm::vec4> &colors,std::vector<float> particleSize, const std::vector<uint16_t> &indices, Topology meshTopology);
    ~Mesh();
    void update(const std::vector<glm::vec3> &vertexPositions, const std::vector<glm::vec3> &normals, const std::vector<glm::vec4> &uvs, const std::vector<glm::vec4> &colors, std::vector<float> particleSize, const std::vector<uint16_t> &indices, Topology meshTopology);
    inline int32_t getVertexCount() const { return m_vertexCount; }
    inline Topology topology() const { return m_topology; }
    const std::vector<glm::vec3>& getVertexPositions() const { return m_vertexPositions; }
    const std::vector<glm::vec3>& getNormals() const { return m_normals; }
    const std::vector<glm::vec4>& getUvs() const { return m_uvs; }
    const std::vector<glm::vec4>& getColors() const { return m_colors; }
    const std::vector<float>& getParticleSize() const { return m_particleSize; }
    const std::vector<uint16_t>& getIndices() const { return m_indices; }

private:
    void bind() const;
    void setVertexAttributePointers();

private:
    friend class Renderer;
    std::vector<glm::vec3> m_vertexPositions;
    std::vector<glm::vec3> m_normals;
    std::vector<glm::vec4> m_uvs;
    std::vector<glm::vec4> m_colors;
    std::vector<float> m_particleSize;
    std::vector<uint16_t> m_indices;
    Topology m_topology{ Topology::Triangles }; // mesh拓扑结构
    uint32_t m_vbo{ 0 };
    uint32_t m_vao{ 0 };
    uint32_t m_ebo{ 0 };
    int32_t m_vertexCount{ 0 };
};
} // namespace re

#endif //SIMPLERENDERENGINE_MESH_H
