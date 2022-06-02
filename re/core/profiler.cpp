//
// Created by william on 2022/6/1.
//

#include "profiler.h"

#include "framebuffer.h"
#include "glCommonDefine.h"
#include "guiCommonDefine.h"
#include "mesh.h"
#include "renderer.h"
#include "shader.h"
#include "texture.h"

#include <string>

namespace re
{
namespace
{
std::string appendSize(std::string s, int size)
{
    if (size > 1)
    {
        s += "[" + std::to_string(size) + "]";
    }
    return s;
}

std::string glEnumToString(int type)
{
    std::string typeStr = "unknown";
    switch (type)
    {
    case GL_FLOAT:
        typeStr = "float";
        break;
    case GL_FLOAT_VEC2:
        typeStr = "vec2";
        break;
    case GL_FLOAT_VEC3:
        typeStr = "vec3";
        break;
    case GL_FLOAT_VEC4:
        typeStr = "vec4";
        break;
    case GL_FLOAT_MAT4:
        typeStr = "mat4";
        break;
    case GL_FLOAT_MAT3:
        typeStr = "mat3";
        break;
    case GL_INT_VEC4:
        typeStr = "ivec4";
        break;
    default:
        break;
    }
    return typeStr;
}

} // namespace

std::string glUniformToString(Shader::UniformType type)
{
    switch (type)
    {
    case Shader::UniformType::Float:
        return "float";
    case Shader::UniformType::Int:
        return "int";
    case Shader::UniformType::Mat3:
        return "mat3";
    case Shader::UniformType::Mat4:
        return "mat4";
    case Shader::UniformType::Texture:
        return "texture";
    case Shader::UniformType::TextureCube:
        return "texture cube";
    case Shader::UniformType::Vec3:
        return "vec3";
    case Shader::UniformType::Vec4:
        return "vec4";
    case Shader::UniformType::Invalid:
        return "Unsupported";
    }
    return "Unknown";
}

Profiler::Profiler(int frames, GLFWRenderer* renderer) :
    m_frames(frames), m_renderer(renderer)
{
    m_stats.resize(frames);
    m_milliseconds.resize(frames);
    m_timer.reset();
    m_data.resize(frames);
}

void Profiler::update()
{
    auto deltaTime = (float)m_timer.elapsedMicro();
    m_stats[m_frameCount % m_frames] = Renderer::s_instance->getRenderStats();
    m_milliseconds[m_frameCount % m_frames] = deltaTime;
    m_frameCount++;
}

void Profiler::gui()
{
    auto* r = Renderer::s_instance;
    static bool open = true;
    ImGui::Begin("re Renderer", &open);
    if (ImGui::CollapsingHeader("Renderer"))
    {
        ImGui::LabelText("Window size", "%ix%i", r->getWindowSize().x, r->getWindowSize().y);
        ImGui::LabelText("Framebuffer size", "%ix%i", r->getFramebufferSize().x, r->getFramebufferSize().y);
    }
    if (ImGui::CollapsingHeader("Performance"))
    {
        float max = 0;
        float sum = 0;
        for (int i = 0; i < m_frames; i++)
        {
            int idx = (m_frameCount + i) % m_frames;
            float t = m_milliseconds[idx];
            m_data[(-m_frameCount % m_frames + idx + m_frames) % m_frames] = t;
            max = std::max(max, t);
            sum += t;
        }
        float avg = 0;
        if (m_frameCount > 0)
        {
            avg = sum / std::min(m_frameCount, m_frames);
        }
        char res[128];
        sprintf(res, "Avg time: %4.2f ms\nMax time: %4.2f ms", avg, max);

        ImGui::PlotLines(res, m_data.data(), m_frames, 0, "Milliseconds", -1, max * 1.2f, ImVec2(ImGui::CalcItemWidth(), 150));

        max = 0;
        sum = 0;
        for (int i = 0; i < m_frames; i++)
        {
            int idx = (m_frameCount + i) % m_frames;
            float t = m_stats[idx].drawCalls;
            m_data[(-m_frameCount % m_frames + idx + m_frames) % m_frames] = t;
            max = std::max(max, t);
            sum += t;
        }
        avg = 0;
        if (m_frameCount > 0)
        {
            avg = sum / std::min(m_frameCount, m_frames);
        }
        sprintf(res, "Avg: %4.1f\nMax: %4.1f", avg, max);

        ImGui::PlotLines(res, m_data.data(), m_frames, 0, "Draw calls", -1, max * 1.2f, ImVec2(ImGui::CalcItemWidth(), 150));

        max = 0;
        sum = 0;
        for (int i = 0; i < m_frames; i++)
        {
            int idx = (m_frameCount + i) % m_frames;
            float t = m_stats[idx].stateChangesShader + m_stats[idx].stateChangesMaterial + m_stats[idx].stateChangesMesh;
            m_data[(-m_frameCount % m_frames + idx + m_frames) % m_frames] = t;
            max = std::max(max, t);
            sum += t;
        }
        avg = 0;
        if (m_frameCount > 0)
        {
            avg = sum / std::min(m_frameCount, m_frames);
        }
        sprintf(res, "Avg: %4.1f\nMax: %4.1f", avg, max);

        ImGui::PlotLines(res, m_data.data(), m_frames, 0, "State changes", -1, max * 1.2f, ImVec2(ImGui::CalcItemWidth(), 150));
    }
    if (ImGui::CollapsingHeader("Memory"))
    {
        float max = 0;
        float sum = 0;
        for (int i = 0; i < m_frames; i++)
        {
            int idx = (m_frameCount + i) % m_frames;
            float t = m_stats[idx].meshBytes / 1000000.0f;
            m_data[(-m_frameCount % m_frames + idx + m_frames) % m_frames] = t;
            max = std::max(max, t);
            sum += t;
        }
        float avg = 0;
        if (m_frameCount > 0)
        {
            avg = sum / std::min(m_frameCount, m_frames);
        }
        char res[128];
        sprintf(res, "Avg: %4.1f\nMax: %4.1f", avg, max);

        ImGui::PlotLines(res, m_data.data(), m_frames, 0, "Mesh MB", -1, max * 1.2f, ImVec2(ImGui::CalcItemWidth(), 150));

        max = 0;
        sum = 0;
        for (int i = 0; i < m_frames; i++)
        {
            int idx = (m_frameCount + i) % m_frames;
            float t = m_stats[idx].textureBytes / 1000000.0f;
            m_data[(-m_frameCount % m_frames + idx + m_frames) % m_frames] = t;
            max = std::max(max, t);
            sum += t;
        }
        avg = 0;
        if (m_frameCount > 0)
        {
            avg = sum / std::min(m_frameCount, m_frames);
        }
        sprintf(res, "Avg: %4.1f\nMax: %4.1f", avg, max);

        ImGui::PlotLines(res, m_data.data(), m_frames, 0, "Texture MB", -1, max * 1.2f, ImVec2(ImGui::CalcItemWidth(), 150));
    }
    if (ImGui::CollapsingHeader("Shaders"))
    {
        for (auto* s : r->m_shaders)
        {
            showShader(s);
        }
        if (r->m_shaders.empty())
        {
            ImGui::LabelText("", "No shaders");
        }
    }
    if (ImGui::CollapsingHeader("Textures"))
    {
        for (auto* t : r->m_textures)
        {
            showTexture(t);
        }
        if (r->m_textures.empty())
        {
            ImGui::LabelText("", "No textures");
        }
    }
    if (ImGui::CollapsingHeader("Meshes"))
    {
        for (auto* m : r->m_meshes)
        {
            showMesh(m);
        }
        if (r->m_meshes.empty())
        {
            ImGui::LabelText("", "No meshes");
        }
    }
    if (ImGui::CollapsingHeader("Framebuffer objects"))
    {
        for (auto fbo : r->m_fbos)
        {
            showFramebufferObject(fbo);
        }
        if (r->m_fbos.empty())
        {
            ImGui::LabelText("", "No framebuffer objects");
        }
    }
    ImGui::End();
}

void Profiler::showTexture(Texture* tex)
{
    std::string s = tex->name() + "##" + std::to_string((u_int64_t)tex);
    if (ImGui::TreeNode(s.c_str()))
    {
        ImGui::LabelText("Size", "%ix%i", tex->width(), tex->height());
        ImGui::LabelText("Cubemap", "%s", tex->isCubeMap() ? "true" : "false");
        ImGui::LabelText("Filtersampling", "%s", tex->isFilterSampling() ? "true" : "false");
        ImGui::LabelText("Wrap tex-coords", "%s", tex->isWrapTextureCoordinates() ? "true" : "false");
        ImGui::LabelText("Data size", "%f MB", tex->getDataSize() / (1000 * 1000.0f));
        if (!tex->isCubeMap())
        {
            ImGui::Image(reinterpret_cast<ImTextureID>(tex->m_info.id), ImVec2(100, 100));
        }
        ImGui::TreePop();
    }
}

void Profiler::showMesh(Mesh* mesh)
{
    std::string s = mesh->name() + "##" + std::to_string((u_int64_t)mesh);
    if (ImGui::TreeNode(s.c_str()))
    {
        ImGui::LabelText("Vertex count", "%i", mesh->getVertexCount());
        ImGui::LabelText("Mesh size", "%.2f MB", mesh->getDataSize() / (1000 * 1000.0f));
        if (ImGui::TreeNode("Vertex attributes"))
        {
            auto attributeNames = mesh->getAttributeNames();
            for (const auto& a : attributeNames)
            {
                auto type = mesh->getType(a);
                std::string typeStr = glEnumToString(type.first);
                typeStr = appendSize(typeStr, type.second);
                ImGui::LabelText(a.c_str(), "%s", typeStr.c_str());
            }
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("Index sets"))
        {
            if (mesh->getIndicesSet() == 0)
            {
                ImGui::LabelText("", "None");
            }
            else
            {
                for (int i = 0; i < mesh->getIndicesSet(); i++)
                {
                    char res[128];
                    sprintf(res, "Index %i size", i);
                    ImGui::LabelText(res, "%i", mesh->getIndicesSize(i));
                }
            }
            ImGui::TreePop();
        }
        ImGui::TreePop();
    }
}

void Profiler::showShader(Shader* shader)
{
    std::string s = shader->name() + "##" + std::to_string((u_int64_t)shader);
    if (ImGui::TreeNode(s.c_str()))
    {
        if (ImGui::TreeNode("Attributes"))
        {
            auto attributeNames = shader->getAttributeNames();
            for (const auto& a : attributeNames)
            {
                auto type = shader->getAttibuteType(a);
                std::string typeStr = glEnumToString(type.first);
                typeStr = appendSize(typeStr, type.second);
                ImGui::LabelText(a.c_str(), "%s", typeStr.c_str());
            }
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("Uniforms"))
        {
            auto uniformNames = shader->getUniformNames();
            for (const auto& a : uniformNames)
            {
                auto type = shader->getUniformType(a);
                std::string typeStr = glUniformToString(type.type);
                typeStr = appendSize(typeStr, type.arraySize);
                ImGui::LabelText(a.c_str(), "%s", typeStr.c_str());
            }
            ImGui::TreePop();
        }
        auto blend = shader->getBlend();
        std::string s;
        switch (blend)
        {
        case Shader::BlendType::AdditiveBlending:
            s = "Additive blending";
            break;
        case Shader::BlendType::AlphaBlending:
            s = "Alpha blending";
            break;
        case Shader::BlendType::Disabled:
            s = "Disabled";
            break;
        }
        ImGui::LabelText("Blending", "%s", s.c_str());
        ImGui::LabelText("Depth test", "%s", shader->isDepthTest() ? "true" : "false");
        ImGui::LabelText("Depth write", "%s", shader->isDepthWrite() ? "true" : "false");
        ImGui::LabelText("Offset","factor: %.1f units: %.1f",shader->getOffset().x,shader->getOffset().y);
        ImGui::TreePop();
    }
}

void Profiler::showFramebufferObject(FrameBuffer* fbo)
{
    std::string s = fbo->name() + "##" + std::to_string((u_int64_t)fbo);
    if (ImGui::TreeNode(s.c_str()))
    {
        ImGui::TreePop();
    }
}
} // namespace re