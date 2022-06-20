//
// Created by william on 2022/5/22.
//

#ifndef SIMPLERENDERENGINE_TEXTURE_H
#define SIMPLERENDERENGINE_TEXTURE_H
#include <cstdlib>
#include <map>
#include <string>
#include <string_view>
#include <vector>

namespace re
{
class Texture : public std::enable_shared_from_this<Texture>
{
public:
    enum class PixelFormat : uint8_t
    {
        RGBA,
        RGB
    };

    enum class SamplerColorspace : uint8_t
    {
        Linear,
        Gamma
    };

    enum class CubeMapSide : uint8_t
    {
        PositiveX,
        NegativeX,
        PositiveY,
        NegativeY,
        PositiveZ,
        NegativeZ
    };

    enum class ResourceType : uint8_t
    {
        File,
        Memory
    };

    struct Info
    {
        PixelFormat format = PixelFormat::RGBA;
        SamplerColorspace colorspace = SamplerColorspace::Linear;
        ResourceType resourceType = ResourceType::File;
        std::string name;
        int width = 0;
        int height = 0;
        int channels = 0;
        uint32_t id = 0;
        uint32_t target = 0;
        bool transparent; // 透明纹理
        bool generateMipmap = false;
        bool filterSampling = true;
        bool wrapTextureCoordinates = true;
    };

public:
    class TextureBuilder
    {
    public:
        TextureBuilder(const TextureBuilder&) = delete;
        ~TextureBuilder();
        TextureBuilder& withGenerateMipmaps(bool enable);
        // if true texture sampling is filtered (bi-linear or tri-linear sampling) otherwise use point sampling.
        TextureBuilder& withFilterSampling(bool enable);
        TextureBuilder& withWrappedTextureCoordinates(bool enable);
        TextureBuilder& withFile(std::string_view filename);
        TextureBuilder& withFileCubeMap(std::string_view filename, CubeMapSide side);
        TextureBuilder& withRGBAData(const char* data, int width, int height);
        TextureBuilder& withWhiteData(int width = 1, int height = 1);
        TextureBuilder& withSamplerColorspace(SamplerColorspace samplerColorspace);
        TextureBuilder& withName(std::string_view name);
        std::shared_ptr<Texture> build();

    private:
        TextureBuilder();

    public:
        Info m_info{};
        friend class Texture;
    };

public:
    static TextureBuilder create();
    static std::shared_ptr<Texture> getWhiteTexture();
    static std::shared_ptr<Texture> getFontTexture();
    static std::shared_ptr<Texture> getSphereTexture();
    static std::shared_ptr<Texture> getCubeMapTexture();
    ~Texture();

public:
    inline int width() const { return m_info.width; }
    inline int height() const { return m_info.height; }
    inline const std::string& name() const { return m_info.name; }
    inline bool isFilterSampling() const { return m_info.filterSampling; }
    inline bool isWrapTextureCoordinates() const { return m_info.wrapTextureCoordinates; }
    bool isCubeMap() const;
    bool isMipMapped() const; // has texture mipmapped enabled
    bool isTransparent() const;
    void invokeGenerateMipmap();
    // get size of the texture in bytes on GPU
    size_t getDataSize() const;
    SamplerColorspace getSamplerColorSpace() const;

private:
    Texture(int32_t id, int width, int height, uint32_t target, std::string string);
    void updateTextureSampler(bool filterSampling, bool wrapTextureCoordinates) const;

private:
    inline static std::shared_ptr<Texture> s_whiteTexture{ nullptr };
    inline static std::shared_ptr<Texture> s_fontTexture{ nullptr };
    inline static std::shared_ptr<Texture> s_sphereTexture{ nullptr };
    inline static std::shared_ptr<Texture> s_cubeMapTexture{ nullptr };

private:
    Info m_info{};
    friend class Shader;
    friend class Material;
    friend class RenderPass;
    friend class FrameBuffer;
    friend class Inspector;
    friend class Sprite;
};
} // namespace re

#endif //SIMPLERENDERENGINE_TEXTURE_H
