// Copyright (c) 2022. Lorem ipsum dolor sit amet, consectetur adipiscing elit.
// Morbi non lorem porttitor neque feugiat blandit. Ut vitae ipsum eget quam lacinia accumsan.
// Etiam sed turpis ac ipsum condimentum fringilla. Maecenas magna.
// Proin dapibus sapien vel ante. Aliquam erat volutpat. Pellentesque sagittis ligula eget metus.
// Vestibulum commodo. Ut rhoncus gravida arcu.

//
// Created by william on 2022/5/31.
//

#ifndef SIMPLERENDERENGINE_FRAMEBUFFER_H
#define SIMPLERENDERENGINE_FRAMEBUFFER_H
#include <glm/glm.hpp>
#include <string>
#include <string_view>
#include <vector>

namespace re
{
class RenderPass;
class Texture;

class FrameBuffer
{
public:
    class FrameBufferBuilder
    {
    public:
        FrameBufferBuilder& withTexture(const std::shared_ptr<Texture>& texture);
        FrameBufferBuilder& withName(std::string_view name);
        std::shared_ptr<FrameBuffer> build();

    private:
        std::vector<std::shared_ptr<Texture>> m_textures;
        std::string m_name;
        glm::uvec2 m_size;
        FrameBufferBuilder() = default;
        FrameBufferBuilder(const FrameBufferBuilder&) = default;
        friend class FrameBuffer;
    };

public:
    FrameBuffer(std::string_view name);
    ~FrameBuffer();
    inline const std::string& name() const { return m_name; }
    void setTexture(std::shared_ptr<Texture> tex, int index = 0);
    static FrameBufferBuilder create();
    static int getMaximumColorAttachments();

private:
    void bind();

private:
    std::vector<std::shared_ptr<Texture>> m_textures;
    std::string m_name;
    uint32_t m_fbo{ 0 };
    uint32_t m_rbo{ 0 };
    glm::uvec2 m_size;
    bool m_dirty{ true };
    friend class RenderPass;
};
} // namespace re

#endif //SIMPLERENDERENGINE_FRAMEBUFFER_H
