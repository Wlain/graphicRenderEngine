
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
Mesh::Mesh(std::map<std::string, std::vector<float>>& attributesFloat, std::map<std::string, std::vector<glm::vec2>>& attributesVec2, std::map<std::string, std::vector<glm::vec3>>& attributesVec3, std::map<std::string, std::vector<glm::vec4>>& attributesVec4, std::map<std::string, std::vector<glm::i32vec4>>& attributesIVec4, const std::vector<uint16_t>& indices, Mesh::Topology meshTopology)
{
    if (!Renderer::s_instance)
    {
        throw std::runtime_error("Cannot instantiate re::Mesh before re::Renderer is created.");
    }
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);
    update(attributesFloat, attributesVec2, attributesVec3, attributesVec4, attributesIVec4, indices, meshTopology);
}

Mesh::~Mesh()
{
    if (Renderer::s_instance)
    {
        RenderStats& renderStats = Renderer::s_instance->m_renderStatsCurrent;
        renderStats.meshBytes -= getDataSize();
        renderStats.meshCount--;
    }
    for (auto obj : m_shaderToVao)
    {
        glDeleteVertexArrays(1, &(obj.second));
    }
    glDeleteBuffers(1, &m_ebo);
    glDeleteBuffers(1, &m_vbo);
}

void Mesh::bind(Shader* shader)
{
    auto res = m_shaderToVao.find(shader->m_id);
    if (res != m_shaderToVao.end())
    {
        GLuint vao = res->second;
        glBindVertexArray(vao);
    }
    else
    {
        GLuint index;
        glGenVertexArrays(1, &index);
        glBindVertexArray(index);
        setVertexAttributePointers(shader);
        m_shaderToVao[shader->m_id] = index;
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indices.empty() ? 0 : m_ebo);
}

void Mesh::update(std::map<std::string, std::vector<float>>& attributesFloat, std::map<std::string, std::vector<glm::vec2>>& attributesVec2, std::map<std::string, std::vector<glm::vec3>>& attributesVec3, std::map<std::string, std::vector<glm::vec4>>& attributesVec4, std::map<std::string, std::vector<glm::i32vec4>>& attributesIVec4, const std::vector<uint16_t>& indices, Mesh::Topology meshTopology)
{
    m_indices = indices;
    m_topology = meshTopology;
    m_vertexCount = 0;
    m_totalBytesPerVertex = 0;
    for (const auto& obj : m_shaderToVao)
    {
        glDeleteVertexArrays(1, &(obj.second));
    }
    m_shaderToVao.clear();
    m_attributeByName.clear();
    // 强制执行的stdout 140布局规则( https://learnopengl.com/#!advanced-opengl/advanced-glsl), 顺序是 vec3 vec4 ivec4 vec2 float
    for (const auto& pair : attributesVec3)
    {
        m_vertexCount = std::max(m_vertexCount, (int)pair.second.size());
        m_attributeByName[pair.first] = { m_totalBytesPerVertex, 3, GL_FLOAT, GL_FLOAT_VEC3 };
        m_totalBytesPerVertex += sizeof(glm::vec4); // note use vec4 size
    }
    for (const auto& pair : attributesVec4)
    {
        m_vertexCount = std::max(m_vertexCount, (int)pair.second.size());
        m_attributeByName[pair.first] = { m_totalBytesPerVertex, 4, GL_FLOAT, GL_FLOAT_VEC4 };
        m_totalBytesPerVertex += sizeof(glm::vec4);
    }
    for (const auto& pair : attributesIVec4)
    {
        m_vertexCount = std::max(m_vertexCount, (int)pair.second.size());
        m_attributeByName[pair.first] = { m_totalBytesPerVertex, 4, GL_INT, GL_INT_VEC4 };
        m_totalBytesPerVertex += sizeof(glm::i32vec4);
    }
    for (const auto& pair : attributesVec2)
    {
        m_vertexCount = std::max(m_vertexCount, (int)pair.second.size());
        m_attributeByName[pair.first] = { m_totalBytesPerVertex, 2, GL_FLOAT, GL_FLOAT_VEC2 };
        m_totalBytesPerVertex += sizeof(glm::vec2);
    }
    for (auto& pair : attributesFloat)
    {
        m_vertexCount = std::max(m_vertexCount, (int)pair.second.size());
        m_attributeByName[pair.first] = { m_totalBytesPerVertex, 1, GL_FLOAT, GL_FLOAT };
        m_totalBytesPerVertex += sizeof(float);
    }
    //添加最后的padding(使顶点与 vec4对齐)
    if (m_totalBytesPerVertex % (sizeof(float) * 4) != 0)
    {
        m_totalBytesPerVertex += sizeof(float) * 4 - m_totalBytesPerVertex % (sizeof(float) * 4);
    }
    std::vector<float> interleavedData((m_vertexCount * m_totalBytesPerVertex) / sizeof(float), 0);
    const char* dataPtr = (char*)interleavedData.data();
    // 添加数据(将每个元素复制到interleaved buffer中)
    for (const auto& pair : attributesVec3)
    {
        auto& attribute = m_attributeByName[pair.first];
        for (int i = 0; i < pair.second.size(); i++)
        {
            auto* locationPtr = (glm::vec3*)(dataPtr + (m_totalBytesPerVertex * i) + attribute.offset);
            *locationPtr = pair.second[i];
        }
    }
    for (const auto& pair : attributesVec4)
    {
        auto& attribute = m_attributeByName[pair.first];
        for (int i = 0; i < pair.second.size(); i++)
        {
            auto* locationPtr = (glm::vec4*)(dataPtr + m_totalBytesPerVertex * i + attribute.offset);
            *locationPtr = pair.second[i];
        }
    }
    for (const auto& pair : attributesIVec4)
    {
        auto& attribute = m_attributeByName[pair.first];
        for (int i = 0; i < pair.second.size(); i++)
        {
            auto* locationPtr = (glm::i32vec4*)(dataPtr + m_totalBytesPerVertex * i + attribute.offset);
            *locationPtr = pair.second[i];
        }
    }
    for (const auto& pair : attributesVec2)
    {
        auto& attribute = m_attributeByName[pair.first];
        for (int i = 0; i < pair.second.size(); i++)
        {
            auto* locationPtr = (glm::vec2*)(dataPtr + m_totalBytesPerVertex * i + attribute.offset);
            *locationPtr = pair.second[i];
        }
    }
    for (const auto& pair : attributesFloat)
    {
        auto& attribute = m_attributeByName[pair.first];
        for (int i = 0; i < pair.second.size(); i++)
        {
            auto* locationPtr = (float*)(dataPtr + m_totalBytesPerVertex * i + attribute.offset);
            *locationPtr = pair.second[i];
        }
    }
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * interleavedData.size(), interleavedData.data(), GL_STATIC_DRAW);

    if (!indices.empty())
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
        GLsizeiptr indicesSize = indices.size() * sizeof(uint16_t);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize, indices.data(), GL_STATIC_DRAW);
    }
    m_indices = std::move(indices);
    m_attributesFloat = std::move(attributesFloat);
    m_attributesVec2 = std::move(attributesVec2);
    m_attributesVec3 = std::move(attributesVec3);
    m_attributesVec4 = std::move(attributesVec4);
    m_attributesIVec4 = std::move(attributesIVec4);
}

void Mesh::setVertexAttributePointers(Shader* shader)
{
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    int vertexAttribArray = 0;
    for (auto attribute : shader->m_attributes)
    {
        auto res = m_attributeByName.find(attribute.first);
        if (res != m_attributeByName.end() && attribute.second.type == res->second.attributeType && attribute.second.arraySize == 1)
        {
            glEnableVertexAttribArray(attribute.second.position);
            glVertexAttribPointer(attribute.second.position, res->second.elementCount, res->second.dataType, GL_FALSE, m_totalBytesPerVertex, (void*)(res->second.offset));
            vertexAttribArray++;
        }
        else
        {
            glDisableVertexAttribArray(attribute.second.position);
            switch (attribute.second.type)
            {
            case GL_FLOAT:
                if (attribute.second.arraySize == 1)
                {
                    glVertexAttrib1f(attribute.second.position, 0);
                }
                else if (attribute.second.arraySize == 2)
                {
                    glVertexAttrib2f(attribute.second.position, 0, 0);
                }
                else if (attribute.second.arraySize == 3)
                {
                    glVertexAttrib3f(attribute.second.position, 0, 0, 0);
                }
                else if (attribute.second.arraySize == 4)
                {
                    glVertexAttrib4f(attribute.second.position, 0, 0, 0, 0);
                }
                break;
            case GL_INT:
                if (attribute.second.arraySize == 1)
                {
                    glVertexAttribI1i(attribute.second.position, 0);
                }
                else if (attribute.second.arraySize == 2)
                {
                    glVertexAttribI2i(attribute.second.position, 0, 0);
                }
                else if (attribute.second.arraySize == 3)
                {
                    glVertexAttribI3i(attribute.second.position, 0, 0, 0);
                }
                else if (attribute.second.arraySize == 4)
                {
                    glVertexAttribI4i(attribute.second.position, 0, 0, 0, 0);
                }
                break;
            }
        }
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
    builder.m_attributesFloat = m_attributesFloat;
    builder.m_attributesVec2 = m_attributesVec2;
    builder.m_attributesVec3 = m_attributesVec3;
    builder.m_attributesVec4 = m_attributesVec4;
    builder.m_attributesIVec4 = m_attributesIVec4;
    builder.m_indices = m_indices;
    builder.m_topology = m_topology;
    return builder;
}

size_t Mesh::getDataSize()
{
    return m_totalBytesPerVertex;
}

std::vector<glm::vec3> Mesh::getPositions()
{
    std::vector<glm::vec3> position;
    auto ref = m_attributesVec3.find("position");
    if (ref != m_attributesVec3.end())
    {
        position = ref->second;
    }
    return position;
}

std::vector<glm::vec3> Mesh::getNormals()
{
    std::vector<glm::vec3> normal;
    auto ref = m_attributesVec3.find("normal");
    if (ref != m_attributesVec3.end())
    {
        normal = ref->second;
    }
    return normal;
}

std::vector<glm::vec4> Mesh::getUVs()
{
    std::vector<glm::vec4> uv;
    auto ref = m_attributesVec4.find("uv");
    if (ref != m_attributesVec4.end())
    {
        uv = ref->second;
    }
    return uv;
}

std::vector<glm::vec4> Mesh::getColors()
{
    std::vector<glm::vec4> color;
    auto ref = m_attributesVec4.find("color");
    if (ref != m_attributesVec4.end())
    {
        color = ref->second;
    }
    return color;
}

std::vector<float> Mesh::getParticleSizes()
{
    std::vector<float> particleSize;
    auto ref = m_attributesFloat.find("particleSize");
    if (ref != m_attributesFloat.end())
    {
        particleSize = ref->second;
    }
    return particleSize;
}

template <typename T>
T Mesh::get(std::string_view attributeName)
{
    return nullptr;
}

template <>
const std::vector<float>& Mesh::get(std::string_view uniformName)
{
    return m_attributesFloat[uniformName.data()];
}

template <>
const std::vector<glm::vec2>& Mesh::get(std::string_view uniformName)
{
    return m_attributesVec2[uniformName.data()];
}

template <>
const std::vector<glm::vec3>& Mesh::get(std::string_view uniformName)
{
    return m_attributesVec3[uniformName.data()];
}

template <>
const std::vector<glm::vec4>& Mesh::get(std::string_view uniformName)
{
    return m_attributesVec4[uniformName.data()];
}

template <>
const std::vector<glm::ivec4>& Mesh::get(std::string_view uniformName)
{
    return m_attributesIVec4[uniformName.data()];
}

std::pair<int, int> Mesh::getType(std::string_view name)
{
    auto ret = m_attributeByName.find(name.data());
    if (ret != m_attributeByName.end())
    {
        return { ret->second.dataType, ret->second.elementCount };
    }
    return { -1, -1 };
}

std::vector<std::string> Mesh::getNames()
{
    std::vector<std::string> names;
    for (auto& u : m_attributeByName)
    {
        names.push_back(u.first);
    }
    return names;
}

std::array<glm::vec3, 2> Mesh::getBoundsMinMax()
{
    return m_boundsMinMax;
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
    withPositions(vertices);
    withNormals(normals);
    withUvs(uvs);
    withIndices(indices);
    withMeshTopology(Topology::Triangles);
    return *this;
}

Mesh::MeshBuilder& Mesh::MeshBuilder::withCube(float length)
{
    using namespace glm;
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
    withPositions(positions);
    withNormals(normals);
    withUvs(uvs);
    withMeshTopology(Topology::Triangles);
    return *this;
}

Mesh::MeshBuilder& Mesh::MeshBuilder::withSphere(int stacks, int slices, float radius)
{
    using namespace glm;
    size_t vertexCount = ((stacks + 1) * slices + 1);
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
        for (int i = 0; i <= slices; i++)
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
    for (int j = 0; j <= stacks; j++)
    {
        for (int i = 0; i <= slices; i++)
        {
            glm::u8vec2 offset[] = {
                // first triangle
                { i, j },
                { (i + 1) % (slices+1), j + 1 },
                { (i + 1) % (slices+1), j },

                // second triangle
                { i, j },
                { i, j + 1 },
                { (i + 1) % (slices+1), j + 1 }
            };
            for (const auto& o : offset)
            {
                index = o[1] * (slices+1) + o[0];
                finalPosition.push_back(vertices[index]);
                finalNormals.push_back(normals[index]);
                finalUVs.push_back(uvs[index]);
            }
        }
    }
    withPositions(finalPosition);
    withNormals(finalNormals);
    withUvs(finalUVs);
    withMeshTopology(Topology::Triangles);
    return *this;
}

Mesh::MeshBuilder& Mesh::MeshBuilder::withPositions(const std::vector<glm::vec3>& position)
{
    withUniform("position", position);
    return *this;
}

Mesh::MeshBuilder& Mesh::MeshBuilder::withNormals(const std::vector<glm::vec3>& normal)
{
    withUniform("normal", normal);
    return *this;
}

Mesh::MeshBuilder& Mesh::MeshBuilder::withUvs(const std::vector<glm::vec4>& uv)
{
    withUniform("uv", uv);
    return *this;
}

Mesh::MeshBuilder& Mesh::MeshBuilder::withColors(const std::vector<glm::vec4>& colors)
{
    withUniform("color", colors);
    return *this;
}

Mesh::MeshBuilder& Mesh::MeshBuilder::withParticleSizes(const std::vector<float>& particleSize)
{
    withUniform("particleSize", particleSize);
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

std::shared_ptr<Mesh> Mesh::MeshBuilder::build()
{
    // update stats
    RenderStats& renderStats = Renderer::s_instance->m_renderStatsCurrent;
    Mesh* mesh;
    if (m_updateMesh != nullptr)
    {
        renderStats.meshBytes -= m_updateMesh->getDataSize();
        m_updateMesh->update(m_attributesFloat, m_attributesVec2, m_attributesVec3, m_attributesVec4, m_attributesIVec4, m_indices, m_topology);
        renderStats.meshBytes += mesh->getDataSize();
        return m_updateMesh->shared_from_this();
    }
    else
    {
        auto* mesh = new Mesh(m_attributesFloat, m_attributesVec2, m_attributesVec3, m_attributesVec4, m_attributesIVec4, m_indices, m_topology);
        renderStats.meshCount++;
        return std::shared_ptr<Mesh>(mesh);
    }
}

Mesh::MeshBuilder& Mesh::MeshBuilder::withUniform(std::string_view name, const std::vector<float>& values)
{
    if (m_updateMesh != nullptr && m_attributesFloat.find(name.data()) == m_attributesFloat.end())
    {
        LOG_ERROR("Cannot change mesh structure. {} did not exist in original mesh.", name.data());
    }
    else
    {
        m_attributesFloat[name.data()] = values;
    }
    return *this;
}

Mesh::MeshBuilder& Mesh::MeshBuilder::withUniform(std::string_view name, const std::vector<glm::vec2>& values)
{
    if (m_updateMesh != nullptr && m_attributesVec2.find(name.data()) == m_attributesVec2.end())
    {
        LOG_ERROR("Cannot change mesh structure. {} did not exist in original mesh.", name.data());
    }
    else
    {
        m_attributesVec2[name.data()] = values;
    }
    return *this;
}

Mesh::MeshBuilder& Mesh::MeshBuilder::withUniform(std::string_view name, const std::vector<glm::vec3>& values)
{
    if (m_updateMesh != nullptr && m_attributesVec3.find(name.data()) == m_attributesVec3.end())
    {
        LOG_ERROR("Cannot change mesh structure. {} did not exist in original mesh.", name.data());
    }
    else
    {
        m_attributesVec3[name.data()] = values;
    }
    return *this;
}

Mesh::MeshBuilder& Mesh::MeshBuilder::withUniform(std::string_view name, const std::vector<glm::vec4>& values)
{
    if (m_updateMesh != nullptr && m_attributesVec4.find(name.data()) == m_attributesVec4.end())
    {
        LOG_ERROR("Cannot change mesh structure. {} did not exist in original mesh.", name.data());
    }
    else
    {
        m_attributesVec4[name.data()] = values;
    }
    return *this;
}

Mesh::MeshBuilder& Mesh::MeshBuilder::withUniform(std::string_view name, const std::vector<glm::i32vec4>& values)
{
    if (m_updateMesh != nullptr && m_attributesIVec4.find(name.data()) == m_attributesIVec4.end())
    {
        LOG_ERROR("Cannot change mesh structure. {} did not exist in original mesh.", name.data());
    }
    else
    {
        m_attributesIVec4[name.data()] = values;
    }
    return *this;
}

} // namespace re