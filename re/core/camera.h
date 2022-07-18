//
// Created by william on 2022/5/22.
//

#ifndef SIMPLERENDERENGINE_CAMERA_H
#define SIMPLERENDERENGINE_CAMERA_H
#include "ray.h"

#include <array>
#include <glm/glm.hpp>
namespace re
{
/**
 * class Camera
 */
class Camera
{
public:
    /**
     * 默认是正交投影：eye:(0,0,0)，看向z负轴，使用正交投影，[-1,1]
     */
    Camera();
    ~Camera();
    // set position of camera in world space (view transform) using
    void setLookAt(glm::vec3 eye, glm::vec3 at, glm::vec3 up);
    // set the camera view transform using worldspace position and rotation is degrees
    void setPositionAndRotation(glm::vec3 position, glm::vec3 rotationEulersDegrees);
    // return the camera position (computed from the view transform)
    glm::vec3 getPosition();
    glm::vec3 getRotationEuler();
    // 世界坐标到屏幕的获取射线
    Ray screenPointToRay(glm::vec2 position);
    void setPerspectiveProjection(float fieldOfViewY, float nearPlane, float farPlane);
    void setOrthographicProjection(float orthographicSize, float nearPlane, float farPlane);
    void setWindowCoordinates();
    inline void setViewTransform(const glm::mat4& viewTransform) { m_viewTransform = viewTransform; }
    void setProjectionTransform(const glm::mat4& projectionTransform);
    [[nodiscard]] inline glm::mat4 getViewTransform() const { return m_viewTransform; }
    glm::mat4 getProjectionTransform(const glm::uvec2& viewportSize);
    glm::mat4 getInfiniteProjectionTransform(glm::uvec2 viewportSize);
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

#endif // SIMPLERENDERENGINE_CAMERA_H
