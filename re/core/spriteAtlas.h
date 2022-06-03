// Copyright (c) 2022. Lorem ipsum dolor sit amet, consectetur adipiscing elit.
// Morbi non lorem porttitor neque feugiat blandit. Ut vitae ipsum eget quam lacinia accumsan.
// Etiam sed turpis ac ipsum condimentum fringilla. Maecenas magna.
// Proin dapibus sapien vel ante. Aliquam erat volutpat. Pellentesque sagittis ligula eget metus.
// Vestibulum commodo. Ut rhoncus gravida arcu.

//
// Created by william on 2022/6/1.
//

#ifndef SIMPLERENDERENGINE_SPRITEATLAS_H
#define SIMPLERENDERENGINE_SPRITEATLAS_H
#include "sprite.h"

#include <map>
#include <string_view>
namespace re
{
class SpriteAtlas
{
public:
    ~SpriteAtlas();
    // create sprite atlas based on JSON file
    static std::shared_ptr<SpriteAtlas> create(std::string_view jsonFile, std::string_view imageFile);
    // create sprite atlas (with single sprite) using texture
    static std::shared_ptr<SpriteAtlas> createSingleSprite(const std::shared_ptr<Texture>& texture, std::string_view name = "sprite", glm::vec2 pivot = { 0.5f, 0.5f }, const glm::ivec2 pos = { 0, 0 }, glm::ivec2 size = { 0, 0 });
    Sprite get(std::string_view name); // Return a copy of a Sprite object.
    std::vector<std::string> getNames();
    const std::string& getAtlasName() const { return m_atlasName; };

private:
    SpriteAtlas(std::map<std::string, Sprite>&& sprites, std::shared_ptr<Texture> texture, std::string_view Atlas);

private:
    std::string m_atlasName;
    std::map<std::string, Sprite> m_sprites;
    std::shared_ptr<Texture> m_texture;
};
} // namespace re

#endif //SIMPLERENDERENGINE_SPRITEATLAS_H
