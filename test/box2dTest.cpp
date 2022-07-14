//
// Created by william on 2022/7/12.
//
#include "basicProject.h"
#include "core/spriteAtlas.h"

#include <box2d/box2d.h>
#include <glm/ext.hpp>

class Box2DDebugDraw : public b2Draw
{
public:
    ~Box2DDebugDraw() override = default;
    Box2DDebugDraw()
    {
        SetFlags(b2Draw::e_shapeBit);
    }
    void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override
    {
        for (int i = 0; i < vertexCount; i++)
        {
            m_lines.emplace_back(vertices[i].x, vertices[i].y, 0);
            m_lines.emplace_back(vertices[(i + 1) % vertexCount].x, vertices[(i + 1) % vertexCount].y, 0);
        }
    }

    void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override
    {
        DrawPolygon(vertices, vertexCount, color);
    }

    void DrawCircle(const b2Vec2& center, float radius, const b2Color& color) override
    {
        glm::vec3 c{ center.x, center.y, 0 };
        for (int i = 0; i < 16; i++)
        {
            float v = i * glm::two_pi<float>() / 16;
            float v1 = (i + 1) * glm::two_pi<float>() / 16;
            m_lines.push_back(c + glm::vec3{ sin(v), cos(v), 0 } * radius);
            m_lines.push_back(c + glm::vec3{ sin(v1), cos(v1), 0 } * radius);
        }
    }

    void DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color) override
    {
        DrawCircle(center, radius, color);
    }

    void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) override
    {
        m_lines.emplace_back(p1.x, p1.y, 0);
        m_lines.emplace_back(p2.x, p2.y, 0);
    }

    void DrawTransform(const b2Transform& xf) override
    {
    }
    void DrawPoint(const b2Vec2& p, float size, const b2Color& color) override
    {
    }
    void clear()
    {
        m_lines.clear();
    }

    const std::vector<glm::vec3>& getLines()
    {
        return m_lines;
    }

private:
    std::vector<glm::vec3> m_lines;
};

class Box2dExample : public BasicProject
{
public:
    ~Box2dExample() override = default;
    void initialize() override
    {
        b2Vec2 gravity{ 0.0f, -980.0f };
        m_world = std::make_shared<b2World>(gravity);
        m_world->SetDebugDraw(&m_debugDraw);
        m_camera.setWindowCoordinates();
        m_spriteAtlas = SpriteAtlas::create("resources/sprites/marioPacked.json", "resources/sprites/marioPacked.png");

        b2BodyDef bodyDef;
        auto groundBody = m_world->CreateBody(&bodyDef);
        b2PolygonShape boxShape;
        boxShape.SetAsBox(10000, 100);

        b2FixtureDef boxFixtureDef;
        boxFixtureDef.shape = &boxShape;
        boxFixtureDef.density = 1;
        groundBody->CreateFixture(&boxFixtureDef);
        spawnBox(500, 500, 45);
    }

    void spawnBox(int posX, int posY, float angle)
    {
        auto names = m_spriteAtlas->getNames();
        // Set up random number distribution
        auto sprite = m_spriteAtlas->get(names.at(glm::linearRand<int>(0, 20)));
        const int size = 1;
        sprite.setScale({ size, size });
        sprite.setPosition({ posX, posY });
        sprite.setColor(glm::vec4{ glm::linearRand<float>(0.0f, 1.0f), glm::linearRand<float>(0.0f, 1.0f), glm::linearRand<float>(0.0f, 1.0f), 1.0f });
        m_sprites.push_back(sprite);

        b2BodyDef myBodyDef;
        myBodyDef.type = b2_dynamicBody;    // this will be a dynamic body
        myBodyDef.position.Set(posX, posY); // set the starting position
        myBodyDef.angle = angle;            // set the starting angle
        auto dynBody = m_world->CreateBody(&myBodyDef);
        b2PolygonShape boxShape;
        boxShape.SetAsBox(size, size);
        b2FixtureDef boxFixtureDef;
        boxFixtureDef.shape = &boxShape;
        boxFixtureDef.restitution = 0.8;
        boxFixtureDef.density = 10;
        dynBody->CreateFixture(&boxFixtureDef);
        m_physicsEntities.push_back(dynBody);
    }

    void cursorPosEvent(double xPos, double yPos) override
    {
        if (m_leftMousePressed)
        {
            auto framebufferSize = m_renderer.getFrameBufferSize();
            auto windowsSize = m_renderer.getWindowSize();
            auto ratio = framebufferSize.x / windowsSize.x;
            // 坐标映射
            auto mouseX = std::clamp((int)(xPos * ratio), 0, windowsSize.x * (int)ratio);
            auto mouseY = std::clamp((int)(windowsSize.y - yPos) * (int)ratio, 0, windowsSize.y * (int)ratio);
            spawnBox(mouseX, mouseY, 0);
        }
    }

    void render() override
    {
        m_world->Step(m_deltaTime, m_velocity, m_position);
        auto renderPass = RenderPass::create()
                              .withCamera(m_camera)
                              .withClearColor(true, { 1.0f, 1.f, 1.0f, 1.0f })
                              .build();
        for (int i = 0; i < m_sprites.size(); i++)
        {
            auto pos = m_physicsEntities[i]->GetPosition();
            auto rotation = glm::degrees(m_physicsEntities[i]->GetAngle());
            m_sprites[i].setPosition({ pos.x, pos.y });
            m_sprites[i].setRotation(rotation);
        }
        auto sb = SpriteBatch::create().addSprites(m_sprites.begin(), m_sprites.end()).build();
        renderPass.draw(sb);
        // debug draw
        m_world->DebugDraw();
        renderPass.drawLines(m_debugDraw.getLines(), { 1, 0, 0, 1 });
        m_debugDraw.clear();
    }
    void setTitle() override
    {
        m_title = "Box2dExample";
    }

private:
    std::shared_ptr<b2World> m_world;
    std::shared_ptr<SpriteAtlas> m_spriteAtlas;
    std::vector<Sprite> m_sprites;
    std::vector<b2Body*> m_physicsEntities; // 物理实例
    Box2DDebugDraw m_debugDraw;
    int32_t m_velocity = 8; // 速度
    int32_t m_position = 3;
    bool m_spawn = false;
};

void box2dTest()
{
    Box2dExample test;
    test.run();
}