//
// Created by cwb on 2022/6/20.
//

#include "basicProject.h"
#include "guiCommonDefine.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/rotate_vector.hpp>
using namespace re;

class PbrExample : public BasicProject
{
public:
    glm::vec3 rotatedPosition(float x, float y, float distance)
    {
        glm::vec3 pos{ 0, 0, distance };
        return glm::rotateY(glm::rotateX(pos, glm::radians(x)), glm::radians(y));
    }

    std::shared_ptr<Texture> updateTexture(char* filename, std::shared_ptr<Texture> fallback)
    {
        auto res = Texture::create().withFile(GET_CURRENT(filename)).build();
        if (res)
        {
            return res;
        }
        return fallback;
    }

    using BasicProject::BasicProject;
    ~PbrExample() override = default;
    void initialize() override
    {
        updateLight();
        m_camera.setPerspectiveProjection(60, 0.1, 100);
        m_camera.setLookAt({ 0, 0, s_cameraDist }, { 0, 0, 0 }, { 0, 1, 0 });
        m_camera.setViewport({ 0.3333, 0 }, { 0.6666, 1 });
        m_colorTex = Texture::create().withWhiteData(2, 2).build();
        m_metToughTex = Texture::create().withWhiteData(2, 2).build();
        m_normalTex = Texture::create().withWhiteData(2, 2).build();
        m_emissiveTex = Texture::create().withWhiteData(2, 2).build();
        m_occlusionTex = Texture::create().withWhiteData(2, 2).build();
        m_colorTex = Texture::create().withFile(GET_CURRENT(s_colorTexStr)).build();
        m_metToughTex = Texture::create().withFile(GET_CURRENT(s_metRoughTexStr)).withSamplerColorspace(Texture::SamplerColorspace::Gamma).build();
        m_normalTex = Texture::create()
                          .withFile(GET_CURRENT(s_normalTexStr))
                          .withSamplerColorspace(Texture::SamplerColorspace::Gamma)
                          .build();
        m_emissiveTex = Texture::create().withFile(GET_CURRENT(s_emissiveTexStr)).build();
        m_occlusionTex = Texture::create().withFile(GET_CURRENT(s_occlusionTexStr)).withSamplerColorspace(Texture::SamplerColorspace::Gamma).build();

        updateMaterial();

        m_meshes = { { Mesh::create().withSphere(32, 64).build(),
                       Mesh::create().withCube().build(),
                       Mesh::create().withTorus(48, 48).build() } };
    }

    void update(float deltaTime) override
    {
        BasicProject::update(deltaTime);
    }

    void render() override
    {
        // updateLight
        static Camera clearScreen;
        auto renderPassClear = RenderPass::create()
                                   .withCamera(clearScreen)
                                   .withGUI(false)
                                   .withClearColor(true, { 0, 0, 0, 1 })
                                   .build();
        renderPassClear.finish();

        auto renderPass = RenderPass::create()
                              .withCamera(m_camera)
                              .withWorldLights(m_lightCount == 0 ? (&m_lightsSingle) : (&m_lightsDuo))
                              .withClearColor(true, { .2f, .2f, .2f, 1 })
                              .build();
        renderPass.draw(m_meshes[m_meshType], glm::eulerAngleY(glm::radians(30.0f * m_totalTime)), m_material);

        renderGUI();
        renderPass.finish();
    }
    void setTitle() override
    {
        m_title = "PbrExample";
    }

    void updateLight()
    {
        m_lightsSingle.clear();
        m_lightsDuo.clear();
        m_lightsSingle.setAmbientLight({ m_ambientLight, m_ambientLight, m_ambientLight });
        m_lightsDuo.setAmbientLight({ m_ambientLight, m_ambientLight, m_ambientLight });
        bool directional = m_lightType == 0;
        if (directional)
        {
            m_lightsSingle.addLight(Light::create().withDirectionalLight(rotatedPosition(-10, -10, 1)).withColor({ 1, 1, 1 }).build());
            m_lightsDuo.addLight(Light::create().withDirectionalLight(rotatedPosition(-30, -30, 1)).withColor({ 1, 1, 1 }).build());
            m_lightsDuo.addLight(Light::create().withDirectionalLight(rotatedPosition(30, -30 + 180, 1)).withColor({ 1, 1, 1 }).build());
        }
        else
        {
            m_lightsSingle.addLight(Light::create().withPointLight(rotatedPosition(-10, -10, m_lightDistance)).withColor({ 1, 1, 1 }).build());
            m_lightsDuo.addLight(Light::create().withPointLight(rotatedPosition(-30, -30, m_lightDistance)).withColor({ 1, 1, 1 }).build());
            m_lightsDuo.addLight(Light::create().withPointLight(rotatedPosition(30, -30 + 180, m_lightDistance)).withColor({ 1, 1, 1 }).build());
        }
    }

    void updateMaterial()
    {
        m_material = Shader::getStandardPBR()->createMaterial(m_specialization);
        m_material->setColor(m_color);
        m_material->setMetallicRoughness(m_metallicRoughness);
        m_material->setTexture(m_colorTex);
        m_material->set("normalTex", m_normalTex);
        m_material->set("mrTex", m_metToughTex);
        m_material->set("normalScale", m_normalScale);
        m_material->set("emissiveTex", m_emissiveTex);
        m_material->set("emissiveFactor", m_emissiveFactor);
        m_material->set("occlusionTex", m_occlusionTex);
        m_material->set("occlusionStrength", m_occlusionStrength);
        m_material->setSpecularity(m_specularity);
    }

    void renderGUI()
    {
        ImGui::SetNextWindowPos(ImVec2{ 0, 0 });               // set next window position. call before Begin(). use pivot=(0.5f,0.5f) to center on given point, etc.
        ImGui::SetNextWindowSize(ImVec2{ 800 * .3333f, 600 }); // set next window size. set axis to 0.0f to force an auto-fit on this axis. call before Begin()
        ImGui::Begin("PBR");
        bool updatedMat = false;
        if (ImGui::CollapsingHeader("Material"))
        {
            auto col4 = m_color.toLinear();
            updatedMat |= ImGui::ColorEdit4("Color", &col4.x);
            m_color.setFromLinear(col4);
            if (m_specialization.find("S_NO_BASECOLORMAP") == m_specialization.end())
            {
                updatedMat |= loadTexture("ColorTex", m_colorTex, s_colorTexStr);
            }
            updatedMat |= ImGui::DragFloat("Metallic", &m_metallicRoughness.x, 0.05f, 0, 1);
            updatedMat |= ImGui::DragFloat("Roughness", &m_metallicRoughness.y, 0.05f, 0, 1);
            if (m_specialization.find("S_METALROUGHNESSMAP") != m_specialization.end())
            {
                updatedMat |= loadTexture("MetallicRoughnessTex", m_metToughTex, s_metRoughTexStr);
            }
            if (m_specialization.find("S_NORMALMAP") != m_specialization.end())
            {
                updatedMat |= loadTexture("NormalTex", m_normalTex, s_normalTexStr);
                updatedMat |= ImGui::DragFloat("NormalScale", &m_normalScale, 0.1f, 0.0f, 2.0f);
            }
            if (m_specialization.find("S_OCCLUSIONMAP") != m_specialization.end())
            {
                updatedMat |= loadTexture("Occlusion Tex", m_occlusionTex, s_occlusionTexStr);
                updatedMat |= ImGui::DragFloat("Occlusion Strength", &m_occlusionStrength, 0.1f, 0.0f, 2.0f);
            }
            if (m_specialization.find("S_EMISSIVEMAP") != m_specialization.end())
            {
                updatedMat |= loadTexture("Emissive Tex", m_emissiveTex, s_emissiveTexStr);
                updatedMat |= ImGui::DragFloat4("Emissive Factor", &m_emissiveFactor.x, 0.1f, 0.0f, 2.0f);
            }
        }
        if (ImGui::CollapsingHeader("Light"))
        {
            ImGui::Combo("Light count", &m_lightCount, "One light\0Two lights\0");
            bool updatedLight = false;
            updatedLight |= ImGui::Combo("Light type", &m_lightType, "Directional\0Point\0");
            if (m_lightType == 1)
            {
                updatedLight |= ImGui::DragFloat("LightDistance", &m_lightDistance, 0.5f, 0.0f, 50.0f);
            }
            updatedLight |= ImGui::DragFloat("Ambient light", &m_ambientLight, 0.02f, 0.0f, 1.0f);
            if (updatedLight)
            {
                updateLight();
            }
        }
        if (ImGui::CollapsingHeader("Model"))
        {
            ImGui::Combo("Mesh", &m_meshType, "Sphere\0Cube\0Torus\0");
        }
        if (ImGui::CollapsingHeader("Shader"))
        {
            auto shaderConstants = Shader::getStandardPBR()->getAllSpecializationConstants();
            for (auto& s : shaderConstants)
            {
                bool checked = m_specialization.find(s) != m_specialization.end();
                if (ImGui::Checkbox(s.c_str(), &checked))
                {
                    if (checked)
                    {
                        m_specialization.insert({ s, "1" });
                    }
                    else
                    {
                        m_specialization.erase(m_specialization.find(s));
                    }
                    updatedMat = true;
                }
            }
        }
        if (updatedMat)
        {
            updateMaterial();
        }
        ImGui::End();

        static Inspector inspector;
        inspector.update();
        inspector.gui();
    }

    bool loadTexture(std::string label, std::shared_ptr<Texture>& texRef, std::string_view fileLocation)
    {
        bool changed = ImGui::InputText(label.c_str(), (char*)fileLocation.data(), fileLocation.size());
        if (changed)
        {
            auto res = Texture::create().withFile(GET_CURRENT(fileLocation.data())).build();
            if (res)
            {
                texRef = res;
                return true;
            }
        }
        return false;
    }

private:
    static constexpr float s_cameraDist = 3.5f;
    static constexpr const auto s_colorTexStr = "test/resources/BoomBox_baseColor.png";
    static constexpr const auto s_metRoughTexStr = "test/resources/BoomBox_roughnessMetallic.png";
    static constexpr const auto s_normalTexStr = "test/resources/BoomBox_normal.png";
    static constexpr const auto s_emissiveTexStr = "test/resources/BoomBox_emissive.png";
    static constexpr const auto s_occlusionTexStr = "test/resources/BoomBox_occlusion.png";

private:
    std::shared_ptr<Texture> m_colorTex;
    std::shared_ptr<Texture> m_metToughTex;
    std::shared_ptr<Texture> m_normalTex;
    std::shared_ptr<Texture> m_emissiveTex;
    std::shared_ptr<Texture> m_occlusionTex;
    std::map<std::string, std::string> m_specialization;
    std::vector<std::shared_ptr<Mesh>> m_meshes;
    WorldLights m_lightsSingle;
    WorldLights m_lightsDuo;
    // 金属度（metallic）：0：代表非金属，1：代表金属；粗糙度（Roughness）：值越大越粗糙
    glm::vec2 m_metallicRoughness = glm::vec2(0.0, 0.5);
    glm::vec4 m_emissiveFactor = glm::vec4(1, 1, 1, 1);
    Color m_color = { 1, 1, 1, 1 };
    Color m_specularity = Color(1, 1, 1, 50);
    float m_occlusionStrength = 1; // 环境光遮蔽
    float m_normalScale = 1;
    float m_lightDistance = 10;
    float m_ambientLight = .1f;
    int m_lightType = 0;
    int m_meshType = 0;
    int m_lightCount = 0;
};

void pbrTest()
{
    PbrExample test;
    test.run();
}