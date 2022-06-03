//
// Created by william on 2022/6/1.
//

#ifndef SIMPLERENDERENGINE_PROFILER_H
#define SIMPLERENDERENGINE_PROFILER_H
#include "framebuffer.h"
#include "renderStats.h"
#include "timer.h"
#include "worldLights.h"

#include <chrono>
#include <vector>

namespace re
{
class Texture;
class GLFWRenderer;
class Mesh;
class Shader;
class FrameBuffer;
class SpriteAtlas;

class Profiler
{
public:
    explicit Profiler(int frames = 300, GLFWRenderer* renderer = nullptr);

    void update();
    void gui(bool useWindow = true);

private:
    void showTexture(Texture* tex);
    void showMesh(Mesh* mesh);
    void showShader(Shader* shader);
    void showFramebufferObject(FrameBuffer* fbo);
    void showSpriteAtlas(SpriteAtlas* pAtlas);
    std::shared_ptr<Texture> getTmpTexture();
    void initFramebuffer();

private:
    WorldLights m_worldLights;
    std::vector<std::shared_ptr<Texture>> m_offscreenTextures;
    std::shared_ptr<FrameBuffer> m_framebuffer;
    std::vector<RenderStats> m_stats;
    std::vector<float> m_data;
    GLFWRenderer* m_renderer{ nullptr };
    std::vector<float> m_milliseconds;
    const float m_previewSize = 100;
    int m_usedTextures{ 0 };
    int m_frames{ 0 };
    int m_frameCount{ 0 };
    float m_time{ 0.0f };
    Timer m_timer;
};
} // namespace re

#endif //SIMPLERENDERENGINE_PROFILER_H
