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
    // 默认是正交投影：eye:(0,0,0)，看向z负轴，使用正交投影，[-1,1]
    Camera();
    void setLookAt(glm::vec3 eye, glm::vec3 at, glm::vec3 up);
    void setPerspectiveProjection(float fieldOfViewY, float nearPlane, float farPlane);
    void setOrthographicProjection(float orthographicSize, float nearPlane, float farPlane);
    void setWindowCoordinates();
    inline void setViewTransform(const glm::mat4& viewTransform) { m_viewTransform = viewTransform; }
    void setProjectionTransform(const glm::mat4& projectionTransform);
    inline glm::mat4 getViewTransform() const { return m_viewTransform; }
    glm::mat4 getProjectionTransform(const glm::uvec2& viewportSize);
    void setViewport(const glm::vec2& offset, const glm::vec2& size);

private:
    enum class ProjectionType
    {
        Perspective,
        Orthographic,
        OrthographicWindow,
        Custom
    };

    union ProjectionValue
    {
        struct
        {
            float fieldOfViewY;
            float nearPlane;
            float farPlane;
        } perspective;
        struct
        {
            float orthographicSize;
            float nearPlane;
            float farPlane;
        } orthographic;
        float customProjectionMatrix[16];
    };

private:
    glm::mat4 m_viewTransform{ 1.0f };
    ProjectionValue m_projectionValue{ 1.0f };
    ProjectionType m_projectionType = ProjectionType::Orthographic;
    glm::vec2 m_viewportOffset = glm::vec2{ 0, 0 };
    glm::vec2 m_viewportSize = glm::vec2{ 1, 1 };
    friend class RenderPass;
};
} // namespace re

#endif //SIMPLERENDERENGINE_CAMERA_H
