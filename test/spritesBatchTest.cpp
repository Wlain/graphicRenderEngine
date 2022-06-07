//
// Created by william on 2022/6/1.
//
#include "basicProject.h"
#include "core/sprite.h"
#include "core/spriteAtlas.h"
#include "core/spriteBatch.h"
#include "guiCommonDefine.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
class SpritesBatchExample : public BasicProject
{
public:
    using BasicProject::BasicProject;
    ~SpritesBatchExample() override = default;

    void initialize() override
    {
        m_spriteAtlas = SpriteAtlas::create(GET_CURRENT("test/resources/planetCute.json"), GET_CURRENT("test/resources/planetCute.png"));
        m_camera.setWindowCoordinates();
    }
    void render() override
    {
        auto renderPass = RenderPass::create()
                              .withCamera(m_camera)
                              .withClearColor(true, { .3, .3, 1, 1 })
                              .build();
        //        auto spriteBatchBuilder = SpriteBatch::create();
        //        auto sprite = m_spriteAtlas->get("lowerLeft.png");
        //        sprite.setPosition({ 100, 100 });
        //        spriteBatchBuilder.addSprite(sprite);
        //        renderPass.draw(spriteBatchBuilder.build());
        auto names = m_spriteAtlas->getNames();
        static bool demoWorld = true;
        ImGui::Checkbox("RenderWorld", &demoWorld);
        if (demoWorld)
        {
            if (m_spriteBatch == nullptr)
            {
                auto worldBuilder = SpriteBatch::create();
                glm::ivec2 blockSize(101, 50);

                std::vector<std::vector<int>> fields = {
                    { 0, 0, 10, 10, 9, 9, 10, 0, 0, 10, 10, 9, 9, 10 },
                    { 0, 0, 10, 10, 9, 9, 10, 0, 0, 10, 10, 9, 9, 10 },
                    { 0, 0, 10, 10, -1, -1, -1, 0, 0, 10, 10, -1, -1, -1 },
                };
                for (int y = 0; y < fields.size(); y++)
                {
                    for (int x = 0; x < fields[y].size(); x++)
                    {
                        int id = fields[y][x];
                        if (id == -1) continue;

                        auto sprite = m_spriteAtlas->get(names.at(id));
                        sprite.setPosition(glm::vec2(x * blockSize.x, y * blockSize.y));
                        worldBuilder.addSprite(sprite);
                    }
                }
                m_spriteBatch = worldBuilder.build();
            }
            static float d = 0;
            renderPass.draw(m_spriteBatch, glm::translate(glm::vec3(-500 + sin(d) * 500, 0, 0)));
            d += 0.016;
        }
        else
        {
            m_spriteBatch.reset();
            static const char** namesPtr = new const char*[names.size()];
            for (int i = 0; i < names.size(); i++)
            {
                namesPtr[i] = names[i].c_str();
            }
            static int selected = 0;
            ImGui::Combo("Sprite", &selected, namesPtr, names.size());
            auto sprite = m_spriteAtlas->get(names.at(selected));
            static glm::vec4 color(1, 1, 1, 1);
            static glm::vec2 scale(1, 1);
            static glm::vec2 position(200, 100);
            static float rotation = 0;
            static glm::bvec2 flip = { false, false };

            ImGui::ColorEdit4("Color", &color.x, true);
            ImGui::DragFloat2("Pos", &position.x, 1);
            ImGui::DragFloat("Rotation", &rotation, 1);
            ImGui::DragFloat2("Scale", &scale.x, 0.1);
            ImGui::Checkbox("Flip x", &flip.x);
            ImGui::Checkbox("Flip y", &flip.y);

            sprite.setColor(color);
            sprite.setScale(scale);
            sprite.setPosition(position);
            sprite.setRotation(rotation);
            sprite.setFlip(flip);

            auto sb = SpriteBatch::create()
                          .addSprite(sprite)
                          .build();
            renderPass.draw(sb);

            std::vector<glm::vec3> lines;
            auto spriteCorners = sprite.getCorners();
            for (int i = 0; i < 4; i++)
            {
                lines.emplace_back(spriteCorners[i], 0);
                lines.emplace_back(spriteCorners[(i + 1) % 4], 0);
            }
            renderPass.drawLines(lines);
        }
        auto& renderStats = Renderer::s_instance->getRenderStats();
        float bytesToMB = 1.0f / (1024 * 1024);
        ImGui::Text("re draw-calls %i meshes %i (%.2fMB) textures %i (%.2fMB) shaders %i", renderStats.drawCalls, renderStats.meshCount, renderStats.meshBytes * bytesToMB, renderStats.textureCount, renderStats.textureBytes * bytesToMB, renderStats.shaderCount);
        m_profiler.update();
        m_profiler.gui();
    }
    void setTitle() override
    {
        m_title = "SpritesBatchExample";
    }

private:
    std::shared_ptr<SpriteBatch> m_spriteBatch;
    std::shared_ptr<SpriteAtlas> m_spriteAtlas;
};

void spritesBatchTest()
{
    SpritesBatchExample test;
    test.run();
}