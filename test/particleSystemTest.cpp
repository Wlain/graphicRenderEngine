//
// Created by cwb on 2022/7/19.
//
#include "engineTestSimple.h"
#include "core/particleEmitter.h"
#include "guiCommonDefine.h"
#include "utils/utils.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/random.hpp>

class ParticleSystemExample : public CommonInterface
{
public:
    using CommonInterface::CommonInterface;
    ~ParticleSystemExample() override = default;
    void initialize() override
    {
        m_camera.setLookAt(m_eye, m_at, { 0, 1, 0 });
        m_camera.setPerspectiveProjection(m_fov, m_near, m_far);
        std::string resDir = "resources/particles";
        auto files = getCurrentDirFiles(resDir);

        for (const auto& file : files)
        {
            if (file == ".DS_Store") continue;
            m_textureNames += file;
            m_textureNames.insert(m_textureNames.end(), '\0');
            m_textures.push_back(Texture::create().withFile(resDir + '/' + file).build());
            LOG_ERROR("cwb:{}", file.c_str());
        }
        m_emitPosition = glm::linearRand(glm::vec3(-m_size, -m_size, -m_size), glm::vec3(m_size, m_size, m_size));
        m_material = Shader::getStandardParticles()->createMaterial();
        m_material->setTexture(m_textures[m_selectedTexture]);
        m_emitter = ParticleEmitter::create()
                        .withParticleCount(m_particleCount)
                        .withMaterial(m_material)
                        .withPosition(m_emitPosition, m_emitPosition)
                        .withAcceleration(m_acceleration, m_acceleration)
                        .withVelocity(m_emitVelocity, m_emitVelocity)
                        .withRotation(m_emitRotation, m_emitRotation)
                        .withAngularVelocity(m_emitAngularVelocity, m_emitAngularVelocity)
                        .withColor(m_colorFrom, m_colorFrom, m_colorTo, m_colorTo)
                        .withSize(m_sizeFrom, m_sizeFrom, m_sizeTo, m_sizeTo)
                        .withEmissionRate(m_emissionRate)
                        .withVisible(m_visible)
                        .withLifeSpan(m_lifeSpan)
                        .withRunning(m_started)
                        .withEmitting(m_emitting)
                        .build();
    }
    void resize(int width, int height) override
    {
        CommonInterface::resize(width, height);
    }

    void cameraGUI()
    {
        if (ImGui::CollapsingHeader("Camera"))
        {
            ImGui::Checkbox("Perspective projection", &m_perspective);
            if (m_perspective)
            {
                ImGui::DragFloat("FOV", &m_fov, 1, 1, 179);
            }
            else
            {
                ImGui::DragFloat("OrthoSize", &m_orthoSize, 0.1, 0.1, 10);
            }
            ImGui::DragFloat("Near", &m_near, 0.1, -10, 10);
            ImGui::DragFloat("Far", &m_far, 0.1, 0.1, 100);
            if (m_perspective)
            {
                m_camera.setPerspectiveProjection(m_fov, m_near, m_far);
            }
            else
            {
                m_camera.setOrthographicProjection(m_orthoSize, m_near, m_far);
            }
            ImGui::DragFloat3("eye", &m_eye.x);
            ImGui::DragFloat3("at", &m_at.x);
            m_camera.setLookAt(m_eye, m_at, { 0, 1, 0 });
        }
    }

    void particleSystemGUI()
    {
        if (ImGui::CollapsingHeader("Particle System"))
        {
            bool update{};
            update |= ImGui::DragInt("EmissionRate", &m_emissionRate);
            update |= ImGui::DragFloat("Life time", &m_lifeSpan);
            update |= ImGui::DragFloat3("Acceleration", &m_acceleration[0]);
            update |= ImGui::Checkbox("running", &m_started);
            update |= ImGui::Checkbox("visible", &m_visible);
            update |= ImGui::Checkbox("emitting", &m_emitting);
            ImGui::Text("Active particles: %i", m_emitter->activeParticles());
            update |= ImGui::ColorEdit4("Color from", &m_colorFrom.x);
            update |= ImGui::ColorEdit4("Color to", &m_colorTo.x);
            update |= ImGui::DragFloat("Size to", &m_sizeFrom, 1, 0.1, 500);
            update |= ImGui::DragFloat("Size to", &m_sizeTo, 1, 0.1, 500);
            bool changedTex = ImGui::Combo("Texture", &m_selectedTexture, m_textureNames.c_str());
            if (changedTex)
            {
                m_material->setTexture(m_textures[m_selectedTexture]);
            }
            update |= ImGui::DragFloat3("Emit pos", &m_emitPosition.x);
            update |= ImGui::DragFloat3("Emit velocity", &m_emitVelocity.x);
            update |= ImGui::DragFloat("Emit init rotation", &m_emitRotation);
            update |= ImGui::DragFloat("Emit angular velocity", &m_emitAngularVelocity);
            if (update)
            {
                m_emitter->ParticleEmitter::update()
                    .withParticleCount(m_particleCount)
                    .withMaterial(m_material)
                    .withPosition(m_emitPosition, m_emitPosition)
                    .withAcceleration(m_acceleration, m_acceleration)
                    .withVelocity(m_emitVelocity, m_emitVelocity)
                    .withRotation(m_emitRotation, m_emitRotation)
                    .withAngularVelocity(m_emitAngularVelocity, m_emitAngularVelocity)
                    .withColor(m_colorFrom, m_colorFrom, m_colorTo, m_colorTo)
                    .withSize(m_sizeFrom, m_sizeFrom, m_sizeTo, m_sizeTo)
                    .withEmissionRate(m_emissionRate)
                    .withVisible(m_visible)
                    .withLifeSpan(m_lifeSpan)
                    .withRunning(m_started)
                    .withEmitting(m_emitting);
            }
        }
    }

    void render() override
    {
        auto rp = RenderPass::create()
                      .withCamera(m_camera)
                      .withWorldLights(m_worldLights.get())
                      .withClearColor(true, { 0, 0, 0, 1 })
                      .build();

        m_emitter->draw(rp);
        ImGui::Begin("Settings");
        particleSystemGUI();
        cameraGUI();
        ImGui::End();
        m_inspector.update();
        m_inspector.gui();
    }
    void update(float deltaTime) override
    {
        m_emitter->update().withPosition(glm::sphericalRand(10.0f), glm::sphericalRand(10.0f));
        m_emitter->update(deltaTime);
    }

private:
    std::shared_ptr<ParticleEmitter> m_emitter;
    std::vector<std::shared_ptr<Texture>> m_textures;
    glm::vec4 m_colorFrom = { 1.0f, 0.0f, 0.0f, 1.0f };
    glm::vec4 m_colorTo = { 1.0f, 1.0f, 1.0f, 0.0f };
    glm::vec3 m_eye{ 0.0f, 0.0f, -10.0f };
    glm::vec3 m_at{ 0.0f, 0.0f, 0.0f };
    glm::vec3 m_acceleration{ 0.0f, -9.8f, 0.0f }; // 加速度
    glm::vec3 m_emitVelocity{ 0.0f };
    glm::vec3 m_emitPosition{ 0.0f, 0.0f, 0.0f };
    std::string m_textureNames;
    int m_selectedTexture{ 3 };
    int m_emissionRate{ 60 };
    int m_particleCount{ 500 };
    float m_lifeSpan{ 10.0f };
    float m_sizeFrom{ 50.0f };
    float m_sizeTo{ 40.0f };
    float m_fov{ 60.0f };
    float m_near{ 0.1f };
    float m_far{ 50.0f };
    float m_orthoSize{ 2.0f };
    float m_emitRotation{ 10.0f };
    float m_emitAngularVelocity{ 10.0f };
    bool m_perspective{ true };
    bool m_started{ true };  // 是否还在运行
    bool m_visible{ true };  // 是否可见
    bool m_emitting{ true }; // 发射状态
    float m_size = 10.0f;
};

void particleSystemTest()
{
    GLFWRenderer renderer{};
    EngineTestSimple test(renderer);
    auto sceneNodeEffect = std::make_shared<ParticleSystemExample>(&renderer);
    auto effect = std::make_shared<EffectManager>();
    effect->insertEffect(sceneNodeEffect);
    test.setEffect(effect);
    test.setTitle("ParticleSystemExample");
    test.run();
}