//
// Created by william on 2022/5/23.
//

#ifndef SIMPLERENDERENGINE_TEXT_H
#define SIMPLERENDERENGINE_TEXT_H
namespace re
{
class Mesh;

class Text
{
public:
    // Creates a simple Text mesh with the origin in (0,0,0) and with the size (32,32,0) for each letter.
    // The width of a text mesh is 32*length
    static Mesh* createTextMesh(const char* text);
};
} // namespace re

#endif //SIMPLERENDERENGINE_TEXT_H
