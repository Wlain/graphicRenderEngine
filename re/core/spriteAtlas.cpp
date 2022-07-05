//
// Created by william on 2022/6/1.
//

#include "spriteAtlas.h"

#include "commonMacro.h"
#include "renderer.h"
#include "texture.h"
#include "utils.h"

#include <json11/json11.hpp>
#include <string>
#include <vector>

namespace re
{
SpriteAtlas::~SpriteAtlas()
{
    auto r = Renderer::s_instance;
    if (r)
    {
        r->m_spriteAtlases.erase(std::remove(r->m_spriteAtlases.begin(), r->m_spriteAtlases.end(), this));
    }
}

std::shared_ptr<SpriteAtlas> SpriteAtlas::create(std::string_view jsonFile, std::string_view imageFile, bool flipAnchorY)
{
    auto texture = Texture::create().withFile(imageFile).build();
    return create(jsonFile, texture, flipAnchorY);
}

std::shared_ptr<SpriteAtlas> SpriteAtlas::create(std::string_view jsonFile, const std::shared_ptr<Texture>& texture, bool flipAnchorY)
{
    std::string err;
    const auto json = json11::Json::parse(getFileContents(jsonFile), err);
    if (!err.empty())
    {
        LOG_ERROR("{}", err.c_str());
        return {};
    }
    std::map<std::string, Sprite> sprites;
    auto& list = json["frames"].array_items();
    for (auto& spriteElement : list)
    {
        std::string name = spriteElement["filename"].string_value();
        // "trimmed": true,
        // "rotated": false, // rotated sprites not supported
        // "spriteSourceSize": {"x":121,"y":94,"w":117,"h":131},
        // "sourceSize": {"w":256,"h":257},
        glm::ivec2 pos;
        glm::ivec2 size;
        glm::ivec2 sourcePos;
        glm::ivec2 sourceSize;
        glm::vec2 pivot;
        pos.x = (int)spriteElement["frame"]["x"].number_value();
        pos.y = (int)spriteElement["frame"]["y"].number_value();

        size.x = (int)spriteElement["frame"]["w"].number_value();
        size.y = (int)spriteElement["frame"]["h"].number_value();

        if (spriteElement["rotated"].bool_value())
        {
            LOG_ERROR("Rotated sprites not supported: {}", jsonFile.data());
        }

        if (spriteElement["trimmed"].bool_value())
        {
            sourcePos.x = (int)spriteElement["spriteSourceSize"]["x"].number_value();
            sourcePos.y = (int)spriteElement["spriteSourceSize"]["y"].number_value();
            sourceSize.x = (int)spriteElement["sourceSize"]["w"].number_value();
            sourceSize.y = (int)spriteElement["sourceSize"]["h"].number_value();
            float spriteHeight = (float )spriteElement["spriteSourceSize"]["h"].number_value();
            sourcePos.y = sourceSize.y - sourcePos.y - spriteHeight;
        }
        else
        {
            sourcePos = { 0, 0 };
            sourceSize = size;
        }
        pos.y = texture->height() - pos.y - size.y;
        if (spriteElement["pivot"].is_object())
        {
            pivot.x = (float)spriteElement["pivot"]["x"].number_value();
            pivot.y = (float)spriteElement["pivot"]["y"].number_value();
        }
        else
        {
            pivot.x = 0.5f;
            pivot.y = 0.5f;
        }
        if (flipAnchorY)
        {
            pivot.y = 1.0f - pivot.y;
        }
        Sprite sprite(pos, size, sourcePos, sourceSize, pivot, texture.get());
        sprites.emplace(name, std::move(sprite));
    }
    return std::shared_ptr<SpriteAtlas>(new SpriteAtlas(std::move(sprites), texture, jsonFile));
}

Sprite SpriteAtlas::get(std::string_view name)
{
    if (m_sprites.find(name.data()) == m_sprites.end())
    {
        LOG_WARN("Cannot find sprite {} in spriteatlas", name.data());
        return {};
    }
    return m_sprites[name.data()];
}

std::vector<std::string> SpriteAtlas::getNames()
{
    std::vector<std::string> names;
    for (const auto& e : m_sprites)
    {
        names.emplace_back(e.first);
    }
    return std::move(names);
}

SpriteAtlas::SpriteAtlas(std::map<std::string, Sprite>&& sprites, std::shared_ptr<Texture> texture, std::string_view atlasName) :
    m_atlasName(atlasName)
{
    for (const auto& s : sprites)
    {
        m_sprites.insert({ s.first, s.second });
    }

    m_texture = std::move(texture);
    Renderer::s_instance->m_spriteAtlases.push_back(this);
}

std::shared_ptr<SpriteAtlas> SpriteAtlas::createSingleSprite(const std::shared_ptr<Texture>& texture, std::string_view name, glm::vec2 pivot, glm::ivec2 pos, glm::ivec2 size)
{
    std::map<std::string, Sprite> sprites;
    if (size == glm::ivec2{ 0, 0 })
    {
        size.x = texture->width();
        size.y = texture->height();
    }
    Sprite sprite(pos, size, { 0, 0 }, size, pivot, texture.get());
    sprites.emplace(std::pair<std::string, Sprite>(name, std::move(sprite)));
    return std::shared_ptr<SpriteAtlas>(new SpriteAtlas(std::move(sprites), texture, std::string(name) + "_atlas"));
}

} // namespace re