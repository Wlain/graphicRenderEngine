//
// Created by cwb on 2022/7/25.
//

#include "engineTestSimple.h"
#include "guiCommonDefine.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/rotate_vector.hpp>

class Node;

Node* cameraNode;

class Node
{
public:
    Node();

    Node(Node* parent);
    glm::mat4 localToWorld();
    void gui(int level = 0);

public:
    glm::vec3 m_position{ 0.0f, 0.0f, 0.0f };
    glm::vec3 m_rotationEuler{ 0.0f, 0.0f, 0.0f };
    glm::vec3 m_scale{ 1.0f, 1.0f, 1.0f };
    std::vector<Node> m_children;
    Node* m_parent{ nullptr };
};

Node::Node() = default;

Node::Node(Node* parent) :
    m_parent(parent)
{
}

glm::mat4 Node::localToWorld()
{
    auto rotationEulerRadian = glm::radians(m_rotationEuler);
    auto res = glm::translate(m_position) * glm::eulerAngleXYZ(rotationEulerRadian.x, rotationEulerRadian.y, rotationEulerRadian.z) * glm::scale(m_scale);
    if (m_parent)
    {
        res = m_parent->localToWorld() * res;
    }
    return res;
}

void Node::gui(int level)
{
    ImGui::PushID(this);
    ImGui::Indent(level * 20);
    if (ImGui::CollapsingHeader("Node"))
    {
        ImGui::DragFloat3("Local Position", &m_position.x);
        ImGui::DragFloat3("Local Rotation", &m_rotationEuler.x);
        ImGui::DragFloat3("Local Scale", &m_scale.x);
        auto globalPos = localToWorld() * glm::vec4(0, 0, 0, 1); // transform 0,0,0 (pivot point) from local coordinate frame to global coordinate frame
        bool changed = ImGui::DragFloat3("Global Position", &globalPos.x);
        if (changed)
        {
            glm::vec3 deltaPositionInLocalSpace = glm::inverse(localToWorld()) * globalPos;
            m_position = m_position + deltaPositionInLocalSpace;
        }
        if (ImGui::Button("Add child"))
        {
            m_children.emplace_back(this);
        }
        if (ImGui::Button("Make camera"))
        {
            cameraNode = this;
        }
        for (auto& n : m_children)
        {
            n.gui(level + 1);
        }
    }
    ImGui::Unindent(level * 20);
    ImGui::PopID();
}

class SceneNodeEffect : public CommonInterface
{
public:
    using CommonInterface::CommonInterface;
    ~SceneNodeEffect() override = default;
    void initialize() override
    {
        m_camera.setLookAt({ 0, 0, 30 }, { 0, 0, 0 }, { 0, 1, 0 });
        m_camera.setPerspectiveProjection(60, 0.1, 100);

        m_material = Shader::getStandardBlinnPhong()->createMaterial();
        m_material->setColor({ 1.0f, 1.0f, 1.0f, 1.0f });
        m_material->setSpecularity({ 1, 1, 1, 20.0f });
        m_mesh = Mesh::create().withCube().build();
        m_worldLights = MAKE_UNIQUE(m_worldLights);
        m_worldLights->setAmbientLight({ 0.5, 0.5, 0.5 });
        m_worldLights->addLight(Light::create().withPointLight({ 0, 3, 0 }).withColor({ 1, 0, 0 }).withRange(20).build());
        m_worldLights->addLight(Light::create().withPointLight({ 3, 0, 0 }).withColor({ 0, 1, 0 }).withRange(20).build());
        m_worldLights->addLight(Light::create().withPointLight({ 0, -3, 0 }).withColor({ 0, 0, 1 }).withRange(20).build());
        m_worldLights->addLight(Light::create().withPointLight({ -3, 0, 0 }).withColor({ 1, 1, 1 }).withRange(20).build());
        m_root.m_children.emplace_back(nullptr);
    }

    void renderNode(Node& node, RenderPass& rp)
    {
        rp.draw(m_mesh, node.localToWorld(), m_material);
        // draw children
        for (auto& n : node.m_children)
        {
            renderNode(n, rp);
        }
    }

    void render() override
    {
        auto renderPass = RenderPass::create()
                              .withCamera(m_camera)
                              .withWorldLights(m_worldLights.get())
                              .withClearColor(true, { 1, 0, 0, 1 })
                              .build();

        for (auto& n : m_root.m_children)
        {
            renderNode(n, renderPass);
        }

        // draw gui
        ImGui::Begin("SceneGraph");
        if (ImGui::Button("Add object"))
        {
            m_root.m_children.emplace_back(nullptr);
        }
        for (auto& n : m_root.m_children)
        {
            n.gui();
        }
        ImGui::End();
    }
    void setTitle() override
    {
        m_title = "SceneNodeEffect";
    }

private:
    Node m_root;
};

void sceneTest()
{
    GLFWRenderer renderer{};
    EngineTestSimple test(renderer);
    auto sceneNodeEffect = std::make_shared<SceneNodeEffect>(&renderer);
    auto effect = std::make_shared<EffectManager>();
    effect->insertEffect(sceneNodeEffect);
    test.setEffect(effect);
    test.run();
}
