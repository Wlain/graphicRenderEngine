//
// Created by william on 2022/5/22.
//

#ifndef SIMPLERENDERENGINE_MESH_H
#define SIMPLERENDERENGINE_MESH_H
#include "shader.h"

#include <array>
#include <cstdlib>
#include <glm/glm.hpp>
#include <map>
#include <vector>
// 创建网格后，顶点属性的数量和类型不能更改, 顶点允许update,
// 每个网格可以有多个索引集与之关联，这允许使用多种材质进行渲染
namespace re
{
class Shader;

class Mesh : public std::enable_shared_from_this<Mesh>
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
        // quad x,y = [-1;1] and z=0, UV=[0;1], normals=(0,0,1)
        MeshBuilder& withQuad();
        // cube mesh including UV coordinates, positions and normals
        MeshBuilder& withCube(float length = 1);
        // sphere mesh including UV coordinates, positions and normals
        MeshBuilder& withSphere(int stacks = 16, int slices = 32, float radius = 1);
        // set vertex attribute "position" of type vec3
        MeshBuilder& withPositions(const std::vector<glm::vec3>& position);
        // set vertex attribute "normal" of type vec3
        MeshBuilder& withNormals(const std::vector<glm::vec3>& normal);
        // set vertex attribute "uv" of type vec4(treated as two sets of texture coordinates))
        MeshBuilder& withUvs(const std::vector<glm::vec4>& uv);
        // set vertex attribute "colors" of type vec4
        MeshBuilder& withColors(const std::vector<glm::vec4>& colors);
        // set vertex attribute "particleSize" of type float
        MeshBuilder& withParticleSizes(const std::vector<float>& particleSize);
        MeshBuilder& withAttribute(std::string_view name, const std::vector<float>& values);
        MeshBuilder& withAttribute(std::string_view name, const std::vector<glm::vec2>& values);
        MeshBuilder& withAttribute(std::string_view name, const std::vector<glm::vec3>& values);
        MeshBuilder& withAttribute(std::string_view name, const std::vector<glm::vec4>& values);
        MeshBuilder& withAttribute(std::string_view name, const std::vector<glm::i32vec4>& values);
        MeshBuilder& withMeshTopology(Topology topology);
        MeshBuilder& withIndices(const std::vector<uint16_t>& indices, Mesh::Topology topology = Mesh::Topology::Triangles, int indexSet = 0);
        std::shared_ptr<Mesh> build();

    private:
        MeshBuilder() = default;
        MeshBuilder(const MeshBuilder&) = default;

    private:
        std::map<std::string, std::vector<float>> m_attributesFloat;
        std::map<std::string, std::vector<glm::vec2>> m_attributesVec2;
        std::map<std::string, std::vector<glm::vec3>> m_attributesVec3;
        std::map<std::string, std::vector<glm::vec4>> m_attributesVec4;
        std::map<std::string, std::vector<glm::i32vec4>> m_attributesIVec4;
        std::vector<std::vector<uint16_t>> m_indices;
        std::vector<Topology> m_topologies; // mesh拓扑结构
        Mesh* m_updateMesh{ nullptr };
        friend class Mesh;
    };

public:
    static MeshBuilder create();
    MeshBuilder update();

public:
    ~Mesh();
    inline int32_t getVertexCount() const { return m_vertexCount; }
    inline Topology getMeshTopology(int indexSet = 0) const { return m_topologies[indexSet]; }
    inline const std::vector<uint16_t>& getIndices(int indexSet = 0) const { return m_indices[indexSet]; }
    inline size_t getIndexSets() const { return m_indices.size(); }
    std::vector<glm::vec3> getPositions();
    std::vector<glm::vec3> getNormals();
    std::vector<glm::vec4> getUVs();
    std::vector<glm::vec4> getColors();
    std::vector<float> getParticleSizes();
    template <typename T>
    T get(std::string_view attributeName);
    std::pair<int, int> getType(std::string_view name);
    std::vector<std::string> getNames();
    // get the local axis aligned bounding box (AABB)
    std::array<glm::vec3, 2> getBoundsMinMax();
    // get size of the mesh in bytes on GPU
    int getDataSize();

private:
    struct Attribute
    {
        int offset;        // 偏移
        int elementCount;  // 元素个数
        int dataType;      // 数据类型:eg:GL_FLOAT
        int attributeType; // 属性类型:eg:GL_FLOAT_VEC3
    };
    void bind(Shader* shader, int indexSet);
    void setVertexAttributePointers(Shader* shader);
    Mesh(std::map<std::string, std::vector<float>>& attributesFloat, std::map<std::string, std::vector<glm::vec2>>& attributesVec2, std::map<std::string, std::vector<glm::vec3>>& attributesVec3, std::map<std::string, std::vector<glm::vec4>>& attributesVec4, std::map<std::string, std::vector<glm::i32vec4>>& attributesIVec4, std::vector<std::vector<uint16_t>>& indices, std::vector<Topology>& meshTopology);
    void update(std::map<std::string, std::vector<float>>& attributesFloat, std::map<std::string, std::vector<glm::vec2>>& attributesVec2, std::map<std::string, std::vector<glm::vec3>>& attributesVec3, std::map<std::string, std::vector<glm::vec4>>& attributesVec4, std::map<std::string, std::vector<glm::i32vec4>>& attributesIVec4, std::vector<std::vector<uint16_t>>& indices, std::vector<Topology>& meshTopology);

private:
    std::map<std::string, Attribute> m_attributeByName;
    std::map<std::string, std::vector<float>> m_attributesFloat;
    std::map<std::string, std::vector<glm::vec2>> m_attributesVec2;
    std::map<std::string, std::vector<glm::vec3>> m_attributesVec3;
    std::map<std::string, std::vector<glm::vec4>> m_attributesVec4;
    std::map<std::string, std::vector<glm::i32vec4>> m_attributesIVec4;
    std::vector<std::vector<uint16_t>> m_indices;
    std::vector<Topology> m_topologies; // mesh拓扑结构
    std::array<glm::vec3, 2> m_boundsMinMax;
    std::map<unsigned int, unsigned int> m_shaderToVao;
    uint32_t m_vbo{ 0 };
    std::vector<uint32_t> m_ebos;
    int32_t m_vertexCount{ 0 };
    int m_dataSize{ 0 };
    int m_totalBytesPerVertex{ 0 };
    friend class RenderPass;
};
} // namespace re

#endif //SIMPLERENDERENGINE_MESH_H
