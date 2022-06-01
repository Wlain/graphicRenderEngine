//
// Created by william on 2022/5/22.
//

#include "camera.h"

#include "glCommonDefine.h"
#include "renderer.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
namespace re
{
Camera::Camera()
{
    m_projectionValue.orthographic.orthographicSize = 1;
    m_projectionValue.orthographic.nearPlane = -1;
    m_projectionValue.orthographic.farPlane = 1;
}

void Camera::setLookAt(glm::vec3 eye, glm::vec3 at, glm::vec3 up)
{
    if (glm::length(eye - at) < std::numeric_limits<float>::epsilon())
    {
        auto eyeStr = glm::to_string(eye);
        auto atStr = glm::to_string(at);
        LOG_WARN("Camera::setLookAt() invalid parameters. eye ({}) must be different from at ({})", eyeStr.c_str(), atStr.c_str());
    }
    setViewTransform(glm::lookAt<float>(eye, at, up));
}

void Camera::setPerspectiveProjection(float fieldOfViewY, float nearPlane, float farPlane)
{
    m_projectionValue.perspective.fieldOfViewY = glm::radians(fieldOfViewY);
    m_projectionValue.perspective.nearPlane = nearPlane;
    m_projectionValue.perspective.farPlane = farPlane;
    m_projectionType = ProjectionType::Perspective;
}

void Camera::setOrthographicProjection(float orthographicSize, float nearPlane, float farPlane)
{
    m_projectionValue.orthographic.orthographicSize = orthographicSize;
    m_projectionValue.orthographic.nearPlane = nearPlane;
    m_projectionValue.orthographic.farPlane = farPlane;
    m_projectionType = ProjectionType::Orthographic;
}

void Camera::setWindowCoordinates()
{
    m_projectionType = ProjectionType::OrthographicWindow;
}

void Camera::setViewport(const glm::vec2& offset, const glm::vec2& size)
{
    m_viewportOffset = offset;
    m_viewportSize = size;
}

void Camera::setProjectionTransform(const glm::mat4& projectionTransform)
{
    memcpy(m_projectionValue.customProjectionMatrix, glm::value_ptr(projectionTransform), sizeof(glm::mat4));
    m_projectionType = ProjectionType::Custom;
}

glm::mat4 Camera::getProjectionTransform(const glm::uvec2& viewportSize)
{
    switch (m_projectionType)
    {
    case ProjectionType::Custom:
        return glm::make_mat4(m_projectionValue.customProjectionMatrix);
    case ProjectionType::Orthographic: {
        float aspect = (float)viewportSize.x / (float)viewportSize.y;
        float sizeX = aspect * m_projectionValue.orthographic.orthographicSize;
        return glm::ortho<float>(-sizeX, sizeX, -m_projectionValue.orthographic.orthographicSize, m_projectionValue.orthographic.orthographicSize, m_projectionValue.orthographic.nearPlane, m_projectionValue.orthographic.farPlane);
    }
    case ProjectionType::OrthographicWindow:
        return glm::ortho<float>(0, (float)viewportSize.x, 0, (float)viewportSize.y, 1.0f, -1.0f);
    case ProjectionType::Perspective:
        return glm::perspectiveFov<float>(m_projectionValue.perspective.fieldOfViewY,
                                          (float)viewportSize.x,
                                          (float)viewportSize.y,
                                          m_projectionValue.perspective.nearPlane,
                                          m_projectionValue.perspective.farPlane);
    }
    return glm::mat4(1);
}

} // namespace re