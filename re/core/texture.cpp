//
// Created by william on 2022/5/22.
//

#include "texture.h"

#include "commonMacro.h"
#include "font.inl"
#include "glCommonDefine.h"

#include <fstream>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

namespace
{
static std::vector<char> readAllBytes(char const* filename)
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
} // namespace

namespace re
{
Texture* Texture::createTextureFromFile(const char* filePath, bool generateMipmaps)
{
    int width{};
    int height{};
    int channels{};
    stbi_set_flip_vertically_on_load(true);
    int desireComp = STBI_rgb_alpha;
    auto pixelsData = readAllBytes(filePath);
    unsigned char* data = stbi_load_from_memory((stbi_uc const*)pixelsData.data(), pixelsData.size(), &width, &height, &channels, desireComp);
    stbi_set_flip_vertically_on_load(false);
    if (data)
    {
        auto* texture = new Texture((const char*)data, width, height, GL_RGBA, generateMipmaps);
        stbi_image_free(data);
        return texture;
    }
    return nullptr;
}

Texture* Texture::getWhiteTexture()
{
    if (s_whiteTexture != nullptr)
    {
        return s_whiteTexture;
    }
    char one = (char)0xff;
    std::vector<char> data(2 * 2 * 4, one);
    s_whiteTexture = createFromMem(data.data(), 2, 2, true);
    return s_whiteTexture;
}

Texture* Texture::getFontTexture()
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
    s_fontTexture = new Texture((const char*)data, width, height, GL_RGBA, true);
    stbi_image_free(data);
    return s_fontTexture;
}

Texture* Texture::createFromMem(const char* data, int width, int height, bool generateMipmaps)
{
    return new Texture(data, width, height, GL_RGBA, generateMipmaps);
}

Texture::Texture(const char* data, int width, int height, uint32_t format, bool generateMipmaps)
{
    m_info.width = width;
    m_info.height = height;
    m_info.generateMipmap = generateMipmaps;
    glGenTextures(1, &m_id);
    GLenum target = GL_TEXTURE_2D;
    GLint mipmapLevel = 0;
    GLint internalFormat = GL_RGBA;
    GLint border = 0;
    GLenum type = GL_UNSIGNED_BYTE;
    glBindTexture(target, m_id);
    glTexImage2D(target, mipmapLevel, internalFormat, width, height, border, format, type, data);
    updateTextureSampler();
    if (generateMipmaps)
    {
        glGenerateMipmap(GL_TEXTURE_2D);
    }
}

void Texture::updateTextureSampler() const
{
    glBindTexture(GL_TEXTURE_2D, m_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_info.wrapTextureCoordinates ? GL_REPEAT : GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_info.wrapTextureCoordinates ? GL_REPEAT : GL_CLAMP_TO_EDGE);
    GLint minification;
    GLint magnification;
    if (m_info.filterSampling)
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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magnification);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minification);
}
} // namespace re