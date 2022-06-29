//
// Created by william on 2022/5/26.
//

#include "basicProject.h"
#include "commonMacro.h"

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

namespace
{
constexpr const char* vertexShaderStr = R"(#version 330
        in vec3 position;
        in vec3 normal;
        in vec2 uv;
        out vec3 vNormal;

        uniform mat4 g_model;
        uniform mat3 g_normalMat;
        #pragma include "uniforms_incl.glsl"
        void main(void) {
            gl_Position = g_projection * g_view * g_model * vec4(position, 1.0);
            vNormal = normal;
        }
    )";
constexpr const char* fragmentShaderStr = R"(#version 330
        out vec4 fragColor;
        in vec3 vNormal;

        uniform samplerCube tex;

        void main(void)
        {
            fragColor = texture(tex, vNormal);
        }
    )";
} // namespace

class CubeMapExample : public BasicProject
{
public:
    using BasicProject::BasicProject;
    ~CubeMapExample() override = default;
    void initialize() override
    {
        glm::vec3 eye{ 0, 0, 3 };
        glm::vec3 at{ 0, 0, 0 };
        glm::vec3 up{ 0, 1, 0 };
        m_camera.setLookAt(eye, at, up);
        m_camera.setPerspectiveProjection(60.0f, 0.1f, 100.0f);
        auto shader = Shader::create()
                          .withSourceString(vertexShaderStr, Shader::ShaderType::Vertex)
                          .withSourceString(fragmentShaderStr, Shader::ShaderType::Fragment)
                          .build();
        m_material = shader->createMaterial();
        auto tex = Texture::create()
                       .withFileCubeMap(GET_CURRENT("test/resources/cube/cube-posx.png"), Texture::CubeMapSide::PositiveX)
                       .withFileCubeMap(GET_CURRENT("test/resources/cube/cube-negx.png"), Texture::CubeMapSide::NegativeX)
                       .withFileCubeMap(GET_CURRENT("test/resources/cube/cube-posy.png"), Texture::CubeMapSide::PositiveY)
                       .withFileCubeMap(GET_CURRENT("test/resources/cube/cube-negy.png"), Texture::CubeMapSide::NegativeY)
                       .withFileCubeMap(GET_CURRENT("test/resources/cube/cube-posz.png"), Texture::CubeMapSide::PositiveZ)
                       .withFileCubeMap(GET_CURRENT("test/resources/cube/cube-negz.png"), Texture::CubeMapSide::NegativeZ)
                       .build();
        m_material->setTexture(tex);
        m_mesh = Mesh::create().withSphere().build();
    }
    void render() override
    {
        auto renderPass = RenderPass::create().withCamera(m_camera).build();
        renderPass.draw(m_mesh, glm::eulerAngleY(glm::radians(30 * m_totalTime)), m_material);
        m_inspector.update();
        m_inspector.gui();
    }
    void setTitle() override
    {
        m_title = "CubeMapExample";
    }
};

void cubeMapText()
{
    CubeMapExample test;
    test.run();
}