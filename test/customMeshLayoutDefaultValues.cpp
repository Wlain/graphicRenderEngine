//
// Created by william on 2022/6/12.
//
#include "basicProject.h"

class CustomMeshLayoutDefaultValuesExample : public BasicProject
{
public:
    using BasicProject::BasicProject;
    ~CustomMeshLayoutDefaultValuesExample() override = default;
    void initialize() override
    {
        std::vector<glm::vec2> positions({ { 0, 0.5 },
                                           { -0.5, 0 },
                                           { 0.5, 0 } });
        std::vector<glm::vec4> colors({
            { 1, 0, 0, 1 },
            { 0, 1, 0, 1 },
            { 0, 0, 1, 1 },
        });
        m_mesh = Mesh::create()
                     .withAttribute("posxyzw", positions)
                     .withAttribute("color", colors)
                     .build();
        std::string vertexShaderSource = R"(#version 330
            in vec4 posxyzw;    // should automatically cast vec2 -> vec4 by appending (z = 0.0, w = 1.0)
            in vec4 color;
            out vec4 vColor;

            uniform mat4 g_model;
            uniform mat4 g_view;
            uniform mat4 g_projection;

            void main(void) {
                gl_Position = g_projection * g_view * g_model * posxyzw;
                vColor = color;
            }
        )";
        std::string fragmentShaderSource = R"(#version 330
            out vec4 fragColor;
            in vec4 vColor;

            void main(void)
            {
                fragColor = vColor;
            }
        )";
        m_material = Shader::create()
                         .withSourceString(vertexShaderSource, Shader::ShaderType::Vertex)
                         .withSourceString(fragmentShaderSource, Shader::ShaderType::Fragment)
                         .build()
                         ->createMaterial();
    }
    void render() override
    {
        auto rp = RenderPass::create()
                      .withCamera(m_camera)
                      .withClearColor(true, { 1, 0, 0, 1 })
                      .build();

        rp.draw(m_mesh, glm::mat4(1), m_material);
    }
    void setTitle() override
    {
        m_title = "CustomMeshLayoutDefaultValuesExample";
    }
};

void customMeshLayoutDefaultValuesTest()
{
    CustomMeshLayoutDefaultValuesExample test;
    test.run();
}