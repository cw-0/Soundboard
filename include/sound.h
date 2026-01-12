#pragma once
#include <string>

class Sound {
public:
    explicit Sound(const std::string& filePath);

    std::string filePath;
    std::string soundName;
};
