//
// Created by william on 2022/6/29.
//

#ifndef SIMPLERENDERENGINE_RESOURCE_H
#define SIMPLERENDERENGINE_RESOURCE_H
#include <map>
#include <string>
namespace re
{
enum class ResourceType : uint8_t
{
    File = 0b001,
    Memory = 0b010,
    BuildIn = 0b100,
    All = 0b111
};
} // namespace re

#endif // SIMPLERENDERENGINE_RESOURCE_H
