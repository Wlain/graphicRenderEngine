//
// Created by cwb on 2022/7/14.
//
#include "basicProject.h"
#include "guiCommonDefine.h"

#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include <sol/sol.hpp>

class LuaForMathematicalFuncExample : public BasicProject
{
public:
    ~LuaForMathematicalFuncExample() override = default;
    void initialize() override
    {
        m_camera.setOrthographicProjection(s_size, -1, 1); // 使用正交投影

        m_lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::math);

        // 注册回调函数
        // 数学pi
        m_lua.set_function("pi", [&](sol::this_state state, sol::variadic_args va) {
            return glm::pi<double>();
        });
        // 对浮点数取模（求余）
        m_lua.set_function("fmod", [&](sol::this_state state, sol::variadic_args va) {
            double x = va[0];
            double y = va[1];
            return fmod(x, y);
        });
        // 冲击函数
        m_lua.set_function("remainder", [&](sol::this_state state, sol::variadic_args va) {
            double x = va[0];
            double y = va[1];
            return remainder(x, y);
        });
        // 绝对值函数
        m_lua.set_function("abs", [&](sol::this_state state, sol::variadic_args va) {
            double x = va[0];
            return abs(x);
        });
        // 正弦函数
        m_lua.set_function("sin", [&](sol::this_state state, sol::variadic_args va) {
            double x = va[0];
            return sin(x);
        });
        // 余弦函数
        m_lua.set_function("cos", [&](sol::this_state state, sol::variadic_args va) {
            double x = va[0];
            return cos(x);
        });
        // 幂函数
        m_lua.set_function("pow", [&](sol::this_state state, sol::variadic_args va) {
            double x = va[0];
            double y = va[1];
            return pow(x, y);
        });
        updateLuaScript();

        m_positions.resize(2 * s_size / s_delta);
        updateFunction();
    }
    void updateLuaScript()
    {
        m_lua.script(luaScript);                     // evaluate lua script
        m_updatePosFunc = m_lua["mathematicalFunc"]; // get lua function update pos
    }

    void updateFunction()
    {
        try
        {
            if (m_error.empty())
            {
                int i = 0;
                float x = -s_size;
                while (x < s_size)
                {
                    m_positions[i] = { x, (float)m_updatePosFunc(x), 0 };
                    i++;
                    x += s_delta;
                }
            }
        }
        catch (sol::error err)
        {
            m_error = err.what();
        }
    }

    void render() override
    {
        // 函数顶点处理
        RenderPass rp = RenderPass::create()
                            .withCamera(m_camera)
                            .withClearColor(true, { 0.0, 0.0, 0.0, 1.0 })
                            .build();

        // draw grid
        for (int i = -s_size; i <= s_size; ++i)
        {
            if (i == 0)
            {
                rp.drawLines({ { i, -s_size, 0 }, { i, s_size, 0 } }, { 1, 0, 0, 1 });
                rp.drawLines({ { -s_size, i, 0 }, { s_size, i, 0 } }, { 0, 1, 0, 1 });
            }
            else
            {
                rp.drawLines({ { i, -s_size, 0 }, { i, s_size, 0 } }, { 0.33f, 0.33f, 0.33f, 1.0f });
                rp.drawLines({ { -s_size, i, 0 }, { s_size, i, 0 } }, { 0.33f, 0.33f, 0.33f, 1.0f });
            }
        }
        // draw Arrows
        // x-axis
        rp.drawLines({ { s_size, 0, 0 }, { s_size - 0.5, 0.5, 0 } }, { 0, 1, 0, 1 });
        rp.drawLines({ { s_size, 0, 0 }, { s_size - 0.5, -0.5, 0 } }, { 0, 1, 0, 1 });
        // y-axis
        rp.drawLines({ { 0, s_size, 0 }, { 0.5, s_size - 0.5, 0 } }, { 1, 0, 0, 1 });
        rp.drawLines({ { 0, s_size, 0 }, { -0.5, s_size - 0.5, 0 } }, { 1, 0, 0, 1 });
        // draw func
        rp.drawLines(m_positions, { 1.0, 1.0, 1.0, 1.0 }, Mesh::Topology::LineStrip);

        ImGui::InputTextMultiline("Lua script", luaScript, s_luaScriptSize);
        if (!m_error.empty())
        {
            ImGui::LabelText("Error", "%s", m_error.c_str());
        }

        if (ImGui::Button("Update"))
        {
            // 防止crash
            try
            {
                updateLuaScript();
                updateFunction();
                m_error = "";
            }
            catch (sol::error err)
            {
                m_error = err.what();
            }
        }
    }
    void setTitle() override
    {
        m_title = "LuaForMathematicalFuncExample";
    }

private:
    static constexpr const int s_size = 10;
    static constexpr const int s_luaScriptSize = 2048;
    static constexpr const float s_delta = 0.01f;

private:
    char luaScript[s_luaScriptSize] =
        "-- A visual demo of functions that can be updated in real time, which based lua script language:\n"
        "-- fmod(a,b), remainder(a,b), abs(a), sin(a), cos(a), pow(a), pi()\n"
        "-- default:y = x\n"
        "function mathematicalFunc(x) \n"
        " return sin(x) \n"
        "end";
    std::vector<glm::vec3> m_positions;
    std::string m_error;
    sol::state m_lua;
    std::function<double(double)> m_updatePosFunc;
};

void luaForMathematicalFuncTest()
{
    LuaForMathematicalFuncExample test;
    test.run();
}