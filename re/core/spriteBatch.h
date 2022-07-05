//
// Created by william on 2022/6/1.
//

#ifndef SIMPLERENDERENGINE_SPRITEBATCH_H
#define SIMPLERENDERENGINE_SPRITEBATCH_H
#include "mesh.h"
#include "sprite.h"

#include <vector>
namespace re
{
class Shader;
class Material;
class SpriteBatch
{
public:
    class SpriteBatchBuilder
    {
    public:
        [[maybe_unused]] SpriteBatchBuilder& withShader(std::shared_ptr<Shader> shader);
        SpriteBatchBuilder& addSprite(Sprite sprite);
        template <typename T>
        SpriteBatchBuilder& addSprites(T first, const T last);
        std::shared_ptr<SpriteBatch> build();

    private:
        SpriteBatchBuilder();
        std::shared_ptr<Shader> m_shader;
        std::vector<Sprite> m_sprites;
        friend class SpriteBatch;
    };
    static SpriteBatchBuilder create();

private:
    SpriteBatch(std::shared_ptr<Shader> shader, std::vector<Sprite>&& sprites);

private:
    // 可以复用的资源
    std::vector<std::shared_ptr<Material>> m_materials;
    std::vector<std::shared_ptr<Mesh>> m_spriteMeshes;
    friend class RenderPass;
};

} // namespace re

#endif //SIMPLERENDERENGINE_SPRITEBATCH_H
