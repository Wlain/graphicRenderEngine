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
    static std::shared_ptr<SpriteAtlas> create(std::string jsonFile, std::string imageFile);

    Sprite get(std::string_view name);

    std::vector<std::string> getNames();

private:
    SpriteAtlas(std::map<std::string, Sprite>&& sprites, std::shared_ptr<Texture> texture);

private:
    std::map<std::string, Sprite> m_sprites;
    std::shared_ptr<Texture> m_texture;
};
} // namespace re

#endif //SIMPLERENDERENGINE_SPRITEATLAS_H
