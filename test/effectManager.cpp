//
// Created by cwb on 2022/7/25.
//

#include "effectManager.h"

EffectManager::EffectManager() = default;

EffectManager::~EffectManager() = default;

void EffectManager::insertEffect(const std::shared_ptr<CommonInterface>& effect)
{
    m_effects.push_back(effect);
}
