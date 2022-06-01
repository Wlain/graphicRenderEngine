// Copyright (c) 2022. Lorem ipsum dolor sit amet, consectetur adipiscing elit.
// Morbi non lorem porttitor neque feugiat blandit. Ut vitae ipsum eget quam lacinia accumsan.
// Etiam sed turpis ac ipsum condimentum fringilla. Maecenas magna.
// Proin dapibus sapien vel ante. Aliquam erat volutpat. Pellentesque sagittis ligula eget metus.
// Vestibulum commodo. Ut rhoncus gravida arcu.

//
// Created by william on 2022/6/1.
//

#ifndef SIMPLERENDERENGINE_UTILS_H
#define SIMPLERENDERENGINE_UTILS_H
#include <filesystem>
#include <fstream>

static std::string getFileContents(const std::filesystem::path& filename)
{
    std::ifstream in{ filename, std::ios::in | std::ios::binary };
    if (in)
    {
        std::string contents;
        in.seekg(0, std::ios::end);
        contents.resize(in.tellg());
        in.seekg(0, std::ios::beg);
        in.read(&contents[0], contents.size());
        in.close();
        return contents;
    }
    throw(errno);
}
#endif //SIMPLERENDERENGINE_UTILS_H
