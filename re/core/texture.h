//
// Created by william on 2022/5/22.
//

#ifndef SIMPLERENDERENGINE_TEXTURE_H
#define SIMPLERENDERENGINE_TEXTURE_H
#include <cstdlib>
#include <vector>
namespace re
{
class Texture
{
public:
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
        uint32_t id = 0;
        uint32_t target = 0;
        bool generateMipmap = false;
        bool filterSampling = true;
        bool wrapTextureCoordinates = true;
    };

public:
    class TextureBuilder
    {
    public:
        TextureBuilder& withGenerateMipmaps(bool enable);
        // if true texture sampling is filtered (bi-linear or tri-linear sampling) otherwise use point sampling.
        TextureBuilder& withFilterSampling(bool enable);
        TextureBuilder& withWrappedTextureCoordinates(bool enable);
        TextureBuilder& withFile(const char* filename);
        TextureBuilder& withRGBData(const char* data, int width, int height);
        TextureBuilder& withRGBAData(const char* data, int width, int height);
        TextureBuilder& withWhiteData(int width = 2, int height = 2);
        Texture* build();
        TextureBuilder() = default;
        friend class Texture;

    public:
        std::vector<char> m_dataOne;
        const char* m_data = nullptr;
        const char* m_filename = nullptr;
        Info m_info{};
    };

public:
    static TextureBuilder create();
    static Texture* getWhiteTexture();
    static Texture* getFontTexture();
    static Texture* getSphereTexture();
    ~Texture();

public:
    inline int width() const { return m_info.width; }
    inline int height() const { return m_info.height; }
    inline bool isFilterSampling() const { return m_info.filterSampling; }
    inline bool isWrapTextureCoordinates() const { return m_info.wrapTextureCoordinates; }
    void invokeGenerateMipmap();

private:
    Texture(const char* data, int width, int height, uint32_t format);
    void updateTextureSampler(bool filterSampling, bool wrapTextureCoordinates);

private:
    inline static Texture* s_whiteTexture{ nullptr };
    inline static Texture* s_fontTexture{ nullptr };
    inline static Texture* s_sphereTexture{ nullptr };

private:
    friend class Shader;
    Info m_info{};
};
} // namespace re

#endif //SIMPLERENDERENGINE_TEXTURE_H
