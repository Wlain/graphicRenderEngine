//
// Created by william on 2022/5/22.
//

#include "camera.h"

#include "glCommonDefine.h"
#include "renderer.h"

#include <glm/gtc/matrix_transform.hpp>
namespace re
{
Camera::Camera()
{
    lazyInstantiateViewport();
}

void Camera::setLookAt(glm::vec3 eye, glm::vec3 at, glm::vec3 up)
{
    m_viewTransform = glm::lookAt(eye, at, up);
}

void Camera::setPerspectiveProjection(float fieldOfViewY, float nearPlane, float farPlane)
{
    lazyInstantiateViewport();
    m_projectionTransform = glm::perspectiveFov(glm::radians(fieldOfViewY), (float)m_viewportWidth, (float)m_viewportHeight, nearPlane, farPlane);
}

void Camera::setOrthographicProjection(float left, float right, float bottom, float top, float zNear, float zFar)
{
    m_projectionTransform = glm::ortho(left, right, bottom, top, zNear, zFar);
}

void Camera::setViewport(int x, int y, int width, int height)
{
    m_viewportX = x;
    m_viewportY = y;
    m_viewportWidth = width;
    m_viewportHeight = height;
}

void Camera::lazyInstantiateViewport()
{
    if (Renderer::s_instance && m_viewportWidth == -1)
    {
        auto size = Renderer::s_instance->getWindowSize();
        m_viewportWidth = size.x;
        m_viewportHeight = size.y;
    }
}

} // namespace re