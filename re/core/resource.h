//
// Created by william on 2022/6/29.
//

#ifndef SIMPLERENDERENGINE_RESOURCE_H
#define SIMPLERENDERENGINE_RESOURCE_H
#include <map>
#include <string>
namespace re
{
class Resource
{
public:
    enum class Type
    {
        File,
        Memory,
        BuildIn
    };

public:
    static std::string loadResource(std::string key);    // load resource from built-in, filesystem or memory
    static void set(std::string key, std::string value); // set memory resource
    static void reset();                                 // reset memory resources
private:
    static std::map<std::string, std::string> memoryOnlyResources;

private:
    Type m_type;
    std::string m_value;
};
} // namespace re

#endif //SIMPLERENDERENGINE_RESOURCE_H
