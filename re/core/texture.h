//
// Created by william on 2022/5/22.
//

#ifndef SIMPLERENDERENGINE_TEXTURE_H
#define SIMPLERENDERENGINE_TEXTURE_H
#include <cstdlib>
namespace re
{
class Texture
{
    enum class PixelFormat
    {
        RGBA,
        RGB
    };

    struct Info
    {
        PixelFormat format = PixelFormat::RGBA;
        int width = 0;
        int height = 0;
        int channels = 0;
        bool generateMipmap = false;
        bool filterSampling = true;
        bool wrapTextureCoordinates = true;
    };

public:
    static Texture* createTextureFromFile(const char* filePath, bool generateMipmaps = false);
    static Texture* getWhiteTexture();
    static Texture* getFontTexture();
    static Texture* getSphereTexture();
    static Texture* createFromMem(const char* data, int width, int height, bool generateMipmaps = false);
    ~Texture();
    inline int width() const { return m_info.width; }
    inline int height() const { return m_info.height; }
    inline bool isFilterSampling() const { return m_info.filterSampling; }
    inline void setFilterSampling(bool enable)
    {
        m_info.filterSampling = enable;
        updateTextureSampler();
    }
    inline bool isWrapTextureCoordinates() const { return m_info.wrapTextureCoordinates; }
    inline void setWrapTextureCoordinates(bool enable)
    {
        m_info.wrapTextureCoordinates = enable;
        updateTextureSampler();
    }

private:
    Texture(const char* data, int width, int height, uint32_t format, bool generateMipmaps);
    void updateTextureSampler() const;

private:
    inline static Texture* s_whiteTexture{ nullptr };
    inline static Texture* s_fontTexture{ nullptr };
    inline static Texture* s_sphereTexture{ nullptr };

private:
    friend class Shader;
    uint32_t m_id{ 0 };
    Info m_info{};
};
} // namespace re

#endif //SIMPLERENDERENGINE_TEXTURE_H
