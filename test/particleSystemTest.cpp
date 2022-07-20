//
// Created by cwb on 2022/7/19.
//
#include "basicProject.h"
#include "core/particleEmitter.h"
#include "guiCommonDefine.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/random.hpp>
#include <glm/gtx/transform.hpp>

class ParticleSystemExample : public BasicProject
{
public:
    ~ParticleSystemExample() override = default;
    void initialize() override
    {
        m_texture = Texture::create().withFile("resources/particles/flash01.png").build();
        m_emitter = std::make_shared<ParticleEmitter>(500, m_texture);
        glm::vec3 gravity = glm::vec3(0, -9.8, 0); // 引力
        m_emitter->setAcceleration(gravity, gravity);
        updateSizeInterpolation();
        updateColorInterpolation();
        updateEmit();
        m_camera.setLookAt(m_eye, m_at, { 0, 1, 0 });
        m_camera.setPerspectiveProjection(m_fov, m_near, m_far);
        m_mesh = Mesh::create().withSphere().build();
        m_planeMesh = Mesh::create().withCube(10).build();
        m_worldLights = MAKE_UNIQUE(m_worldLights);
        m_worldLights->addLight(Light::create()
                                    .withDirectionalLight(glm::normalize(glm::vec3(1, 1, 1)))
                                    .build());
        // Add fake shadows
        m_worldLights->addLight(Light::create()
                                    .withPointLight(m_p1 - glm::vec3(0, 0.8, 0))
                                    .withColor({ -3.0f, -3.0f, -3.0f })
                                    .withRange(4)
                                    .build());
        m_worldLights->addLight(Light::create()
                                    .withPointLight(m_p1 - glm::vec3(0, 0.8, 0))
                                    .withColor({ -3.0f, -3.0f, -3.0f })
                                    .withRange(4)
                                    .build());

        m_mat1 = Shader::getStandardBlinnPhong()->createMaterial();
        m_mat1->setColor({ 0, 0, 1, 1 });
        m_mat1->setSpecularity({ 0, 0, 0, 0 });

        m_mat2 = Shader::getStandardBlinnPhong()->createMaterial();
        m_mat2->setColor({ 1, 0, 0, 1 });
        m_mat2->setSpecularity({ 0, 0, 0, 0 });
        m_matPlane = Shader::getStandardBlinnPhong()->createMaterial();
        m_matPlane->setColor({ 1, 1, 1, 1 });
        m_matPlane->setSpecularity({ 0, 0, 0, 0 });
    }
    void resize(int width, int height) override
    {
        BasicProject::resize(width, height);
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
            ImGui::DragFloat3("eye", &m_eye.x, 0.1, -10, 10);
            ImGui::DragFloat3("at", &m_at.x, 0.1, -10, 10);
            m_camera.setLookAt(m_eye, m_at, { 0, 1, 0 });
        }
    }

    void updateColorInterpolation()
    {
        m_emitter->setColor(m_colorFrom, m_colorFrom, m_colorTo, m_colorTo);
    }

    void updateSizeInterpolation()
    {
        m_emitter->setSize(m_sizeFrom, m_sizeFrom, m_sizeTo, m_sizeTo);
    }

    void updateEmit()
    {
        m_emitter->setPosition(m_emitPosition, m_emitPosition);
        m_emitter->setVelocity(glm::sphericalRand(m_emitVelocity), glm::sphericalRand(m_emitVelocity));
        m_emitter->setRotation(m_emitRotation, m_emitRotation);
        m_emitter->setAngularVelocity(m_emitAngularVelocity, m_emitAngularVelocity);
    }

    void particleSystemGUI()
    {
        if (ImGui::CollapsingHeader("Particle System"))
        {
            //            ImGui::DragFloat("EmissionRate", &m_emitter->emissionRate, 0.1f, 0, 200);
            //            ImGui::DragFloat("Life time", &m_emitter->lifeSpan, 0.1f, 0, 100);
            //            ImGui::DragFloat3("Gravity", &m_emitter->gravity.x, .1f, -50, 50);
            //            ImGui::Checkbox("running", &m_emitter->running);
            //            ImGui::Checkbox("visible", &m_emitter->visible);
            //            ImGui::Checkbox("emitting", &m_emitter->emitting);
            //            ImGui::Text("Active particles: %i", m_emitter->getActiveParticles());
            bool changedColorFrom = ImGui::ColorEdit4("Color from", &m_colorFrom.x);
            bool changedColorTo = ImGui::ColorEdit4("Color to", &m_colorTo.x);
            if (changedColorFrom || changedColorTo)
            {
                updateColorInterpolation();
            }
            bool changedSize = ImGui::DragFloat("Size from", &m_sizeFrom, 1, 0.1, 500);
            changedSize |= ImGui::DragFloat("Size to", &m_sizeTo, 1, 0.1, 500);
            if (changedSize)
            {
                updateSizeInterpolation();
            }
            //            bool changedTex = ImGui::Combo("Texture", &selectedTexture, textureNames.data(), textureNames.size());
            //            if (changedTex)
            //            {
            //                m_emitter->setTexture(textures[selectedTexture]);
            //            }
            bool changedEmit = ImGui::DragFloat3("Emit pos", &m_emitPosition.x);
            changedEmit |= ImGui::DragFloat("Emit velocity", &m_emitVelocity);
            changedEmit |= ImGui::DragFloat("Emit init rotation", &m_emitRotation);
            changedEmit |= ImGui::DragFloat("Emit angular velocity", &m_emitAngularVelocity);
            if (changedEmit)
            {
                updateEmit();
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

        //        rp.draw(m_mesh, m_pos1, m_mat1);
        //        rp.draw(m_mesh, m_pos2, m_mat2);
        //        rp.draw(m_planeMesh, glm::translate(glm::vec3{ 0, -1.0f, 0 }) * glm::scale(glm::vec3{ 1, .01f, 1 }), m_matPlane);

        m_emitter->draw(rp);
        //        ImGui::Begin("Settings");
        //        particleSystemGUI();
        //        cameraGUI();
        //        ImGui::End();
        //        m_inspector.update();
        //        m_inspector.gui();
    }
    void update(float deltaTime) override
    {
        m_emitter->update(deltaTime);
    }
    void setTitle() override
    {
        m_title = "ParticleSystemExample";
    }

private:
    std::shared_ptr<ParticleEmitter> m_emitter;
    std::shared_ptr<Texture> m_texture;
    std::shared_ptr<Mesh> m_planeMesh;
    std::shared_ptr<Material> m_mat1;
    std::shared_ptr<Material> m_mat2;
    std::shared_ptr<Material> m_matPlane;
    glm::vec4 m_colorFrom = { 1, 0, 0, 1 };
    glm::vec4 m_colorTo = { 1, 1, 1, 0 };
    glm::vec3 m_eye{ 0, 0, -10 };
    glm::vec3 m_at{ 0, 0, 0 };
    glm::vec3 m_p1 = { -1, 0, 0 };
    glm::vec3 m_p2 = { 1, 0, 0 };
    glm::mat4 m_pos1 = glm::translate(glm::mat4(1), m_p1);
    glm::mat4 m_pos2 = glm::translate(glm::mat4(1), m_p2);
    glm::vec3 m_emitPosition{ 0, 3, 0 };
    float m_sizeFrom = 50;
    float m_sizeTo = 30;
    float m_fov{ 60.0f };
    float m_near{ 0.1 };
    float m_far{ 50 };
    float m_orthoSize{ 2 };
    float m_emitVelocity{ 1 };
    float m_emitRotation{ 10 };
    float m_emitAngularVelocity{ 10 };
    bool m_perspective{ true };
};

void particleSystemTest()
{
    ParticleSystemExample test;
    test.run();
}