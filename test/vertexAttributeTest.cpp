//
// Created by william on 2022/6/11.
//

#include "basicProject.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

class VertexAttributeExample : public BasicProject
{
public:
    using BasicProject::BasicProject;
    ~VertexAttributeExample() override = default;
    void initialize() override
    {
        m_material = Shader::create()
                         .withSourceFile("embeddedResource/texture_vert.glsl", Shader::ShaderType::Vertex)
                         .withSourceFile("embeddedResource/texture_frag.glsl", Shader::ShaderType::Fragment)
                         .withName("Unlit")
                         .withCullFace(Shader::CullFace::None)
                         .build()
                         ->createMaterial();

        std::vector<glm::vec3> pos{
            { 0, 0, 0 },
            { 1, 0, 0 },
            { 0, 1, 0 }
        };

        m_mesh = Mesh::create().withPositions(pos).withMeshTopology(Mesh::Topology::Triangles).build();
    }
    void render() override
    {
        auto renderPass = RenderPass::create()
                              .withCamera(m_camera)
                              .withWorldLights(m_worldLights.get())
                              .withClearColor(true, { 1, 0, 0, 1 })
                              .build();
        renderPass.draw(m_mesh, glm::eulerAngleY(glm::radians(30.0f * m_totalTime)), m_material);

        std::vector<glm::vec3> verts = {
            { 0, 0, 0 },
            { 1, 0, 0 },
            { 1, 0, 0 },
            { 0, -1, 0 },
            { 0, -1, 0 },
            { 0, 0, 0 },
        };

        // Keep a shared mesh and material
        static auto meshTopology = Mesh::Topology::Lines;
        static auto material = Shader::getUnlit()->createMaterial();
        std::shared_ptr<Mesh> mesh = Mesh::create()
                                         .withPositions(verts)
                                         .withMeshTopology(meshTopology)
                                         .build();

        // update shared mesh
        mesh->update().withPositions(verts).build();

        // update material
        material->setColor({ 0, 1, 0, 1 });
        renderPass.draw(mesh, glm::mat4(1), material);
        constexpr float offset = -0.2f;
        for (auto& v : verts)
        {
            v.y += offset;
        }
        // update shared mesh
        mesh->update().withPositions(verts).build();

        // update material
        material->setColor({ 0, 1, 0, 1 });
        renderPass.draw(mesh, glm::mat4(1), material);
    }
    void setTitle() override
    {
        m_title = "VertexAttributeExample";
    }
};

void vertexAttributeTest()
{
    VertexAttributeExample test;
    test.run();
}