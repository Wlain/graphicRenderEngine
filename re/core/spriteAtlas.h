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
    static std::shared_ptr<SpriteAtlas> create(std::string_view jsonFile, std::string_view imageFile, bool flipAnchorY = true);
    static std::shared_ptr<SpriteAtlas> create(std::string_view jsonFile, const std::shared_ptr<Texture>& texture, bool flipAnchorY = true);
    // create sprite atlas (with single sprite) using texture
    static std::shared_ptr<SpriteAtlas> createSingleSprite(const std::shared_ptr<Texture>& texture, std::string_view name = "sprite", glm::vec2 pivot = { 0.5f, 0.5f }, const glm::ivec2 pos = { 0, 0 }, glm::ivec2 size = { 0, 0 });
    Sprite get(std::string_view name); // Return a copy of a Sprite object.
    std::vector<std::string> getNames();
    const std::string& getAtlasName() const { return m_atlasName; };
    inline const std::shared_ptr<Texture>& getTexture() const { return m_texture; }; // return sprite texture

private:
    SpriteAtlas(std::map<std::string, Sprite>&& sprites, std::shared_ptr<Texture> texture, std::string_view Atlas);

private:
    std::string m_atlasName;
    std::map<std::string, Sprite> m_sprites;
    std::shared_ptr<Texture> m_texture;
};
} // namespace re

#endif //SIMPLERENDERENGINE_SPRITEATLAS_H
