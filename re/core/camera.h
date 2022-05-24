//
// Created by william on 2022/5/22.
//

#ifndef SIMPLERENDERENGINE_CAMERA_H
#define SIMPLERENDERENGINE_CAMERA_H
#include <glm/glm.hpp>

namespace re
{
class Camera
{
public:
    Camera();
    void setLookAt(glm::vec3 eye, glm::vec3 at, glm::vec3 up);
    void setPerspectiveProjection(float fieldOfViewY, float viewportWidth, float viewportHeight, float nearPlane, float farPlane);
    void setOrthographicProjection(float left, float right, float bottom, float top, float zNear, float zFar);
    inline void setViewTransform(const glm::mat4& viewTransform) { m_viewTransform = viewTransform; }
    inline void setProjectionTransform(const glm::mat4& projectionTransform) { m_projectionTransform = projectionTransform; }
    inline glm::mat4 getViewTransform() const { return m_viewTransform; }
    inline glm::mat4 getProjectionTransform() const { return m_projectionTransform; };
    void setViewport(int x, int y, int width, int height);

private:
    glm::mat4 m_viewTransform{ 1.0f };
    glm::mat4 m_projectionTransform{ 1.0f };
    int m_viewportX{};
    int m_viewportY{};
    int m_viewportWidth{};
    int m_viewportHeight{};
    friend class Renderer;
};
} // namespace re

#endif //SIMPLERENDERENGINE_CAMERA_H
