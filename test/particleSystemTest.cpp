//
// Created by cwb on 2022/7/19.
//
#include "basicProject.h"

class ParticleSystemExample : public BasicProject
{
public:
    ~ParticleSystemExample() override = default;
    void resize(int width, int height) override
    {
        BasicProject::resize(width, height);
    }
    void render() override
    {
        BasicProject::render();
    }
    void update(float deltaTime) override
    {
        BasicProject::update(deltaTime);
    }
    void setTitle() override
    {
        m_title = "ParticleSystemExample"
    }
};

void particleSystemTest()
{
    ParticleSystemExample test;
    test.run();
}