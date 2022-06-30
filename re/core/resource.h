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
    static std::string loadText(std::string key);        // load resource from built-in, filesystem or memory
    static void set(std::string key, std::string value); // set memory resource
    static void reset();                                 // reset memory resources
private:
    static std::map<std::string, std::string> memoryOnlyResources;
};
} // namespace re

#endif //SIMPLERENDERENGINE_RESOURCE_H
