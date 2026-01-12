#include <iostream>
#include <filesystem>
#include <fstream>

#include "validateFiles.h"

int main() {
    validate::createSoundList();
    std::vector<std::string> soundList {validate::checkSoundPaths()};
    if (soundList.empty()) {
        std::cout << "No Sounds.\n";
    }


    return 0;
}