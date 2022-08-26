//
// Created by cwb on 2022/6/20.
//

#include "engineTestSimple.h"
#include "guiCommonDefine.h"
#include "utils/utils.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <filesystem>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <vector>
#include <core/modelImporter.h>

class PbrMaterialSelectExample : public CommonInterface
{
public:
    static glm::vec3 rotatedPosition(float x, float y, float distance)
    {
        glm::vec3 pos{ 0, 0, distance };
        return glm::rotateY(glm::rotateX(pos, glm::radians(x)), glm::radians(y));
    }

    static std::shared_ptr<Texture> updateTexture(const char* filename, std::shared_ptr<Texture> fallback)
    {
        auto res = Texture::create().withFile(filename).build();
        if (res)
        {
            return res;
        }
        return fallback;
    }

    using CommonInterface::CommonInterface;
    ~PbrMaterialSelectExample() override = default;
    void initialize() override
    {
        updateLight();
        m_camera.setPerspectiveProjection(60, 0.1, 100);
        m_camera.setLookAt({ 0, 0, s_cameraDist }, { 0, 0, 0 }, { 0, 1, 0 });
        m_camera.setViewport({ 0.3333, 0 }, { 0.6666, 1 });
        auto files = getCurrentDirFiles("resources/pbr");
        for (const auto& file : files)
        {
            if(file == ".DS_Store") continue;
            m_imageList += file;
            m_imageList.insert(m_imageList.end(), '\0');
            std::string resDir = "resources/pbr/";
            m_colorTex.emplace_back(Texture::create().withFile(resDir + file + s_colorTexStr).build());
            m_metToughTex.emplace_back(Texture::create().withFile(resDir + file + s_metRoughTexStr).withSamplerColorspace(Texture::SamplerColorspace::Gamma).build());
            m_normalTex.emplace_back(Texture::create().withFile(resDir + file + s_normalTexStr).withSamplerColorspace(Texture::SamplerColorspace::Gamma).build());
            m_emissiveTex.emplace_back(Texture::create().withFile(resDir + file + s_emissiveTexStr).build());
            m_occlusionTex.emplace_back(Texture::create().withFile(resDir + file + s_occlusionTexStr).withSamplerColorspace(Texture::SamplerColorspace::Gamma).build());
        }
        updateMaterial();
        m_meshes = { { Mesh::create().withSphere(32, 64).build(),
                       Mesh::create().withCube().build(),
                       Mesh::create().withTorus(48, 48).build(),
                       ModelImporter::importObj("resources/objFiles/monkey/monkey.obj")} };
    }

    void update(float deltaTime) override
    {
        CommonInterface::update(deltaTime);
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
        m_title = "PbrMaterialSelectExample";
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
        m_material->setTexture(m_colorTex[s_pbrMaterial]);
        m_material->set("normalTex", m_normalTex[s_pbrMaterial]);
        m_material->set("mrTex", m_metToughTex[s_pbrMaterial]);
        m_material->set("normalScale", m_normalScale);
        m_material->set("emissiveTex", m_emissiveTex[s_pbrMaterial]);
        m_material->set("emissiveFactor", m_emissiveFactor);
        m_material->set("occlusionTex", m_occlusionTex[s_pbrMaterial]);
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
            updatedMat |= ImGui::DragFloat("Metallic", &m_metallicRoughness.x, 0.05f, 0, 1);
            updatedMat |= ImGui::DragFloat("Roughness", &m_metallicRoughness.y, 0.05f, 0, 2);
            if (m_specialization.find("S_OCCLUSIONMAP") != m_specialization.end())
            {
                updatedMat |= ImGui::DragFloat("Occlusion Strength", &m_occlusionStrength, 0.1f, 0.0f, 2.0f);
            }
            if (m_specialization.find("S_EMISSIVEMAP") != m_specialization.end())
            {
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
            ImGui::Combo("Mesh", &m_meshType, "Sphere\0Cube\0Torus\0monkey\0");
        }
        if (ImGui::CollapsingHeader("PBRMaterial"))
        {
            updatedMat |= ImGui::Combo("PBR", &s_pbrMaterial, m_imageList.c_str());
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

private:
    static constexpr float s_cameraDist = 3.5f;
    // 基础颜色(albedo)
    constexpr static const char* s_colorTexStr = "/albedo.png";
    // 金属度(metallic)
    constexpr static const char* s_metRoughTexStr = "/metallic.png";
    // 法线(normal)
    constexpr static const char* s_normalTexStr = "/normal.png";
    // 自发光
    constexpr static const char* s_emissiveTexStr = "/albedo.png";
    // 环境光遮蔽(ao)
    constexpr static const char* s_occlusionTexStr = "/ao.png";

private:
    std::vector<std::shared_ptr<Texture>> m_colorTex;
    std::vector<std::shared_ptr<Texture>> m_metToughTex;
    std::vector<std::shared_ptr<Texture>> m_normalTex;
    std::vector<std::shared_ptr<Texture>> m_emissiveTex;
    std::vector<std::shared_ptr<Texture>> m_occlusionTex;
    std::map<std::string, std::string> m_specialization;
    std::vector<std::shared_ptr<Mesh>> m_meshes;
    std::string m_imageList;
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
    int s_pbrMaterial = 0;
};

void pbrMaterialSelectTest()
{
    GLFWRenderer renderer{};
    EngineTestSimple test(renderer);
    auto sceneNodeEffect = std::make_shared<PbrMaterialSelectExample>(&renderer);
    auto effect = std::make_shared<EffectManager>();
    effect->insertEffect(sceneNodeEffect);
    test.setEffect(effect);
    test.run();
}