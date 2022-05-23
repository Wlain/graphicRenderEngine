//
// Created by william on 2022/5/22.
//

#include "texture.h"

#include "commonMacro.h"
#include <OpenGL/gl3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

namespace re
{
GLenum getFormat(int channels)
{
    switch (channels)
    {
    case 1:
        return GL_RED;
    case 3:
        return GL_RGB;
    case 4:
        return GL_RGBA;
    default:
        ASSERT(0);
    }
}

Texture* Texture::createTextureFromPNG(const char* filePath, int size, bool generateMipmaps)
{
    int width{};
    int height{};
    int channels{};
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(filePath, &width, &height, &channels, 0);
    if (data)
    {
        auto* texture = new Texture(data, width, height, getFormat(channels), generateMipmaps);
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
    std::vector<unsigned char> data(2 * 2 * 4, one);
    s_whiteTexture = new Texture((unsigned char*)data.data(), 2, 2, GL_RGBA, true);
    return s_whiteTexture;
}

Texture::Texture(unsigned char* data, int width, int height, uint32_t format, bool generateMipmaps)
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
    glTexImage2D(target, mipmapLevel, internalFormat, width, height, border, format, type, data);
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