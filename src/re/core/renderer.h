//
// Created by william on 2022/5/22.
//

#ifndef SIMPLERENDERENGINE_RENDERER_H
#define SIMPLERENDERENGINE_RENDERER_H
#include "camera.h"
#include "light.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
namespace re
{
// 前向声明
class Mesh;
class Shader;

class Renderer
{
public:
    Renderer(GLFWwindow* window);
    ~Renderer();
    inline static Renderer* instance() { return s_instance; }
    void setLight(int lightIndex, Light light);
    Light& getLight(int lightIndex);
    void render(Mesh* mesh, const glm::mat4& modelTransform, Shader* shader);
    void setCamera(Camera* camera);
    inline Camera* getCamera() const { return m_camera; }
    inline Camera* getDefaultCamera() { return &m_defaultCamera; }
    void clearScreen(const glm::vec4& color, bool clearColorBuffer = true, bool clearDepthBuffer = true);
    // Update window with OpenGL rendering
    void swapWindow();

public:
    static constexpr int s_maxSceneLights = 4;
    static constexpr int s_rgVersionMajor = 0;
    static constexpr int s_rgVersionMinor = 1;
    inline static Renderer* s_instance = nullptr;

private:
    Light m_sceneLights[s_maxSceneLights];
    Camera m_defaultCamera;
    Camera* m_camera = nullptr;
    GLFWwindow* m_window = nullptr;
};
} // namespace re
#endif //SIMPLERENDERENGINE_RENDERER_H
