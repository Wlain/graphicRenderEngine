//
// Created by william on 2022/6/1.
//

#ifndef SIMPLERENDERENGINE_SPRITEBATCH_H
#define SIMPLERENDERENGINE_SPRITEBATCH_H
#include "mesh.h"
#include "sprite.h"

#include <vector>
namespace ceres
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
        SpriteBatch::SpriteBatchBuilder& addSprites(T first, const T last)
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

} // namespace ceres

#endif // SIMPLERENDERENGINE_SPRITEBATCH_H
