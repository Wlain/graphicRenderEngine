//
// Created by william on 2022/6/1.
//

#include "spriteBatch.h"

#include "commonMacro.h"
#include "material.h"
namespace re
{
SpriteBatch::SpriteBatchBuilder& SpriteBatch::SpriteBatchBuilder::withShader(std::shared_ptr<Shader> shader)
{
    m_shader = std::move(shader);
    return *this;
}

SpriteBatch::SpriteBatchBuilder& SpriteBatch::SpriteBatchBuilder::addSprite(Sprite sprite)
{
    size_t size = m_sprites.size();
    if (size + 1 >= USHRT_MAX)
    {
        LOG_ERROR("More than %i sprites in a batch ", USHRT_MAX);
        return *this;
    }
    sprite.m_order.details.drawOrder = static_cast<uint16_t>(size);
    m_sprites.emplace_back(std::move(sprite));
    return *this;
}

std::shared_ptr<SpriteBatch> SpriteBatch::SpriteBatchBuilder::build()
{
    return std::shared_ptr<SpriteBatch>{ new SpriteBatch(m_shader, std::move(m_sprites)) };
}

SpriteBatch::SpriteBatchBuilder::SpriteBatchBuilder()
{
    m_shader = Shader::getUnlitSprite();
}

template <typename T>
SpriteBatch::SpriteBatchBuilder& SpriteBatch::SpriteBatchBuilder::addSprites(T first, const T last)
{
    auto start = m_sprites.end();
    int size = m_sprites.size();
    start = m_sprites.insert(m_sprites.end(), first, last);
    while (start != m_sprites.end())
    {
        (*start).m_order.details.drawOrder = static_cast<uint16_t>(size);
        size++;
        start++;
    }
    if (size >= std::numeric_limits<uint16_t>::max())
    {
        LOG_ERROR("More than %i sprites in a batch ", std::numeric_limits<uint16_t>::max());
        m_sprites.resize(std::numeric_limits<uint16_t>::max());
        return *this;
    }
    return *this;
}

SpriteBatch::SpriteBatchBuilder SpriteBatch::create()
{
    return {};
}

SpriteBatch::SpriteBatch(std::shared_ptr<Shader> shader, std::vector<Sprite>&& sprites)
{
    std::sort(sprites.begin(), sprites.end(), [](const Sprite& a, const Sprite& b) {
        return a.m_order.globalOrder > b.m_order.globalOrder;
    });
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec4> colors;
    std::vector<glm::vec4> uvs;
    std::vector<uint16_t> indices;
    Texture* lastTexture = nullptr;
    auto pushCurrentMesh = [&]() {
        m_spriteMeshes.push_back(Mesh::create()
                                     .withName(std::string("dynamicSpriteBatch") + std::to_string(m_spriteMeshes.size()))
                                     .withPositions(vertices)
                                     .withUvs(uvs)
                                     .withColors(colors)
                                     .withIndices(indices)
                                     .build());
        auto material = shader->createMaterial();
        material->setTexture(lastTexture->shared_from_this());
        m_materials.push_back(material);
    };
    // create meshs
    for (auto& s : sprites)
    {
        if (lastTexture && lastTexture != s.m_texture)
        {
            pushCurrentMesh();
            vertices.clear();
            colors.clear();
            uvs.clear();
            indices.clear();
        }
        lastTexture = s.m_texture;
        auto corners = s.getTrimmedCorners();
        auto cornerUvs = s.getUvs();
        auto idx = (int16_t)vertices.size();
        indices.push_back(idx);
        indices.push_back(idx + 1);
        indices.push_back(idx + 2);
        indices.push_back(idx);
        indices.push_back(idx + 2);
        indices.push_back(idx + 3);
        for (int i = 0; i < 4; i++)
        {
            vertices.emplace_back(corners[i], 0);
            uvs.emplace_back(cornerUvs[i], 0, 0);
            colors.emplace_back(s.m_color);
        }
    }
    if (vertices.size() > 0)
    {
        pushCurrentMesh();
    }
}
} // namespace re