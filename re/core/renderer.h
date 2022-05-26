//
// Created by william on 2022/5/22.
//

#ifndef SIMPLERENDERENGINE_RENDERER_H
#define SIMPLERENDERENGINE_RENDERER_H
#include "camera.h"
#include "light.h"
#include "mesh.h"

#include <GLFW/glfw3.h>
#include <algorithm>
#include <glm/glm.hpp>
namespace re
{
// 前向声明
class Mesh;
class Shader;
class ParticleMesh;

class Renderer
{
public:
    explicit Renderer(GLFWwindow* window);
    ~Renderer();
    void setLight(int lightIndex, const Light& light);
    Light& getLight(int lightIndex);
    void render(Mesh* mesh, const glm::mat4& modelTransform, Shader* shader);
    void setCamera(Camera* camera);
    inline Camera* getCamera() const { return m_camera; }
    inline Camera* getDefaultCamera() { return &m_defaultCamera; }
    inline const glm::vec3 getAmbientLight() const
    {
        return glm::vec3(m_ambientLight);
    }
    inline void setAmbientLight(glm::vec3& ambientLight)
    {
        float maxAmbient = std::max(ambientLight.x, std::max(ambientLight.y, ambientLight.z));
        m_ambientLight = glm::vec4(ambientLight, maxAmbient);
    }
    void clearScreen(const glm::vec4& color, bool clearColorBuffer = true, bool clearDepthBuffer = true);
    // Update window with OpenGL rendering
    void swapWindow();
    void setupShader(const glm::mat4& modelTransform, Shader* shader);
    // flush GPU command buffer (must be called when profiling GPU time - should not be called when not profiling)
    void finishGPUCommandBuffer()
    {
        glFlush();
    }

public:
    static constexpr int s_maxSceneLights{ 4 };
    static constexpr int s_rgVersionMajor{ 1 };
    static constexpr int s_rgVersionMinor{ 0 };
    static constexpr int s_rgVersionPoint{ 2 };
    inline static Renderer* s_instance{ nullptr };
    friend class Camera;
    friend class Renderer;

private:
    glm::vec4 m_ambientLight = glm::vec4(0.2f, 0.2f, 0.2f, 0.2f);
    Light m_sceneLights[s_maxSceneLights];
    Camera m_defaultCamera;
    Camera* m_camera{ nullptr };
    GLFWwindow* m_window{ nullptr };
};
} // namespace re
#endif //SIMPLERENDERENGINE_RENDERER_H
