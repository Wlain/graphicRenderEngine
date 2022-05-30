//
// Created by william on 2022/5/22.
//

#include "texture.h"

#include "commonMacro.h"
#include "font.inl"
#include "glCommonDefine.h"
#include "renderer.h"

#include <fstream>
#include <glm/glm.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#ifndef GL_SRGB_ALPHA
    #define GL_SRGB_ALPHA 0x8C42
#endif
#ifndef GL_SRGB
    #define GL_SRGB 0x8C40
#endif

namespace
{
std::vector<char> readAllBytes(char const* filename)
{
    using namespace std;
    ifstream ifs(filename, std::ios::binary | std::ios::ate);
    ifstream::pos_type pos = ifs.tellg();
    if (pos < 0)
    {
        LOG_ERROR("Cannot read {}", filename);
        return {};
    }
    std::vector<char> result(pos);

    ifs.seekg(0, ios::beg);
    ifs.read(&result[0], pos);
    return result;
}

bool isPowerOfTwo(unsigned int x)
{
    // 例如：8:1000 7:111  8 & 7 == 0
    return ((x != 0) && !(x & (x - 1)));
}
} // namespace

namespace re
{
Texture::TextureBuilder& Texture::TextureBuilder::withGenerateMipmaps(bool enable)
{
    m_info.generateMipmap = enable;
    return *this;
}

Texture::TextureBuilder& Texture::TextureBuilder::withFilterSampling(bool enable)
{
    m_info.filterSampling = enable;
    return *this;
}

Texture::TextureBuilder& Texture::TextureBuilder::withWrappedTextureCoordinates(bool enable)
{
    m_info.wrapTextureCoordinates = enable;
    return *this;
}

Texture::TextureBuilder& Texture::TextureBuilder::withFile(const char* filename)
{
    m_info.target = GL_TEXTURE_2D;
    GLint mipmapLevel = 0;
    GLint internalFormat = GL_SRGB_ALPHA; //hasSRGB() ? GL_SRGB_ALPHA : GL_RGBA;
    GLint border = 0;
    GLenum type = GL_UNSIGNED_BYTE;
    checkGLError();
    int desireComp = STBI_rgb_alpha;
    stbi_set_flip_vertically_on_load(true);
    auto pixelsData = readAllBytes(filename);
    auto* data = (char*)stbi_load_from_memory((stbi_uc const*)pixelsData.data(), pixelsData.size(), &m_info.width, &m_info.height, &m_info.channels, desireComp);
    stbi_set_flip_vertically_on_load(false);
    glBindTexture(m_info.target, m_info.id);
    checkGLError();
    glTexImage2D(m_info.target, mipmapLevel, internalFormat, m_info.width, m_info.height, border, GL_RGBA, type, data);
    checkGLError();
    stbi_image_free((void*)data);
    return *this;
}

Texture::TextureBuilder& Texture::TextureBuilder::withRGBAData(const char* data, int width, int height)
{
    m_info.width = width;
    m_info.height = height;
    m_info.format = PixelFormat::RGBA;
    m_info.target = GL_TEXTURE_2D;
    checkGLError();
    GLint mipmapLevel = 0;
    GLint internalFormat = GL_SRGB_ALPHA; //hasSRGB() ? GL_SRGB_ALPHA : GL_RGBA;
    GLint border = 0;
    GLenum type = GL_UNSIGNED_BYTE;
    glBindTexture(m_info.target, m_info.id);
    checkGLError();
    glTexImage2D(m_info.target, mipmapLevel, internalFormat, m_info.width, m_info.height, border, GL_RGBA, type, data);
    checkGLError();
    return *this;
}

Texture::TextureBuilder& Texture::TextureBuilder::withWhiteData(int width, int height)
{
    char one = (char)0xff;
    std::vector<char> dataOne(width * height * 4, one);
    withRGBAData(dataOne.data(), width, height);
    return *this;
}

std::shared_ptr<Texture> Texture::TextureBuilder::build()
{
    if (m_info.target == 0)
    {
        std::runtime_error("texture contain no data");
    }
    if (m_info.id == 0)
    {
        throw std::runtime_error("Texture is already build");
    }
    auto* texture = new Texture(m_info.id, m_info.width, m_info.height, m_info.target);
    if (m_info.generateMipmap)
    {
        texture->invokeGenerateMipmap();
    }
    texture->updateTextureSampler(m_info.filterSampling, m_info.wrapTextureCoordinates);
    m_info.id = 0;
    return std::shared_ptr<Texture>(texture);
}

Texture::TextureBuilder::~TextureBuilder()
{
    if (m_info.id != 0)
    {
        glDeleteTextures(1, &m_info.id);
    }
}

Texture::TextureBuilder::TextureBuilder()
{
    glGenTextures(1, &m_info.id);
}

Texture::TextureBuilder& Texture::TextureBuilder::withFileCubeMap(const char* filename, Texture::CubeMapSide side)
{
    m_info.target = GL_TEXTURE_CUBE_MAP;
    GLint mipmapLevel = 0;
    GLint internalFormat = GL_RGBA;
    GLint border = 0;
    GLenum type = GL_UNSIGNED_BYTE;
    int desireComp = STBI_rgb_alpha;
    auto pixelsData = readAllBytes(filename);
    auto* data = (unsigned char*)stbi_load_from_memory((stbi_uc const*)pixelsData.data(), pixelsData.size(), &m_info.width, &m_info.height, &m_info.channels, desireComp);
    glBindTexture(m_info.target, m_info.id);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + (uint32_t)side, mipmapLevel, internalFormat, m_info.width, m_info.height, border, GL_RGBA, type, data);
    stbi_image_free((void*)data);
    return *this;
}

std::shared_ptr<Texture> Texture::getWhiteTexture()
{
    if (s_whiteTexture != nullptr)
    {
        return s_whiteTexture;
    }
    s_whiteTexture = create().withWhiteData().withFilterSampling(false).build();
    return s_whiteTexture;
}

std::shared_ptr<Texture> Texture::getFontTexture()
{
    if (s_fontTexture != nullptr)
    {
        return s_fontTexture;
    }
    int width{};
    int height{};
    int channels{};
    stbi_set_flip_vertically_on_load(true);
    int desireComp = STBI_rgb_alpha;
    unsigned char* data = stbi_load_from_memory((stbi_uc const*)fontPng, sizeof(fontPng), &width, &height, &channels, desireComp);
    stbi_set_flip_vertically_on_load(false);
    s_fontTexture = Texture::create().withRGBAData((const char*)data, width, height).build();
    stbi_image_free(data);
    return s_fontTexture;
}

std::shared_ptr<Texture> Texture::getSphereTexture()
{
    if (s_sphereTexture != nullptr)
    {
        return s_sphereTexture;
    }
    int size = 128;
    char one = (char)0xff;
    std::vector<char> data(size * size * 4, one);
    for (int x = 0; x < size; x++)
    {
        for (int y = 0; y < size; y++)
        {
            float distToCenter = glm::clamp(1.0f - 2.0f * glm::length(glm::vec2((x + 0.5f) / size, (y + 0.5f) / size) - glm::vec2(0.5f, 0.5f)), 0.0f, 1.0f);
            data[x * size * 4 + y * 4 + 0] = (char)(255 * distToCenter);
            data[x * size * 4 + y * 4 + 1] = (char)(255 * distToCenter);
            data[x * size * 4 + y * 4 + 2] = (char)(255 * distToCenter);
            data[x * size * 4 + y * 4 + 3] = (char)255;
        }
    }
    s_sphereTexture = create().withRGBAData(data.data(), size, size).build();
    return s_sphereTexture;
}

std::shared_ptr<Texture> Texture::getCubeMapTexture()
{
    return nullptr;
}

Texture::TextureBuilder Texture::create()
{
    return {};
}

Texture::Texture(int32_t id, int width, int height, uint32_t target)
{
    if (!Renderer::s_instance)
    {
        throw std::runtime_error("Cannot instantiate re::Texture before re::Renderer is created.");
    }
    m_info.id = id;
    m_info.width = width;
    m_info.height = height;
    m_info.target = target;
    // update stats
    RenderStats& renderStats = Renderer::s_instance->m_renderStatsCurrent;
    renderStats.textureCount++;
    renderStats.textureBytes += getDataSize();
}

Texture::~Texture()
{
    if (Renderer::s_instance)
    {
        // update stats
        RenderStats& renderStats = Renderer::s_instance->m_renderStatsCurrent;
        renderStats.textureCount--;
        renderStats.textureBytes -= getDataSize();
        glDeleteTextures(1, &m_info.id);
    }
}

void Texture::updateTextureSampler(bool filterSampling, bool wrapTextureCoordinates) const
{
    glBindTexture(m_info.target, m_info.id);
    glTexParameteri(m_info.target, GL_TEXTURE_WRAP_S, wrapTextureCoordinates ? GL_REPEAT : GL_CLAMP_TO_EDGE);
    glTexParameteri(m_info.target, GL_TEXTURE_WRAP_T, wrapTextureCoordinates ? GL_REPEAT : GL_CLAMP_TO_EDGE);
    GLint minification;
    GLint magnification;
    if (filterSampling)
    {
        minification = GL_NEAREST;
        magnification = GL_NEAREST;
    }
    else if (m_info.generateMipmap)
    {
        minification = GL_LINEAR_MIPMAP_LINEAR;
        magnification = GL_LINEAR;
    }
    else
    {
        minification = GL_LINEAR;
        magnification = GL_LINEAR;
    }
    glTexParameteri(m_info.target, GL_TEXTURE_MAG_FILTER, magnification);
    glTexParameteri(m_info.target, GL_TEXTURE_MIN_FILTER, minification);
}

void Texture::invokeGenerateMipmap()
{
    if (isPowerOfTwo((uint32_t)m_info.width) && isPowerOfTwo((uint32_t)m_info.height))
    {
        LOG_ERROR("Ignore mipmaps for textures not power of two");
    }
    else
    {
        m_info.generateMipmap = true;
        glGenerateMipmap(m_info.target);
    }
}

bool Texture::isCubeMap() const
{
    return m_info.target != GL_TEXTURE_2D;
}
} // namespace re