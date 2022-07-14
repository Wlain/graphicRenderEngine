
//
// Created by william on 2022/5/22.
//

#include "mesh.h"

#include "glCommonDefine.h"
#include "renderStats.h"
#include "renderer.h"
#include "shader.h"

#include <glm/gtc/constants.hpp>
#include <iomanip>
namespace re
{
Mesh::Mesh(std::map<std::string, std::vector<float>>&& attributesFloat, std::map<std::string, std::vector<glm::vec2>>&& attributesVec2, std::map<std::string, std::vector<glm::vec3>>&& attributesVec3,
           std::map<std::string, std::vector<glm::vec4>>&& attributesVec4, std::map<std::string, std::vector<glm::i32vec4>>&& attributesIVec4, std::vector<std::vector<uint16_t>>&& indices,
           std::vector<Topology>& meshTopology, BufferUsage bufferUsage, std::string_view name, RenderStats& renderStats)
{
    m_meshId = m_meshIdCount++;
    if (Renderer::s_instance == nullptr)
    {
        throw std::runtime_error("Cannot instantiate re::Mesh before re::Renderer is created.");
    }
    glGenBuffers(1, &m_vbo);
    update(std::move(attributesFloat), std::move(attributesVec2), std::move(attributesVec3), std::move(attributesVec4), std::move(attributesIVec4), std::move(indices), meshTopology, bufferUsage, name, renderStats);
    Renderer::s_instance->m_meshes.emplace_back(this);
}

Mesh::~Mesh()
{
    auto* r = Renderer::s_instance;
    if (r != nullptr)
    {
        RenderStats& renderStats = r->m_renderStatsCurrent;
        auto dataSize = getDataSize();
        renderStats.meshBytes -= dataSize;
        renderStats.meshBytesDeallocated += dataSize;
        renderStats.meshCount--;
        if (!r->m_meshes.empty())
        {
            // 指定删除某个元素
            r->m_meshes.erase(std::remove(r->m_meshes.begin(), r->m_meshes.end(), this));
        }
    }
    for (auto obj : m_shaderToVao)
    {
        glDeleteVertexArrays(1, &(obj.second.vaoID));
    }
    glDeleteBuffers(1, &m_vbo);

    if (m_ebo != 0)
    {
        glDeleteBuffers(1, &m_ebo);
    }
}

void Mesh::bind(Shader* shader)
{
    auto res = m_shaderToVao.find(shader->m_id);
    if (res != m_shaderToVao.end() && res->second.shaderId == shader->m_shaderUniqueId)
    {
        GLuint vao = res->second.vaoID;
        glBindVertexArray(vao);
    }
    else
    {
        GLuint index;
        if (res != m_shaderToVao.end())
        {
            index = res->second.vaoID;
        }
        else
        {
            glGenVertexArrays(1, &index);
        }
        glBindVertexArray(index);
        setVertexAttributePointers(shader);
        m_shaderToVao[shader->m_id] = { shader->m_shaderUniqueId, index };
        bindIndexSet();
    }
}

void Mesh::bindIndexSet() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
}

void Mesh::update(std::map<std::string, std::vector<float>>&& attributesFloat, std::map<std::string, std::vector<glm::vec2>>&& attributesVec2, std::map<std::string, std::vector<glm::vec3>>&& attributesVec3, std::map<std::string, std::vector<glm::vec4>>&& attributesVec4, std::map<std::string, std::vector<glm::i32vec4>>&& attributesIVec4, std::vector<std::vector<uint16_t>>&& indices, std::vector<Topology>& meshTopology, BufferUsage bufferUsage, std::string_view name, RenderStats& renderStats)
{
    m_vertexCount = 0;
    m_dataSize = 0;
    for (const auto& obj : m_shaderToVao)
    {
        glDeleteVertexArrays(1, &(obj.second.vaoID));
    }
    m_shaderToVao.clear();
    m_attributeByName.clear();
    m_attributesFloat = std::move(attributesFloat);
    m_attributesVec2 = std::move(attributesVec2);
    m_attributesVec3 = std::move(attributesVec3);
    m_attributesVec4 = std::move(attributesVec4);
    m_attributesIVec4 = std::move(attributesIVec4);
    m_indices = std::move(indices);
    m_topologies = std::move(meshTopology);
    m_bufferUsage = bufferUsage;
    auto interleavedData = getInterleavedData();
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * interleavedData.size(), interleavedData.data(), GL_STATIC_DRAW);
    updateIndexBuffers();
    m_boundsMinMax[0] = glm::vec3{ std::numeric_limits<float>::max() };
    m_boundsMinMax[1] = glm::vec3{ -std::numeric_limits<float>::max() };
    auto pos = m_attributesVec3.find("position");
    if (pos != m_attributesVec3.end())
    {
        for (const auto& v : pos->second)
        {
            m_boundsMinMax[0] = glm::min(m_boundsMinMax[0], v);
            m_boundsMinMax[1] = glm::max(m_boundsMinMax[1], v);
        }
    }
    m_dataSize = m_totalBytesPerVertex * m_vertexCount;
    renderStats.meshBytes += m_dataSize;
    renderStats.meshBytesAllocated += m_dataSize;
    m_name = name;
    m_meshId = m_meshIdCount++;
}

void Mesh::setVertexAttributePointers(Shader* shader)
{
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    int vertexAttribArray = 0;
    for (auto& attribute : shader->m_attributes)
    {
        auto meshAttribute = m_attributeByName.find(attribute.first);
        bool attributeFoundInMesh = meshAttribute != m_attributeByName.end();
        bool equalType = attributeFoundInMesh && (attribute.second.type == meshAttribute->second.attributeType || (attribute.second.type >= GL_FLOAT_VEC2 && attribute.second.type <= GL_FLOAT_VEC4 && attribute.second.type >= meshAttribute->second.attributeType) || (attribute.second.type >= GL_INT_VEC2 && attribute.second.type <= GL_INT_VEC4 && attribute.second.type >= meshAttribute->second.attributeType));
        if (equalType && attribute.second.arraySize == 1)
        {
            glEnableVertexAttribArray(attribute.second.position);
            glVertexAttribPointer(attribute.second.position, meshAttribute->second.elementCount, meshAttribute->second.dataType, GL_FALSE, m_totalBytesPerVertex, (char*)nullptr + meshAttribute->second.offset);
            vertexAttribArray++;
        }
        else
        {
            ASSERT(attribute.second.arraySize == 1 && "constant vertex attributes not supported as arrays");
            glDisableVertexAttribArray(attribute.second.position);
            constexpr static const float a[] = { 0, 0, 0, 0,
                                                 0, 0, 0, 0,
                                                 0, 0, 0, 0,
                                                 0, 0, 0, 0 };
            switch (attribute.second.type)
            {
            case GL_INT_VEC4:
                glVertexAttribI4iv(attribute.second.position, (GLint*)a);
                break;
            case GL_INT_VEC3:
                glVertexAttribI3iv(attribute.second.position, (GLint*)a);
                break;
            case GL_INT_VEC2:
                glVertexAttribI2iv(attribute.second.position, (GLint*)a);
                break;
            case GL_INT:
                glVertexAttribI1iv(attribute.second.position, (GLint*)a);
                break;
            case GL_FLOAT_VEC4:
                glVertexAttrib4fv(attribute.second.position, a);
                break;
            case GL_FLOAT_VEC3:
                glVertexAttrib3fv(attribute.second.position, a);
                break;
            case GL_FLOAT_VEC2:
                glVertexAttrib2fv(attribute.second.position, a);
                break;
            case GL_FLOAT:
                glVertexAttrib1fv(attribute.second.position, a);
                break;
            default:
                LOG_ERROR("Unhandled attribute type:{}", (int)attribute.second.type);
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
    builder.m_topologies = m_topologies;
    return builder;
}

int Mesh::getDataSize() const
{
    return m_dataSize;
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

std::vector<glm::vec4> Mesh::getTangents()
{
    std::vector<glm::vec4> color;
    auto ref = m_attributesVec4.find("tangent");
    if (ref != m_attributesVec4.end())
    {
        color = ref->second;
    }
    return color;
}

[[maybe_unused]] std::vector<float> Mesh::getParticleSizes()
{
    std::vector<float> particleSize;
    auto ref = m_attributesFloat.find("particleSize");
    if (ref != m_attributesFloat.end())
    {
        particleSize = ref->second;
    }
    return particleSize;
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
const std::vector<glm::i32vec4>& Mesh::get(std::string_view uniformName)
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

std::vector<std::string> Mesh::getAttributeNames()
{
    std::vector<std::string> names;
    for (auto& u : m_attributeByName)
    {
        names.push_back(u.first);
    }
    return names;
}

const std::array<glm::vec3, 2>& Mesh::getBoundsMinMax() const
{
    return m_boundsMinMax;
}

void Mesh::setBoundsMinMax(const std::array<glm::vec3, 2>& minMax)
{
    m_boundsMinMax = minMax;
}

Mesh::Topology Mesh::getMeshTopology(int indexSet)
{
    if (m_topologies.empty())
    {
        m_topologies.emplace_back();
        m_topologies[0] = Mesh::Topology::Triangles;
    }
    return m_topologies[indexSet];
}

Mesh::BufferUsage Mesh::getMeshBufferUsage()
{
    return m_bufferUsage;
}

std::vector<float> Mesh::getInterleavedData()
{
    m_totalBytesPerVertex = 0;
    // 强制执行的stdout 140布局规则( https://learnopengl.com/#!advanced-opengl/advanced-glsl), 顺序是 vec3 vec4 ivec4 vec2 float
    for (const auto& pair : m_attributesVec3)
    {
        m_vertexCount = std::max(m_vertexCount, (int)pair.second.size());
        m_attributeByName[pair.first] = { m_totalBytesPerVertex, 3, GL_FLOAT, GL_FLOAT_VEC3 };
        m_totalBytesPerVertex += sizeof(glm::vec4); // note use vec4 size
    }
    for (const auto& pair : m_attributesVec4)
    {
        m_vertexCount = std::max(m_vertexCount, (int)pair.second.size());
        m_attributeByName[pair.first] = { m_totalBytesPerVertex, 4, GL_FLOAT, GL_FLOAT_VEC4 };
        m_totalBytesPerVertex += sizeof(glm::vec4);
    }
    for (const auto& pair : m_attributesIVec4)
    {
        m_vertexCount = std::max(m_vertexCount, (int)pair.second.size());
        m_attributeByName[pair.first] = { m_totalBytesPerVertex, 4, GL_INT, GL_INT_VEC4 };
        m_totalBytesPerVertex += sizeof(glm::i32vec4);
    }
    for (const auto& pair : m_attributesVec2)
    {
        m_vertexCount = std::max(m_vertexCount, (int)pair.second.size());
        m_attributeByName[pair.first] = { m_totalBytesPerVertex, 2, GL_FLOAT, GL_FLOAT_VEC2 };
        m_totalBytesPerVertex += sizeof(glm::vec2);
    }
    for (auto& pair : m_attributesFloat)
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
    for (const auto& pair : m_attributesVec3)
    {
        auto& attribute = m_attributeByName[pair.first];
        for (int i = 0; i < pair.second.size(); i++)
        {
            auto* locationPtr = (glm::vec3*)(dataPtr + (m_totalBytesPerVertex * i) + attribute.offset);
            *locationPtr = pair.second[i];
        }
    }
    for (const auto& pair : m_attributesVec4)
    {
        auto& attribute = m_attributeByName[pair.first];
        for (int i = 0; i < pair.second.size(); i++)
        {
            auto* locationPtr = (glm::vec4*)(dataPtr + m_totalBytesPerVertex * i + attribute.offset);
            *locationPtr = pair.second[i];
        }
    }
    for (const auto& pair : m_attributesIVec4)
    {
        auto& attribute = m_attributeByName[pair.first];
        for (int i = 0; i < pair.second.size(); i++)
        {
            auto* locationPtr = (glm::i32vec4*)(dataPtr + m_totalBytesPerVertex * i + attribute.offset);
            *locationPtr = pair.second[i];
        }
    }
    for (const auto& pair : m_attributesVec2)
    {
        auto& attribute = m_attributeByName[pair.first];
        for (int i = 0; i < pair.second.size(); i++)
        {
            auto* locationPtr = (glm::vec2*)(dataPtr + m_totalBytesPerVertex * i + attribute.offset);
            *locationPtr = pair.second[i];
        }
    }
    for (const auto& pair : m_attributesFloat)
    {
        auto& attribute = m_attributeByName[pair.first];
        for (int i = 0; i < pair.second.size(); i++)
        {
            auto* locationPtr = (float*)(dataPtr + m_totalBytesPerVertex * i + attribute.offset);
            *locationPtr = pair.second[i];
        }
    }
    return interleavedData;
}

Mesh::MeshBuilder& Mesh::MeshBuilder::withQuad(float size)
{
    if (m_name.empty())
    {
        std::stringstream ss;
        ss << "re Quad -" << std::setprecision(2) << size;
        m_name = ss.str();
    }
    std::vector<glm::vec3> vertices({ { size, -size, 0 },
                                      { size, size, 0 },
                                      { -size, -size, 0 },
                                      { -size, size, 0 } });

    std::vector<glm::vec3> normals(4, glm::vec3{ 0, 0, 1 });
    std::vector<glm::vec4> uvs({ { 1, 0, 0, 0 },
                                 { 1, 1, 0, 0 },
                                 { 0, 0, 0, 0 },
                                 { 0, 1, 0, 0 } });
    std::vector<uint16_t> indices = {
        0, 1, 2,
        2, 1, 3
    };
    std::vector<glm::vec4> tangents({ glm::vec4{ 1, 0, 0, 1 },
                                      glm::vec4{ 1, 0, 0, 1 },
                                      glm::vec4{ 1, 0, 0, 1 },
                                      glm::vec4{ 1, 0, 0, 1 } });
    withPositions(vertices);
    withNormals(normals);
    withUvs(uvs);
    withTangents(tangents);
    withIndices(indices);
    withMeshTopology(Topology::Triangles);
    return *this;
}

Mesh::MeshBuilder& Mesh::MeshBuilder::withCube(float length)
{
    //    v5----- v4
    //   /|      /|
    //  v1------v0|
    //  | |     | |
    //  | |v6---|-|v7
    //  |/      |/
    //  v2------v3
    if (m_name.empty())
    {
        std::stringstream ss;
        ss << "re Cube -" << std::setprecision(2) << length;
        m_name = ss.str();
    }
    glm::vec3 p[] = { { length, length, length },
                      { -length, length, length },
                      { -length, -length, length },
                      { length, -length, length },

                      { length, length, -length },
                      { -length, length, -length },
                      { -length, -length, -length },
                      { length, -length, -length } };

    std::vector<uint16_t> indices({ 0, 1, 2, 0, 2, 3,
                                    4, 5, 6, 4, 6, 7,
                                    8, 9, 10, 8, 10, 11,
                                    12, 13, 14, 12, 14, 15,
                                    16, 17, 18, 16, 18, 19,
                                    20, 21, 22, 20, 22, 23 });
    std::vector<glm::vec3> positions({
        p[0], p[1], p[2], p[3], // v0-v1-v2-v3
        p[4], p[0], p[3], p[7], // v4-v0-v3-v7
        p[5], p[4], p[7], p[6], // v5-v4-v7-v6
        p[1], p[5], p[6], p[2], // v1-v5-v6-v2
        p[4], p[5], p[1], p[0], // v1-v5-v6-v2
        p[3], p[2], p[6], p[7], // v1-v5-v6-v2
    });
    glm::vec4 u[] = { { 1, 1, 0, 0 },
                      { 0, 1, 0, 0 },
                      { 0, 0, 0, 0 },
                      { 1, 0, 0, 0 } };

    std::vector<glm::vec4> uvs({ u[0], u[1], u[2], u[3],
                                 u[0], u[1], u[2], u[3],
                                 u[0], u[1], u[2], u[3],
                                 u[0], u[1], u[2], u[3],
                                 u[0], u[1], u[2], u[3],
                                 u[0], u[1], u[2], u[3] });
    std::vector<glm::vec3> normals({ { 0, 0, 1 },
                                     { 0, 0, 1 },
                                     { 0, 0, 1 },
                                     { 0, 0, 1 },
                                     { 1, 0, 0 },
                                     { 1, 0, 0 },
                                     { 1, 0, 0 },
                                     { 1, 0, 0 },
                                     { 0, 0, -1 },
                                     { 0, 0, -1 },
                                     { 0, 0, -1 },
                                     { 0, 0, -1 },
                                     { -1, 0, 0 },
                                     { -1, 0, 0 },
                                     { -1, 0, 0 },
                                     { -1, 0, 0 },
                                     { 0, 1, 0 },
                                     { 0, 1, 0 },
                                     { 0, 1, 0 },
                                     { 0, 1, 0 },
                                     { 0, -1, 0 },
                                     { 0, -1, 0 },
                                     { 0, -1, 0 },
                                     { 0, -1, 0 } });
    std::vector<glm::vec4> tangents({
        { 1, 0, 0, 1 },
        { 1, 0, 0, 1 },
        { 1, 0, 0, 1 },
        { 1, 0, 0, 1 },
        { 0, 0, -1, 1 },
        { 0, 0, -1, 1 },
        { 0, 0, -1, 1 },
        { 0, 0, -1, 1 },
        { -1, 0, 0, 1 },
        { -1, 0, 0, 1 },
        { -1, 0, 0, 1 },
        { -1, 0, 0, 1 },
        { 0, 0, 1, 1 },
        { 0, 0, 1, 1 },
        { 0, 0, 1, 1 },
        { 0, 0, 1, 1 },
        { 1, 0, 0, 1 },
        { 1, 0, 0, 1 },
        { 1, 0, 0, 1 },
        { 1, 0, 0, 1 },
        { -1, 0, 0, 1 },
        { -1, 0, 0, 1 },
        { -1, 0, 0, 1 },
        { -1, 0, 0, 1 },
    });
    withPositions(positions);
    withNormals(normals);
    withUvs(uvs);
    withTangents(tangents);
    withIndices(indices);
    withMeshTopology(Topology::Triangles);
    return *this;
}

Mesh::MeshBuilder& Mesh::MeshBuilder::withSphere(int stacks, int slices, float radius)
{
    using namespace glm;
    if (m_name.empty())
    {
        std::stringstream ss;
        ss << "re Sphere " << stacks << "-" << slices << "-" << std::setprecision(2) << radius;
        m_name = ss.str();
    }
    size_t vertexCount = ((stacks + 1) * (slices + 1));
    std::vector<vec3> vertices{ vertexCount };
    std::vector<vec3> normals{ vertexCount };
    std::vector<vec4> tangents{ vertexCount };
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
            vec4 tangent = vec4((vec3)normalize(dvec3(cos(longitude + glm::half_pi<double>()), 0, sin(longitude + glm::half_pi<double>()))), 1);
            normal = normalize(normal);
            tangents[index] = tangent;
            normals[index] = normal;
            uvs[index] = vec4{ 1 - (float)i / (float)slices, (float)j / (float)stacks, 0, 0 };
            vertices[index] = normal * radius;
            index++;
        }
    }
    std::vector<vec3> finalPosition;
    std::vector<vec3> finalNormals;
    std::vector<vec4> finalTangents;
    std::vector<vec4> finalUVs;
    // create indices
    for (int j = 0; j < stacks; j++)
    {
        for (int i = 0; i <= slices; i++)
        {
            glm::u8vec2 offset[] = {
                // first triangle
                { i, j },
                { (i + 1) % (slices + 1), j + 1 },
                { (i + 1) % (slices + 1), j },

                // second triangle
                { i, j },
                { i, j + 1 },
                { (i + 1) % (slices + 1), j + 1 }
            };
            for (const auto& o : offset)
            {
                index = o[1] * (slices + 1) + o[0];
                finalPosition.push_back(vertices[index]);
                finalNormals.push_back(normals[index]);
                finalTangents.push_back(tangents[index]);
                finalUVs.push_back(uvs[index]);
            }
        }
    }
    withPositions(finalPosition);
    withNormals(finalNormals);
    withTangents(finalTangents);
    withUvs(finalUVs);
    withMeshTopology(Topology::Triangles);
    return *this;
}

Mesh::MeshBuilder& Mesh::MeshBuilder::withPositions(const std::vector<glm::vec3>& position)
{
    withAttribute("position", position);
    return *this;
}

Mesh::MeshBuilder& Mesh::MeshBuilder::withNormals(const std::vector<glm::vec3>& normal)
{
    withAttribute("normal", normal);
    return *this;
}

Mesh::MeshBuilder& Mesh::MeshBuilder::withUvs(const std::vector<glm::vec4>& uv)
{
    withAttribute("uv", uv);
    return *this;
}

Mesh::MeshBuilder& Mesh::MeshBuilder::withColors(const std::vector<glm::vec4>& colors)
{
    withAttribute("color", colors);
    return *this;
}

Mesh::MeshBuilder& Mesh::MeshBuilder::withTangents(const std::vector<glm::vec4>& tangent)
{
    withAttribute("tangent", tangent);
    return *this;
}

Mesh::MeshBuilder& Mesh::MeshBuilder::withParticleSizes(const std::vector<float>& particleSize)
{
    withAttribute("particleSize", particleSize);
    return *this;
}

Mesh::MeshBuilder& Mesh::MeshBuilder::withMeshTopology(Mesh::Topology topology)
{
    if (m_topologies.empty())
    {
        m_topologies.emplace_back();
    }
    m_topologies[0] = topology;
    return *this;
}

Mesh::MeshBuilder& Mesh::MeshBuilder::withUsage(BufferUsage usage)
{
    m_bufferUsage = usage;
    return *this;
}

Mesh::MeshBuilder& Mesh::MeshBuilder::withIndices(const std::vector<uint16_t>& indices, Topology topology, int indexSet)
{
    while (indexSet >= m_indices.size())
    {
        m_indices.emplace_back();
    }
    while (indexSet >= m_topologies.size())
    {
        m_topologies.emplace_back();
    }
    m_indices[indexSet] = indices;
    m_topologies[indexSet] = topology;
    return *this;
}

Mesh::MeshBuilder& Mesh::MeshBuilder::withName(std::string_view name)
{
    m_name = name;
    return *this;
}

Mesh::MeshBuilder& Mesh::MeshBuilder::withRecomputeNormals(bool enabled)
{
    m_recomputeNormals = enabled;
    return *this;
}

Mesh::MeshBuilder& Mesh::MeshBuilder::withRecomputeTangents(bool enabled)
{
    m_recomputeTangents = enabled;
    return *this;
}

std::shared_ptr<Mesh> Mesh::MeshBuilder::build()
{
    // update stats
    RenderStats& renderStats = Renderer::s_instance->m_renderStatsCurrent;
    if (m_name.empty())
    {
        m_name = "Unnamed Mesh";
    }
    if (m_recomputeNormals)
    {
        auto newNormals = computeNormals();
        if (!newNormals.empty())
        {
            withNormals(newNormals);
        }
    }

    if (m_recomputeTangents)
    {
        bool hasNormals = m_attributesVec3.find("normal") == m_attributesVec3.end();
        auto newTangents = computeTangents(hasNormals ? m_attributesVec3["normal"] : computeNormals());
        if (!newTangents.empty())
        {
            withTangents(newTangents);
        }
    }
    if (m_updateMesh != nullptr)
    {
        renderStats.meshBytes -= m_updateMesh->getDataSize();
        m_updateMesh->update(std::move(m_attributesFloat), std::move(m_attributesVec2), std::move(m_attributesVec3), std::move(m_attributesVec4), std::move(m_attributesIVec4), std::move(m_indices), m_topologies, m_bufferUsage, m_name, renderStats);
        return m_updateMesh->shared_from_this();
    }
    auto* mesh = new Mesh(std::move(m_attributesFloat), std::move(m_attributesVec2), std::move(m_attributesVec3), std::move(m_attributesVec4), std::move(m_attributesIVec4), std::move(m_indices), m_topologies, m_bufferUsage, m_name, renderStats);
    renderStats.meshCount++;
    return std::shared_ptr<Mesh>(mesh);
}

Mesh::MeshBuilder& Mesh::MeshBuilder::withAttribute(std::string_view name, const std::vector<float>& values)
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

Mesh::MeshBuilder& Mesh::MeshBuilder::withAttribute(std::string_view name, const std::vector<glm::vec2>& values)
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

Mesh::MeshBuilder& Mesh::MeshBuilder::withAttribute(std::string_view name, const std::vector<glm::vec3>& values)
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

Mesh::MeshBuilder& Mesh::MeshBuilder::withAttribute(std::string_view name, const std::vector<glm::vec4>& values)
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

Mesh::MeshBuilder& Mesh::MeshBuilder::withTorus(int segmentsC, int segmentsA, float radiusC, float radiusA)
{
    //  losely based on http://mathworld.wolfram.com/Torus.html
    if (m_name.empty())
    {
        std::stringstream ss;
        ss << "re Torus " << segmentsC << "-" << segmentsA << "-" << std::setprecision(2) << radiusC << "-" << radiusA;
        m_name = ss.str();
    }

    auto vertexCount = (size_t)((segmentsC + 1) * (segmentsA + 1));
    std::vector<glm::vec3> vertices{ vertexCount };
    std::vector<glm::vec3> normals{ vertexCount };
    std::vector<glm::vec4> tangents{ vertexCount };
    std::vector<glm::vec4> uvs{ vertexCount };
    int index = 0;

    // create vertices
    for (int j = 0; j <= segmentsC; j++)
    {
        // outer circle
        float u = glm::two_pi<float>() * (float)j / (float)segmentsC;
        auto t = glm::vec4((glm::vec3)normalize(glm::dvec3(cos(u + glm::half_pi<double>()), sin(u + glm::half_pi<double>()), 0)), 1);
        for (int i = 0; i <= segmentsA; i++)
        {
            float v = glm::two_pi<float>() * (float)i / (float)segmentsA;
            glm::vec3 pos{
                (radiusC + radiusA * cos(v)) * cos(u),
                (radiusC + radiusA * cos(v)) * sin(u),
                radiusA * sin(v)
            };
            glm::vec3 tangent{
                (radiusC + radiusA * cos(v)) * cos(u),
                (radiusC + radiusA * cos(v)) * sin(u),
                radiusA * sin(v)
            };
            glm::vec3 posOuter{
                (radiusC + (radiusA * 2) * cos(v)) * cos(u),
                (radiusC + (radiusA * 2) * cos(v)) * sin(u),
                (radiusA * 2) * sin(v)
            };
            tangents[index] = t;
            uvs[index] = glm::vec4{ 1 - (float)j / (float)segmentsC, (float)i / (float)segmentsA, 0, 0 };
            vertices[index] = pos;
            normals[index] = glm::normalize(posOuter - pos);
            index++;
        }
    }
    std::vector<glm::vec3> finalPosition;
    std::vector<glm::vec3> finalNormals;
    std::vector<glm::vec4> finalUVs;
    std::vector<glm::vec4> finalTangents;
    // create indices
    for (int j = 0; j < segmentsC; j++)
    {
        for (int i = 0; i <= segmentsA; i++)
        {
            glm::u8vec2 offset[] = {
                // first triangle
                { i, j },
                { (i + 1) % (segmentsA + 1), j + 1 },
                { (i + 1) % (segmentsA + 1), j },

                // second triangle
                { i, j },
                { i, j + 1 },
                { (i + 1) % (segmentsA + 1), j + 1 },

            };
            for (auto o : offset)
            {
                index = o[1] * (segmentsA + 1) + o[0];
                finalPosition.push_back(vertices[index]);
                finalNormals.push_back(normals[index]);
                finalUVs.push_back(uvs[index]);
            }
        }
    }
    withPositions(finalPosition);
    withNormals(finalNormals);
    withUvs(finalUVs);
    withTangents(finalTangents);
    withMeshTopology(Mesh::Topology::Triangles);
    return *this;
}

Mesh::MeshBuilder& Mesh::MeshBuilder::withWireCube(float length)
{
    if (m_name.length() == 0)
    {
        std::stringstream ss;
        ss << "WireCube " << std::setprecision(2) << length;
        m_name = ss.str();
    }

    std::vector<glm::vec3> positions({ { length, length, length },
                                       { -length, length, length },
                                       { -length, -length, length },
                                       { length, -length, length },

                                       { length, length, -length },
                                       { -length, length, -length },
                                       { -length, -length, -length },
                                       { length, -length, -length } });
    std::vector<uint16_t> indices;

    for (int i = 0; i < positions.size(); i++)
    {
        for (int j = 0; j < i; j++)
        {
            if (glm::length(positions[i] - positions[j]) <= length * 2.1f)
            {
                indices.push_back(i);
                indices.push_back(j);
            }
        }
    }
    withPositions(positions);
    withIndices(indices);
    withMeshTopology(Mesh::Topology::Lines);
    return *this;
}

std::vector<glm::vec3> Mesh::MeshBuilder::computeNormals()
{
    if (m_attributesVec3.find("position") == m_attributesVec3.end())
    {
        LOG_WARN("Cannot find vertex attribute position (vec3) for recomputeNormals()");
        return {};
    }

    std::vector<glm::vec3> vertexPositions = m_attributesVec3["position"];
    std::vector<glm::vec3> normals(vertexPositions.size(), glm::vec3(0));
    // 根据三角形的夹角得出权重值。
    auto computeNormal = [&](int i1, int i2, int i3) {
        auto v1 = vertexPositions[i1];
        auto v2 = vertexPositions[i2];
        auto v3 = vertexPositions[i3];
        auto v1v2 = glm::normalize(v2 - v1);
        auto v1v3 = glm::normalize(v3 - v1);
        auto normal = glm::normalize(glm::cross(v1v2, v1v3));
        float weight1 = acos(glm::max(-1.0f, glm::min(1.0f, glm::dot(v1v2, v1v3))));
        auto v2v3Alias = glm::normalize(v3 - v2);
        float weight2 = glm::pi<float>() - acos(glm::max(-1.0f, glm::min(1.0f, glm::dot(v1v2, v2v3Alias))));
        normals[i1] += normal * weight1;
        normals[i2] += normal * weight2;
        normals[i3] += normal * (glm::pi<float>() - weight1 - weight2);
    };

    if (m_indices.empty())
    {
        if (m_topologies[0] != Mesh::Topology::Triangles)
        {
            LOG_WARN("Cannot only triangles supported for recomputeNormals()");
            return {};
        }
        for (int i = 0; i < vertexPositions.size(); i = i + 3)
        {
            computeNormal(i, i + 1, i + 2);
        }
    }
    else
    {
        for (int j = 0; j < m_indices.size(); j++)
        {
            if (m_topologies[j] != Mesh::Topology::Triangles)
            {
                LOG_WARN("Cannot only triangles supported for recomputeNormals()");
                return {};
            }
            auto& submeshIdx = m_indices[j];
            for (int i = 0; i < submeshIdx.size(); i = i + 3)
            {
                computeNormal(submeshIdx[i], submeshIdx[i + 1], submeshIdx[i + 2]);
            }
        }
    }

    for (auto& val : normals)
    {
        val = glm::normalize(val);
    }
    return normals;
}

std::vector<glm::vec4> Mesh::MeshBuilder::computeTangents(const std::vector<glm::vec3>& normals)
{
    if (m_attributesVec3.find("position") == m_attributesVec3.end())
    {
        LOG_WARN("Cannot find vertex attribute position (vec3) required for m_recomputeNormals()");
        return {};
    }
    if (m_attributesVec4.find("uv") == m_attributesVec4.end())
    {
        LOG_WARN("Cannot find vertex attribute uv (vec4) required for m_recomputeNormals()");
        return {};
    }

    std::vector<glm::vec3> vertexPositions = m_attributesVec3["position"];
    std::vector<glm::vec4> uvs = m_attributesVec4["uv"];

    std::vector<glm::vec3> tan1(vertexPositions.size(), glm::vec3(0.0f));
    std::vector<glm::vec3> tan2(vertexPositions.size(), glm::vec3(0.0f));
    auto computeTangent = [&](int i1, int i2, int i3) {
        auto v1 = vertexPositions[i1];
        auto v2 = vertexPositions[i2];
        auto v3 = vertexPositions[i3];

        auto w1 = glm::vec2(uvs[i1]);
        auto w2 = glm::vec2(uvs[i2]);
        auto w3 = glm::vec2(uvs[i3]);

        float x1 = v2.x - v1.x;
        float x2 = v3.x - v1.x;
        float y1 = v2.y - v1.y;
        float y2 = v3.y - v1.y;
        float z1 = v2.z - v1.z;
        float z2 = v3.z - v1.z;

        float s1 = w2.x - w1.x;
        float s2 = w3.x - w1.x;
        float t1 = w2.y - w1.y;
        float t2 = w3.y - w1.y;

        float r = 1.0F / (s1 * t2 - s2 * t1);
        glm::vec3 sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r,
                       (t2 * z1 - t1 * z2) * r);
        glm::vec3 tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r,
                       (s1 * z2 - s2 * z1) * r);

        tan1[i1] += sdir;
        tan1[i2] += sdir;
        tan1[i3] += sdir;

        tan2[i1] += tdir;
        tan2[i2] += tdir;
        tan2[i3] += tdir;
    };

    if (m_indices.empty())
    {
        if (m_topologies[0] != Mesh::Topology::Triangles)
        {
            LOG_WARN("Cannot only triangles supported for recomputeTangents()");
            return {};
        }
        for (int i = 0; i < vertexPositions.size(); i = i + 3)
        {
            computeTangent(i, i + 1, i + 2);
        }
    }
    else
    {
        for (int j = 0; j < m_indices.size(); j++)
        {
            if (m_topologies[j] != Mesh::Topology::Triangles)
            {
                LOG_WARN("Cannot only triangles supported for recomputeTangents()");
                return {};
            }
            auto& submeshIdx = m_indices[j];
            for (int i = 0; i < submeshIdx.size(); i = i + 3)
            {
                computeTangent(submeshIdx[i], submeshIdx[i + 1], submeshIdx[i + 2]);
            }
        }
    }

    std::vector<glm::vec4> tangent(vertexPositions.size());
    for (long a = 0; a < vertexPositions.size(); a++)
    {
        auto n = normals[a];
        auto t = tan1[a];

        tangent[a] = glm::vec4(
            // Gram-Schmidt orthogonalize
            glm::normalize(t - n * glm::dot(n, t)),
            // Calculate handedness
            (glm::dot(glm::cross(n, t), tan2[a]) < 0.0F) ? -1.0F : 1.0F);
    }
    return tangent;
}

Mesh::MeshBuilder& Mesh::MeshBuilder::withAttribute(std::string_view name, const std::vector<glm::i32vec4>& values)
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

GLenum Mesh::convertBufferUsage(BufferUsage usage)
{
    GLenum u;
    switch (usage)
    {
    case BufferUsage::StaticDraw:
        u = GL_STATIC_DRAW;
        break;
    case BufferUsage::DynamicDraw:
        u = GL_DYNAMIC_DRAW;
        break;
    case BufferUsage::StreamDraw:
        u = GL_STREAM_DRAW;
        break;
    }
    return u;
}

void Mesh::updateIndexBuffers()
{
    m_elementBufferOffsetCount.clear();
    if (m_indices.empty())
    {
        if (m_ebo != 0)
        {
            glDeleteBuffers(1, &m_ebo);
            m_ebo = 0;
        }
    }
    else
    {
        if (m_ebo == 0)
        {
            glGenBuffers(1, &m_ebo);
        }
        size_t totalCount = 0;
        for (auto& index : m_indices)
        {
            totalCount += index.size();
        }
        std::vector<uint16_t> concatenatedIndices;
        concatenatedIndices.reserve(totalCount);
        int offset = 0;
        for (auto& index : m_indices)
        {
            size_t dataSize = index.size() * sizeof(uint16_t);
            concatenatedIndices.insert(concatenatedIndices.end(), index.begin(), index.end());
            m_elementBufferOffsetCount.emplace_back(offset, index.size());
            offset += (int)dataSize;
        }
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, offset, concatenatedIndices.data(), GL_STATIC_DRAW);
        m_dataSize += offset;
    }
}

} // namespace re