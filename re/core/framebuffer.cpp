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
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            LOG_ERROR("GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT");
            break;
        case GL_FRAMEBUFFER_UNSUPPORTED:
            LOG_ERROR("FRAMEBUFFER_UNSUPPORTED");
            break;
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
        default:
            LOG_ERROR("Invalid framebuffer {}", frameBufferStatus);
            break;
        }
    }
}

FrameBuffer::FrameBufferBuilder& FrameBuffer::FrameBufferBuilder::withColorTexture(const std::shared_ptr<Texture>& texture)
{
    ASSERT(!texture->isDepthTexture());
    if (!m_textures.empty() || m_depthTexture.get())
    {
        ASSERT(0);
    }
    this->m_size = { texture->width(), texture->height() };
    m_textures.push_back(texture);
    return *this;
}

FrameBuffer::FrameBufferBuilder& FrameBuffer::FrameBufferBuilder::withDepthTexture(const std::shared_ptr<Texture>& texture)
{
    ASSERT(texture->isDepthTexture());
    if (!m_textures.empty() || m_depthTexture.get())
    {
        ASSERT(0);
    }
    this->m_size = { texture->width(), texture->height() };
    m_depthTexture = texture;
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

    glGenFramebuffers(1, &framebuffer->m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->m_fbo);
    std::vector<GLenum> drawBuffers;
    for (unsigned i = 0; i < m_textures.size(); i++)
    {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_textures[i]->m_info.id, 0);
        drawBuffers.push_back(GL_COLOR_ATTACHMENT0 + i);
    }
    if (m_depthTexture)
    {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTexture->m_info.id, 0);
    }
    else
    {
        glGenRenderbuffers(1, &framebuffer->m_rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, framebuffer->m_rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, m_size.x, m_size.y);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_size.x, m_size.y);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        // attach the renderbuffer to depth attachment point
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, framebuffer->m_rbo);
    }
    // check FBO
    checkStatus();
    framebuffer->m_textures = m_textures;
    framebuffer->m_depthTexture = m_depthTexture;
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
    GLint maxAttach = 0;
    GLint maxDrawBuf = 0;
    glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxAttach);
    glGetIntegerv(GL_MAX_DRAW_BUFFERS, &maxDrawBuf);
    return std::min(maxAttach, maxDrawBuf);
}

int FrameBuffer::getMaximumDepthAttachments()
{
    return 1;
}

FrameBuffer::FrameBuffer(std::string_view name) :
    m_name(name)
{
    Renderer::s_instance->m_fbos.emplace_back(this);
}

void FrameBuffer::setColorTexture(const std::shared_ptr<Texture>& tex, int index)
{
    ASSERT(m_textures.size() > index && index >= 0);
    ASSERT(!tex->isDepthTexture());
    m_textures[index] = tex;
    m_dirty = true;
}

void FrameBuffer::setDepthTexture(const std::shared_ptr<Texture>& tex)
{
    ASSERT(tex->isDepthTexture());
    m_depthTexture = tex;
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
        if (m_depthTexture)
        {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTexture->m_info.id, 0);
        }
        m_dirty = false;
    }
}
} // namespace re