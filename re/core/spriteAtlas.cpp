//
// Created by william on 2022/6/1.
//

#include "spriteAtlas.h"

#include "commonMacro.h"
#include "texture.h"
#include "utils.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <json11/json11.hpp>
#include <string>
#include <vector>

namespace re
{
std::shared_ptr<SpriteAtlas> SpriteAtlas::create(std::string jsonFile, std::string imageFile)
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
    auto& meta = json["meta"];
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
    return std::shared_ptr<SpriteAtlas>(new SpriteAtlas(std::move(sprites), texture));
}

Sprite SpriteAtlas::get(std::string_view name)
{
    if (m_sprites.find(name.data()) == m_sprites.end())
    {
        for (auto& s : m_sprites)
        {
            LOG_ERROR("{} {}", s.first, (name == s.first));
        }
        LOG_ERROR("Not found {} sprites.size:{}", name, m_sprites.size());
    }
    auto s = m_sprites[name.data()];
    const auto& ref = m_sprites[name.data()];
    if (ref.m_spriteSize.x == 0)
    {
        LOG_ERROR("ref {} ", glm::to_string(ref.m_spriteSize));
    }
    return s;
}

std::vector<std::string> SpriteAtlas::getNames()
{
    std::vector<std::string> names;
    for (const auto& e : m_sprites)
    {
        names.emplace_back(e.first);
    }
    return names;
}

SpriteAtlas::SpriteAtlas(std::map<std::string, Sprite>&& sprites, std::shared_ptr<Texture> texture)
{
    for (const auto& s : sprites)
    {
        m_sprites.insert({ s.first, s.second });
    }

    m_texture = std::move(texture);
}
} // namespace re