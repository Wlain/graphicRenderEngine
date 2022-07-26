//
// Created by cwb on 2022/7/25.
//

#ifndef GRAPHICRENDERENGINE_COMMONINTERFACE_H
#define GRAPHICRENDERENGINE_COMMONINTERFACE_H
#include "core/inspector.h"
#include "core/renderer.h"
#include "core/shader.h"
#include "core/worldLights.h"

using namespace ceres;

class CommonInterface
{
public:
    CommonInterface(GLFWRenderer* renderer);
    virtual ~CommonInterface();
    virtual void initialize();
    virtual void resize(int width, int height);
    virtual void update(float deltaTime);
    virtual void render();
    virtual void finalize();
    virtual void setTitle();
    virtual void cursorPosEvent(double xPos, double yPos);
    // button: left or right, action:pressed or released
    virtual void mouseButtonEvent(int button, int action, int mods);
    virtual void dropEvent(int count, const char** paths);
    void setEffect();

protected:
    GLFWRenderer* m_renderer{ nullptr };
    std::string m_title{ "EngineTestSimple" };
    Inspector m_inspector;
    Camera m_camera;
    std::shared_ptr<Material> m_material;
    std::shared_ptr<Mesh> m_mesh;
    std::unique_ptr<WorldLights> m_worldLights;
    glm::vec2 m_rotate{ 0.0f, 0.0f };
    glm::vec2 m_mousePos{ 0.0f, 0.0f };
    float m_deltaTime{ 0.0f };
    float m_totalTime{ 0.0f };
    bool m_enableProfiling{ true };
    bool m_debug{ false };
    bool m_debugKeyPressed{ false };
    bool m_leftMousePressed{ false };
};

#endif // GRAPHICRENDERENGINE_COMMONINTERFACE_H
