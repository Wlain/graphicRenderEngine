//
// Created by william on 2022/5/22.
//

#ifndef SIMPLERENDERENGINE_SHADER_H
#define SIMPLERENDERENGINE_SHADER_H
#include "light.h"

#include <glm/glm.hpp>
namespace re
{
enum class BlendingType
{
    Disabled,
    AlphaBlending
};
class Shader
{
public:
    static Shader* createShader(const char* vertexShader, const char* fragmentShader);
    static Shader* createUnlitColor();
    static Shader* createDebugUV();
    static Shader* createDebugNormals();
    Shader();
    ~Shader();
    bool setMatrix(const char* name, glm::mat4 value);
    bool setMatrix(const char* name, glm::mat3 value);
    bool setVector(const char* name, glm::vec4 value);
    bool setFloat(const char* name, float value);
    bool setInt(const char* name, int value);
    void bind();
    inline void setDepthTest(bool enable) { m_depthTest = enable; }
    inline bool isDepthTest() const { return m_depthTest; }
    inline void setDepthWrite(bool enable) { m_depthWrite = enable; }
    inline bool isDepthWrite() const { return m_depthWrite; }
    inline BlendingType getBlending() const { return m_blending; }

private:
    bool setLights(Light value[4]);
    friend class Mesh;
    friend class Renderer;

private:
    inline static Shader* s_unlitColor = nullptr;
    inline static Shader* s_debugUV = nullptr;
    inline static Shader* s_debugNormals = nullptr;
    inline static Shader* s_specularColor = nullptr;

private:
    unsigned int m_id{ 0 };
    bool m_depthTest{ true };
    bool m_depthWrite{ true };
    BlendingType m_blending{ BlendingType::Disabled };
};
} // namespace re

#endif //SIMPLERENDERENGINE_SHADER_H
