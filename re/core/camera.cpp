//
// Created by william on 2022/5/22.
//

#include "camera.h"

#include "glCommonDefine.h"
#include "renderer.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/transform.hpp>
namespace re
{
Camera::Camera()
{
    m_projectionValue.orthographic.orthographicSize = 1;
    m_projectionValue.orthographic.nearPlane = -1;
    m_projectionValue.orthographic.farPlane = 1;
}

Camera::~Camera() = default;

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
    default:
        return glm::mat4(1);
    }
}

void Camera::setPositionAndRotation(glm::vec3 position, glm::vec3 rotationEulersDegrees)
{
    auto rotationEulersRadians = glm::radians(rotationEulersDegrees);
    auto viewTransform = glm::translate(position) * glm::eulerAngleXYZ(rotationEulersRadians.x, rotationEulersRadians.y, rotationEulersRadians.z);
    setViewTransform(glm::inverse(viewTransform));
}

glm::vec3 Camera::getPosition()
{
    glm::vec3 scale;
    glm::quat orientation;
    glm::vec3 translation;
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(glm::inverse(m_viewTransform),
                   scale,
                   orientation,
                   translation,
                   skew,
                   perspective);
    return translation;
}

glm::vec3 Camera::getRotationEuler()
{
    glm::vec3 scale;
    glm::quat orientation;
    glm::vec3 translation;
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(glm::inverse(m_viewTransform),
                   scale,
                   orientation,
                   translation,
                   skew,
                   perspective);
    return glm::degrees(-glm::eulerAngles(orientation));
}

/// 屏幕像素点转换成射线:屏幕坐标转世界坐标
Ray Camera::screenPointToRay(glm::vec2 position)
{
    glm::vec2 viewportSize = (glm::vec2)Renderer::s_instance->getFramebufferSize() * m_viewportSize;
    // 将屏幕坐标归一化[0, 1]
    position = (position / viewportSize) * 2.0f - glm::vec2(1.0f);
    // 获取pv矩阵
    auto viewProjection = getProjectionTransform(viewportSize) * m_viewTransform;
    auto invViewProjection = glm::inverse(viewProjection);
    // 射线在投影空间的开始和结尾,其中0.1是near，1.0是far
    // 原始裁剪坐标
    glm::vec4 originClipSpace{ position, 1.0f, 1 };
    // 目标裁剪坐标
    glm::vec4 destClipSpace{ position, 0.1f, 1 };
    glm::vec4 originClipSpaceWS = invViewProjection * originClipSpace;
    glm::vec4 destClipSpaceWS = invViewProjection * destClipSpace;
    auto originClipSpaceWS3 = glm::vec3(originClipSpaceWS);
    auto destClipSpaceWS3 = glm::vec3(destClipSpaceWS);
    if (originClipSpaceWS.w != 0)
    {
        originClipSpaceWS3 /= originClipSpaceWS.w;
    }
    if (destClipSpaceWS.w != 0)
    {
        destClipSpaceWS3 /= destClipSpaceWS.w;
    }
    // 获取
    auto rayDirWorldSpace = glm::normalize(destClipSpaceWS3 - originClipSpaceWS3);
    return { originClipSpaceWS3, rayDirWorldSpace };
}

glm::mat4 Camera::getInfiniteProjectionTransform(glm::uvec2 viewportSize)
{
    switch (m_projectionType)
    {
    case ProjectionType::Perspective:
        return glm::tweakedInfinitePerspective(m_projectionValue.perspective.fieldOfViewY, float(viewportSize.x) / float(viewportSize.y), m_projectionValue.perspective.nearPlane);
    default:
        return getProjectionTransform(viewportSize);
    }
}

} // namespace re