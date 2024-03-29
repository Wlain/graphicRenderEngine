//
// Created by william on 2022/5/22.
//

#include "texture.h"

#include "commonMacro.h"
#include "font.inl"
#include "glCommonDefine.h"
#include "renderer.h"

#include <glm/glm.hpp>
#include <utility>
#define STB_IMAGE_IMPLEMENTATION
#include "utils.h"

#include <stb/stb_image.h>

namespace
{
bool isPowerOfTwo(unsigned int x)
{
    // 例如：8:1000 7:111  8 & 7 == 0
    return ((x != 0) && !(x & (x - 1)));
}

} // namespace

namespace ceres
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

Texture::TextureBuilder& Texture::TextureBuilder::withFile(std::string_view filename, bool premultiplyAlpha)
{
    if (m_info.name.empty())
    {
        m_info.name = filename;
    }
    m_info.target = GL_TEXTURE_2D;
    GLint mipmapLevel = 0;
    GLint internalFormat = m_info.colorspace == SamplerColorspace::Linear ? GL_SRGB_ALPHA : GL_RGBA;
    GLint border = 0;
    GLenum type = GL_UNSIGNED_BYTE;
    int desireComp = STBI_rgb_alpha;
    stbi_set_unpremultiply_on_load(premultiplyAlpha);
    stbi_set_flip_vertically_on_load(true);
    auto pixelsData = getFileContents(filename.data());
    auto* data = (char*)stbi_load_from_memory((stbi_uc const*)pixelsData.data(), (int)pixelsData.size(), &m_info.width, &m_info.height, &m_info.channels, desireComp);
    stbi_set_flip_vertically_on_load(false);
    glBindTexture(m_info.target, m_info.id);
    bool isPOT = !isPowerOfTwo(m_info.width) || !isPowerOfTwo(m_info.height);
    if (!isPOT && m_info.filterSampling)
    {
        LOG_INFO("Texture {} is not power of two (was {} x {}) filter sampling", filename, m_info.width, m_info.height);
        m_info.filterSampling = false;
    }
    if (isPOT && m_info.generateMipmap)
    {
        LOG_INFO("Texture {} is not power of two (was {} x {}) mipmapping disabled ", filename, m_info.width, m_info.height);
        m_info.generateMipmap = false;
    }

    checkGlError();
    glTexImage2D(m_info.target, mipmapLevel, internalFormat, m_info.width, m_info.height, border, GL_RGBA, type, data);
    checkGlError();
    stbi_image_free((void*)data);
    return *this;
}

Texture::TextureBuilder& Texture::TextureBuilder::withRGBAData(const char* data, int width, int height)
{
    m_info.width = width;
    m_info.height = height;
    m_info.format = PixelFormat::RGBA;
    m_info.target = GL_TEXTURE_2D;
    GLint mipmapLevel = 0;
    GLint internalFormat = m_info.colorspace == SamplerColorspace::Linear ? GL_SRGB_ALPHA : GL_RGBA;
    GLint border = 0;
    GLenum type = GL_UNSIGNED_BYTE;
    glBindTexture(m_info.target, m_info.id);
    glTexImage2D(m_info.target, mipmapLevel, internalFormat, m_info.width, m_info.height, border, GL_RGBA, type, data);
    return *this;
}

Texture::TextureBuilder& Texture::TextureBuilder::withWhiteData(int width, int height)
{
    char one = (char)0xff;
    std::vector<char> dataOne(width * height * 4, one);
    withRGBAData(dataOne.data(), width, height);
    return *this;
}

Texture::TextureBuilder& Texture::TextureBuilder::withSamplerColorspace(Texture::SamplerColorspace samplerColorspace)
{
    m_info.colorspace = samplerColorspace;
    return *this;
}

Texture::TextureBuilder& Texture::TextureBuilder::withName(std::string_view name)
{
    m_info.name = name;
    return *this;
}

std::shared_ptr<Texture> Texture::TextureBuilder::build()
{
    if (m_info.target == 0)
    {
        LOG_ERROR("Texture contain no data");
    }
    if (m_info.id == 0)
    {
        throw std::runtime_error("Texture is already build");
    }
    if (m_info.name.empty())
    {
        m_info.name = "Unnamed Texture";
    }
    auto* texture = new Texture(m_info.id, m_info.width, m_info.height, m_info.target, m_info.name);
    texture->m_info.generateMipmap = m_info.generateMipmap;
    texture->m_depthPrecision = m_depthPrecision;
    if (m_info.generateMipmap)
    {
        texture->invokeGenerateMipmap();
    }
    texture->updateTextureSampler(m_info.filterSampling, m_info.warp);
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

Texture::TextureBuilder& Texture::TextureBuilder::withFileCubeMap(std::string_view filename, Texture::CubeMapSide side)
{
    if (m_info.name.empty())
    {
        m_info.name = filename;
    }
    m_info.target = GL_TEXTURE_CUBE_MAP;
    GLint mipmapLevel = 0;
    GLint internalFormat = GL_RGBA;
    GLint border = 0;
    GLenum type = GL_UNSIGNED_BYTE;
    int desireComp = STBI_rgb_alpha;
    auto pixelsData = getFileContents(filename.data());
    auto* data = (unsigned char*)stbi_load_from_memory((stbi_uc const*)pixelsData.data(), (int)pixelsData.size(), &m_info.width, &m_info.height, &m_info.channels, desireComp);
    glBindTexture(m_info.target, m_info.id);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + (uint32_t)side, mipmapLevel, internalFormat, m_info.width, m_info.height, border, GL_RGBA, type, data);
    stbi_image_free((void*)data);
    return *this;
}

Texture::TextureBuilder& Texture::TextureBuilder::withDepth(int width, int height, Texture::DepthPrecision precision)
{
    if (m_info.name.empty())
    {
        m_info.name = "Depth texture";
    }
    m_info.target = GL_TEXTURE_2D;
    GLint internalFormat;
    GLint format = GL_DEPTH_COMPONENT;
    GLenum type = GL_UNSIGNED_BYTE;
    m_depthPrecision = precision;
    if (m_depthPrecision == DepthPrecision::I16)
    {
        internalFormat = GL_DEPTH_COMPONENT16;
        type = GL_UNSIGNED_SHORT;
    }
    else if (m_depthPrecision == DepthPrecision::I24)
    {
        internalFormat = GL_DEPTH_COMPONENT24;
        type = GL_UNSIGNED_INT;
    }
    else if (m_depthPrecision == DepthPrecision::I32)
    {
        internalFormat = GL_DEPTH_COMPONENT32;
        type = GL_UNSIGNED_INT;
    }
    else if (m_depthPrecision == DepthPrecision::I24_STENCIL8)
    {
        internalFormat = GL_DEPTH24_STENCIL8;
        type = GL_UNSIGNED_INT;
        format = GL_DEPTH_STENCIL;
    }
    else if (m_depthPrecision == DepthPrecision::F32_STENCIL8)
    {
        internalFormat = GL_DEPTH32F_STENCIL8;
        type = GL_FLOAT_32_UNSIGNED_INT_24_8_REV;
        format = GL_DEPTH_STENCIL;
    }
    else if (m_depthPrecision == DepthPrecision::F32)
    {
        internalFormat = GL_DEPTH_COMPONENT32F;
        type = GL_FLOAT;
    }
    else if (m_depthPrecision == DepthPrecision::STENCIL8)
    {
        internalFormat = GL_STENCIL_INDEX8;
        format = GL_STENCIL_INDEX;
        type = GL_UNSIGNED_BYTE;
    }
    else
    {
        ASSERT(false);
    }
    m_info.width = width;
    m_info.height = height;
    GLint border = 0;
    glBindTexture(m_info.target, m_info.id);
    glTexImage2D(m_info.target, 0, internalFormat, width, height, border, format, type, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    // 指定边缘的颜色
    glm::vec4 ones(1.0, 1.0, 1.0, 1.0);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, &ones.x);
    checkGlError();
    return *this;
}

Texture::TextureBuilder& Texture::TextureBuilder::withWrapUV(Texture::Wrap wrap)
{
    m_info.warp = wrap;
    return *this;
}

std::shared_ptr<Texture> Texture::getWhiteTexture()
{
    if (s_whiteTexture != nullptr)
    {
        return s_whiteTexture;
    }
    s_whiteTexture = create().withWhiteData().withFilterSampling(false).withName("re Default White Texture").build();
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
    stbi_set_unpremultiply_on_load(1);
    stbi_set_flip_vertically_on_load(true);
    int desireComp = STBI_rgb_alpha;
    unsigned char* data = stbi_load_from_memory((stbi_uc const*)fontPng, sizeof(fontPng), &width, &height, &channels, desireComp);
    stbi_set_flip_vertically_on_load(false);
    s_fontTexture = Texture::create().withRGBAData((const char*)data, width, height).withName("re Default Font Texture").build();
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
            float distToCenter = glm::clamp(1.0f - 2.0f * glm::length(glm::vec2(((float)x + 0.5f) / size, ((float)y + 0.5f) / size) - glm::vec2(0.5f, 0.5f)), 0.0f, 1.0f);
            data[x * size * 4 + y * 4 + 0] = (char)(255 * distToCenter);
            data[x * size * 4 + y * 4 + 1] = (char)(255 * distToCenter);
            data[x * size * 4 + y * 4 + 2] = (char)(255 * distToCenter);
            data[x * size * 4 + y * 4 + 3] = (char)255;
        }
    }
    s_sphereTexture = create().withRGBAData(data.data(), size, size).withName("re Default Sphere Texture").build();
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

Texture::Texture(int32_t id, int width, int height, uint32_t target, std::string name)
{
    if (!Renderer::s_instance)
    {
        throw std::runtime_error("Cannot instantiate re::Texture before ceres::Renderer is created.");
    }
    m_info.id = id;
    m_info.width = width;
    m_info.height = height;
    m_info.target = target;
    m_info.name = std::move(name);
    // update stats
    RenderStats& renderStats = Renderer::s_instance->m_renderStatsCurrent;
    renderStats.textureCount++;
    auto dataSize = getDataSize();
    renderStats.textureBytes += (int)dataSize;
    renderStats.textureBytesAllocated += (int)dataSize;
    Renderer::s_instance->m_textures.emplace_back(this);
}

Texture::~Texture()
{
    auto* r = Renderer::s_instance;
    if (r != nullptr)
    {
        // update stats
        RenderStats& renderStats = r->m_renderStatsCurrent;
        renderStats.textureCount--;
        auto dataSize = getDataSize();
        renderStats.textureBytes -= (int)dataSize;
        renderStats.textureBytesDeallocated += (int)dataSize;
        glDeleteTextures(1, &m_info.id);
    }
}

void Texture::updateTextureSampler(bool filterSampling, Wrap warp) const
{
    glBindTexture(m_info.target, m_info.id);
    auto wrapEnum = warp == Wrap::ClampToEdge ? GL_CLAMP_TO_EDGE : (warp == Wrap::Mirror ? GL_MIRRORED_REPEAT : GL_REPEAT);
    glTexParameteri(m_info.target, GL_TEXTURE_WRAP_S, wrapEnum);
    glTexParameteri(m_info.target, GL_TEXTURE_WRAP_T, wrapEnum);
    GLint minification;
    GLint magnification;
    if (!filterSampling)
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
    if (!isPowerOfTwo((uint32_t)m_info.width) || !isPowerOfTwo((uint32_t)m_info.height))
    {
        LOG_ERROR("Ignore mipmaps for textures not power of two");
    }
    else
    {
        m_info.generateMipmap = true;
        glGenerateMipmap(m_info.target);
    }
}

size_t Texture::getDataSize() const
{
    int size = m_info.width * m_info.height * 4;
    if (m_info.generateMipmap)
    {
        size += (int)((1.0f / 3.0f) * (float)size);
    }
    // six sides
    if (m_info.target == GL_TEXTURE_CUBE_MAP)
    {
        size *= 6;
    }
    return size;
}

Texture::SamplerColorspace Texture::getSamplerColorSpace() const
{
    return m_info.colorspace;
}

bool Texture::isDepthTexture() const
{
    return m_depthPrecision != DepthPrecision::None;
}

Texture::DepthPrecision Texture::getDepthPrecision() const
{
    return m_depthPrecision;
}

bool Texture::isCubeMap() const
{
    return m_info.target == GL_TEXTURE_CUBE_MAP;
}

bool Texture::isMipMapped() const
{
    return m_info.generateMipmap;
}

bool Texture::isTransparent() const
{
    return m_info.transparent;
}

} // namespace ceres