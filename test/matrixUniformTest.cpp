//
// Created by william on 2022/6/24.
//
#include "basicProject.h"
#include "guiCommonDefine.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/transform.hpp>
class MatrixUniformExample : public BasicProject
{
public:
    using BasicProject::BasicProject;
    ~MatrixUniformExample() override = default;
    void initialize() override
    {
        m_camera.setLookAt({ 0.0f, 0.0f, 3.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });
        m_camera.setPerspectiveProjection(60.0f, 0.1f, 100.0f);
        std::string vertexShaderSource = R"(#version 330
            in vec3 position;
            in vec2 uv;
            out vec2 vUV;
            uniform mat4 g_model;
            uniform int index;
            uniform mat4 customTransform[2];
            #pragma include "uniforms_incl.glsl"
            void main(void) {
                vUV = uv;
                gl_Position = g_projection * g_view * g_model *  customTransform[index] * vec4(position, 1.0);
            }
        )";
        std::string fragmentShaderSource = R"(#version 330
            in vec2 vUV;
            out vec4 fragColor;
            uniform sampler2D tex;
            void main(void)
            {
                fragColor = texture(tex, vUV);
            }
        )";
        m_material = Shader::create()
                         .withSourceString(vertexShaderSource, Shader::ShaderType::Vertex)
                         .withSourceString(fragmentShaderSource, Shader::ShaderType::Fragment)
                         .build()
                         ->createMaterial();
        m_material->setTexture(Texture::create().withFile("resources/test.jpg").build());
        std::vector<glm::vec3> vertices({ { 1, -1, 0 },
                                          { 1, 1, 0 },
                                          { -1, -1, 0 },
                                          { -1, 1, 0 } });
        std::vector<glm::vec2> uvs({ { 1, 0 },
                                     { 1, 1 },
                                     { 0, 0 },
                                     { 0, 1 } });
        std::vector<uint16_t> indices = {
            0, 1, 2,
            2, 1, 3
        };
        m_mesh = Mesh::create().withAttribute("position", vertices).withAttribute("uv", uvs).withIndices(indices).build();
        m_mat4s = MAKE_SHARED(m_mat4s);
        m_mat4s->resize(2);
        for (auto& mat : *m_mat4s)
        {
            mat = glm::mat4(1);
        }
    }
    void render() override
    {
        auto renderPass = RenderPass::create().withCamera(m_camera).build();
        ImGui::DragInt("Id", &m_id, 1, 0, 1);
        ImGui::DragFloat3("Offset ", &m_offset[m_id].x, 0.1f);
        ImGui::DragFloat("Rotate ", &m_rotate[m_id], 0.1f);
        for (int i = 0; i < 2; i++)
        {
            (*m_mat4s)[i] = glm::translate(m_offset[i]) * glm::rotate(m_rotate[i], glm::vec3(0, 0, 1));
        }
        m_material->set("customTransform", m_mat4s);
        m_material->set("index", m_id);
        renderPass.draw(m_mesh, glm::mat4(1.0), m_material);
    }
    void setTitle() override
    {
        m_title = "MatrixUniformExample";
    }

private:
    std::shared_ptr<std::vector<glm::mat4>> m_mat4s;
    int m_id = 0;
    glm::vec3 m_offset[2] = { { 0, 0, 0 }, { 0, 0, 0 } };
    float m_rotate[2] = { 0, 0 };
};

void matrixUniformTest()
{
    MatrixUniformExample test;
    test.run();
}
