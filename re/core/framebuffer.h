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
        FrameBufferBuilder& withColorTexture(const std::shared_ptr<Texture>& texture);
        FrameBufferBuilder& withDepthTexture(const std::shared_ptr<Texture>& texture);
        FrameBufferBuilder& withName(std::string_view name);
        std::shared_ptr<FrameBuffer> build();

    private:
        std::vector<std::shared_ptr<Texture>> m_textures;
        std::shared_ptr<Texture> m_depthTexture;
        std::string m_name;
        glm::uvec2 m_size{};
        FrameBufferBuilder() = default;
        FrameBufferBuilder(const FrameBufferBuilder&) = default;
        friend class FrameBuffer;
    };

public:
    explicit FrameBuffer(std::string_view name);
    ~FrameBuffer();
    [[nodiscard]] inline const std::string& name() const { return m_name; }
    void setColorTexture(const std::shared_ptr<Texture>& tex, int index = 0);
    void setDepthTexture(const std::shared_ptr<Texture>& tex);
    static FrameBufferBuilder create();
    static int getMaximumColorAttachments();
    static int getMaximumDepthAttachments();

private:
    void bind();

private:
    std::vector<std::shared_ptr<Texture>> m_textures;
    std::shared_ptr<Texture> m_depthTexture;
    std::string m_name;
    uint32_t m_renderBufferDepth{ 0 };
    uint32_t m_fbo{ 0 };
    uint32_t m_rbo{ 0 };
    glm::vec2 m_size;
    bool m_dirty{ true };
    friend class RenderPass;
    friend class Inspector;
};
} // namespace re

#endif // SIMPLERENDERENGINE_FRAMEBUFFER_H
