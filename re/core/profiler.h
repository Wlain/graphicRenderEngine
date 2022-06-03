//
// Created by william on 2022/6/1.
//

#ifndef SIMPLERENDERENGINE_PROFILER_H
#define SIMPLERENDERENGINE_PROFILER_H
#include "renderStats.h"
#include "timer.h"

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
    void gui();

private:
    void showTexture(Texture* tex);
    void showMesh(Mesh* mesh);
    void showShader(Shader* shader);
    void showFramebufferObject(FrameBuffer* fbo);
    void showSpriteAtlas(SpriteAtlas* pAtlas);

private:
    std::vector<RenderStats> m_stats;
    std::vector<float> m_data;
    GLFWRenderer* m_renderer{ nullptr };
    std::vector<float> m_milliseconds;
    int m_frames{ 0 };
    int m_frameCount{ 0 };
    Timer m_timer;
};
} // namespace re

#endif //SIMPLERENDERENGINE_PROFILER_H
