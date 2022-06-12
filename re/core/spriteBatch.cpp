//
// Created by william on 2022/6/1.
//

#include "spriteBatch.h"

#include "material.h"
namespace re
{
SpriteBatch::SpriteBatchBuilder& SpriteBatch::SpriteBatchBuilder::withShader(std::shared_ptr<Shader> shader)
{
    m_shader = std::move(shader);
    return *this;
}

SpriteBatch::SpriteBatchBuilder& SpriteBatch::SpriteBatchBuilder::addSprite(const Sprite& sprite)
{
    m_sprites.emplace_back(sprite);
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
SpriteBatch::SpriteBatchBuilder& SpriteBatch::SpriteBatchBuilder::addSprite(T first, const T last)
{
    m_sprites.insert(m_sprites.end(), first, last);
    return *this;
}

SpriteBatch::SpriteBatchBuilder SpriteBatch::create()
{
    return {};
}

SpriteBatch::SpriteBatch(std::shared_ptr<Shader> shader, std::vector<Sprite>&& sprites)
{
    std::sort(sprites.begin(), sprites.end(), [](const Sprite& a, const Sprite& b) {
        if (a.m_orderInBatch < b.m_orderInBatch)
        {
            return true;
        }
        if (a.m_texture < b.m_texture)
        {
            return true;
        }
        return false;
    });
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec4> colors;
    std::vector<glm::vec4> uvs;
    std::vector<uint16_t> indices;
    Texture* lastTexture = nullptr;
    auto pushCurrentMesh = [&]() {
        m_spriteMeshes.push_back(Mesh::create().withPositions(vertices).withUvs(uvs).withColors(colors).withIndices(indices).build());
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
        auto corners = s.getCorners();
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