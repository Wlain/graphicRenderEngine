// Copyright (c) 2022. Lorem ipsum dolor sit amet, consectetur adipiscing elit.
// Morbi non lorem porttitor neque feugiat blandit. Ut vitae ipsum eget quam lacinia accumsan.
// Etiam sed turpis ac ipsum condimentum fringilla. Maecenas magna.
// Proin dapibus sapien vel ante. Aliquam erat volutpat. Pellentesque sagittis ligula eget metus.
// Vestibulum commodo. Ut rhoncus gravida arcu.

//
// Created by william on 2022/5/31.
//

#include "framebuffer.h"

#include "glCommonDefine.h"
#include "renderer.h"
#include "texture.h"
namespace re
{
void checkStatus()
{
    GLenum frameBufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (frameBufferStatus != GL_FRAMEBUFFER_COMPLETE)
    {
        switch (frameBufferStatus)
        {
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            LOG_ERROR("GL_FRAMEBUFFER_UNDEFINED");
            break;
#ifdef GL_ES_VERSION_2_0
        case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
            LOG_ERROR("GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS");
            break;
#endif
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            LOG_ERROR("GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT");
            break;
        case GL_FRAMEBUFFER_UNSUPPORTED:
            LOG_ERROR("FRAMEBUFFER_UNSUPPORTED");
            break;
#ifndef GL_ES_VERSION_2_0
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
            LOG_ERROR("GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
            LOG_ERROR("GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER");
            break;
        case GL_FRAMEBUFFER_UNDEFINED:
            LOG_ERROR("FRAMEBUFFER_INCOMPLETE_ATTACHMENT");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
            LOG_ERROR("GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
            LOG_ERROR("GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS");
            break;
#endif
        default:
            LOG_ERROR("Invalid framebuffer {}", frameBufferStatus);
            break;
        }
    }
}

FrameBuffer::FrameBufferBuilder& FrameBuffer::FrameBufferBuilder::withTexture(const std::shared_ptr<Texture>& texture)
{
    this->m_size = { texture->width(), texture->height() };
    m_textures.push_back(texture);
    return *this;
}

FrameBuffer::FrameBufferBuilder& FrameBuffer::FrameBufferBuilder::withName(std::string_view name)
{
    m_name = name;
    return *this;
}

std::shared_ptr<FrameBuffer> FrameBuffer::FrameBufferBuilder::build()
{
    if (m_name.empty())
    {
        m_name = "Unnamed Framebuffer";
    }
    auto framebuffer = new FrameBuffer(m_name);
    framebuffer->m_size = m_size;
    glGenRenderbuffers(1, &framebuffer->m_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, framebuffer->m_rbo);
    glRenderbufferStorage(GL_RENDERBUFFER,
#ifdef GL_ES_VERSION_2_0
                          GL_DEPTH_COMPONENT16,
#else
                          GL_DEPTH_COMPONENT32,
#endif
                          m_size.x, m_size.y);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,
                          m_size.x, m_size.y);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glGenFramebuffers(1, &framebuffer->m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->m_fbo);
    for (unsigned i = 0; i < m_textures.size(); i++)
    {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_textures[i]->m_info.id, 0);
    }
    // attach the renderbuffer to depth attachment point
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, framebuffer->m_rbo);
    // check if FBO is configured correctly
    checkStatus();
    framebuffer->m_textures = m_textures;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return std::shared_ptr<FrameBuffer>(framebuffer);
}

FrameBuffer::~FrameBuffer()
{
    if (m_rbo != 0)
    {
        glDeleteRenderbuffers(1, &m_rbo);
    }
    auto* r = Renderer::s_instance;
    if (r != nullptr)
    {
        r->m_fbos.erase(std::remove(r->m_fbos.begin(), r->m_fbos.end(), this));
        glDeleteFramebuffers(1, &m_fbo);
    }
}

FrameBuffer::FrameBufferBuilder FrameBuffer::create()
{
    return {};
}

int FrameBuffer::getMaximumColorAttachments()
{
    return 1;
}

FrameBuffer::FrameBuffer(std::string_view name) :
    m_name(name)
{
    Renderer::s_instance->m_fbos.emplace_back(this);
}

void FrameBuffer::setTexture(std::shared_ptr<Texture> tex, int index)
{
    m_textures[index] = tex;
    m_dirty = true;
}

void FrameBuffer::bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    if (m_dirty)
    {
        for (int i = 0; i < m_textures.size(); i++)
        {
            for (unsigned i = 0; i < m_textures.size(); i++)
            {
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_textures[i]->m_info.id, 0);
            }
        }
        m_dirty = false;
    }
}
} // namespace re