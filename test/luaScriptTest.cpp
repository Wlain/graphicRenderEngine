//
// Created by cwb on 2022/7/14.
//

#include "engineTestSimple.h"
#include "utils/utils.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <sol/sol.hpp>
#include <string>
#include <utility>

class LuaScriptExample : public CommonInterface
{
public:
    using CommonInterface::CommonInterface;
    ~LuaScriptExample() override = default;
    void initialize() override
    {
        m_camera.setLookAt(m_eye, { 0.0, 0.0, 0.0f }, { 0.0f, 1.0f, 0.0f });
        m_camera.setPerspectiveProjection(45.0f, 0.1f, 100.0f);
        m_worldLights = MAKE_UNIQUE(m_worldLights);
        m_worldLights->addLight(Light::create().withDirectionalLight({ 1.0f, 1.0f, 1.0f }).withColor({ 1.0f, 1.0f, 1.0f }).build());
        m_luaScript = getFileContents("resources/luaScripts/test.lua");
        m_material = Shader::getStandardPBR()->createMaterial();
        m_material->setColor({ 1, 0, 0, 1 });
        m_mesh = Mesh::create().withSphere().build();
        m_lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::math);
        m_lua.script(m_luaScript);
    }

    void render() override
    {
        RenderPass rp = RenderPass::create()
                            .withCamera(m_camera)
                            .withWorldLights(m_worldLights.get())
                            .withClearColor(true, { 1, 1, 1 })
                            .build();
        rp.draw(m_mesh, glm::mat4(1), m_material);
    }


private:
    std::string m_luaScript;
    sol::state m_lua;
    std::function<double(int, int, double)> m_getPosFun;
    glm::vec3 m_eye{ 0, 0, 3 };
};

void luaScriptTest()
{
    GLFWRenderer renderer{};
    EngineTestSimple test(renderer);
    auto sceneNodeEffect = std::make_shared<LuaScriptExample>(&renderer);
    auto effect = std::make_shared<EffectManager>();
    effect->insertEffect(sceneNodeEffect);
    test.setEffect(effect);
    test.setTitle("LuaScriptExample");
    test.run();
}