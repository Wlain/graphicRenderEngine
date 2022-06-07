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

std::shared_ptr<SpriteAtlas> SpriteAtlas::create(std::string_view jsonFile, std::string_view imageFile)
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
    auto texture = Texture::create().withFile(imageFile).build();
    for (auto& spriteElement : list)
    {
        std::string name = spriteElement["filename"].string_value();
        // "frame": {"x":154,"y":86,"w":92,"h":44},
        // "pivot": {"x":0.5,"y":0.5}
        const auto& frame = spriteElement["frame"];
        int x = frame["x"].int_value();
        int y = frame["y"].int_value();
        // size
        int w = frame["w"].int_value();
        int h = frame["h"].int_value();
        y = texture->height() - y - h;
        const auto& pivot = spriteElement["pivot"];
        // 锚点
        float px = pivot["x"].number_value();
        float py = pivot["y"].number_value();
        Sprite sprite({ x, y }, { w, h }, { px, py }, texture.get());
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
    Sprite sprite(pos, size, pivot, texture.get());
    sprites.emplace(std::pair<std::string, Sprite>(name, std::move(sprite)));
    return std::shared_ptr<SpriteAtlas>(new SpriteAtlas(std::move(sprites), texture, std::string(name) + "_atlas"));
}

} // namespace re