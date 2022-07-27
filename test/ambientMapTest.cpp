//
// Created by william on 2022/7/6.
//
#include "core/modelImporter.h"
#include "engineTestSimple.h"
#include "guiCommonDefine.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

static std::string s_vertexShaderSource = R"(#version 330
in vec4 position;
in vec3 normal;
out vec3 vNormal;

#include "uniforms_incl.glsl"

void main() {
    gl_Position = g_projection * g_view * g_model * position;
    vNormal = normalize(mat3(g_view) *g_model_it * normal);
}
)";

static std::string s_fragmentShaderSource = R"(#version 330
out vec4 fragColor;
in vec3 vNormal;

uniform sampler2D tex;

void main()
{
    vec3 normal = normalize(vNormal);
    fragColor = texture(tex,normal.xy * 0.5 + 0.5);
}
)";

class AmbientExample : public CommonInterface
{
public:
    using CommonInterface::CommonInterface;
    ~AmbientExample() override = default;
    void initialize() override
    {
        m_camera.setLookAt({ 0, 0, 3.5f }, { 0, 0, 0 }, { 0, 1, 0 });
        m_camera.setPerspectiveProjection(45.0f, 0.1f, 100.0f);
        m_mesh = ModelImporter::importObj("resources/objFiles/suzanne.obj");
        for (int i = 0; i < s_textureCount; i++)
        {
            m_textures[i] = Texture::create().withFile(std::string("resources/matcap/matcap_0000") + std::to_string(i + 1) + ".png").build();
        }
        m_material = Shader::create()
                         .withSourceString(s_vertexShaderSource, Shader::ShaderType::Vertex)
                         .withSourceString(s_fragmentShaderSource, Shader::ShaderType::Fragment)
                         .build()
                         ->createMaterial();
    }
    void render() override
    {
        auto rp = RenderPass::create()
                      .withCamera(m_camera)
                      .withClearColor(true, { 0, 0, 0, 1 })
                      .build();
        m_material->setTexture(m_textures[m_textureIndex]);
        rp.draw(m_mesh, glm::rotate(m_rotate.x, glm::vec3(1, 0, 0)) * glm::rotate(m_rotate.y, glm::vec3(0, 1, 0)), m_material);
        static const char* items[] = { "00001", "00002", "00003", "00004" };
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowContentSize(ImVec2(300, 100));
        ImGui::Begin("MatCap");
        ImGui::ListBox("Texture", &m_textureIndex, items, 4);
        ImGui::End();
    }
    void setTitle() override
    {
        m_title = "AmbientExample";
    }

private:
    inline static const int32_t s_textureCount{ 4 };

private:
    std::array<std::shared_ptr<Texture>, s_textureCount> m_textures;
    int m_textureIndex{ 0 };
};

// 环境光映射
void ambientMapTest()
{
    GLFWRenderer renderer{};
    EngineTestSimple test(renderer);
    auto sceneNodeEffect = std::make_shared<AmbientExample>(&renderer);
    auto effect = std::make_shared<EffectManager>();
    effect->insertEffect(sceneNodeEffect);
    test.setEffect(effect);
    test.run();
}