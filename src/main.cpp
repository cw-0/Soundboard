#include <iostream>
#include <filesystem>
#include <fstream>

#include "utils.h"
#include "validateFiles.h"

int main() {
    validate::createConfigIni();
    std::vector<std::string> soundList {validate::getAbsoluteSounds()};
    std::vector<std::string> relativeSounds = validate::getRelativeSounds();
    for (const auto& sound : relativeSounds) {
        soundList.push_back(sound);
    }

    soundboardUtils::printLoadedSounds(soundList);


    return 0;
}