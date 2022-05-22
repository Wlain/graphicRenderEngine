//
// Created by william on 2022/5/22.
//

#include "camera.h"

#include "renderer.h"

#include <OpenGL/gl3.h>
#include <glm/gtc/matrix_transform.hpp>
namespace re
{
Camera::Camera() = default;

void Camera::lookAt(glm::vec3 eye, glm::vec3 at, glm::vec3 up)
{
    m_viewTransform = glm::lookAt(eye, at, up);
}

void Camera::perspectiveProjection(float fieldOfViewY, float viewportWidth, float viewportHeight, float nearPlane, float farPlane)
{
    m_projectionTransform = glm::perspectiveFov(glm::radians(fieldOfViewY), viewportWidth, viewportHeight, nearPlane, farPlane);
}

void Camera::orthographicProjection(float left, float right, float bottom, float top, float zNear, float zFar)
{
    m_projectionTransform = glm::ortho(left, right, bottom, top, zNear, zFar);
}

void Camera::setViewport(int x, int y, int width, int height)
{
    m_viewportX = x;
    m_viewportY = y;
    m_viewportWidth = width;
    m_viewportHeight = height;
    if (Renderer::instance()->getCamera() == this)
    {
        glViewport(x, y, width, height);
        glScissor(x, y, width, height);
    }
}
} // namespace re