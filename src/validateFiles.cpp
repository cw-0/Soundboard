#include <cstring>
#include <iostream>
#include "ini.h"

namespace validate {

    const mINI::INIFile soundIniFile("../assets/config.ini");
    mINI::INIStructure soundIniStruct;

    void createSoundList() {
        if (soundIniFile.read(soundIniStruct)) {
            std::cout << "config.ini found.\n";
            return;
        }
        soundIniStruct["SOUNDS"]["Default Sound"] = "../assets/sounds/cartoon_running.mp3";
        if (!soundIniFile.generate(soundIniStruct)) {
            std::cout << "Failed to generate sound list.\n";
            return;
        };
        std::cout << "config.ini generated.\n";
    }

    std::vector<std::string> checkSoundPaths() {
        std::vector<std::string> soundList;
        size_t soundCount {soundIniStruct["SOUNDS"].size()};
        std::cout <<  soundCount << " sounds found.\n";

        for (auto const& ini : soundIniStruct) {
            auto const& section = ini.first;
            auto const&  sounds = ini.second;
            if (std::strcmp(section.c_str(), "sounds") != 0) {
                std::cout << section << " != 'sounds'. Skipping.\n";
                continue;
            }
            for (const auto&[fst, snd] : sounds) {
                auto const& key = fst;
                auto const& value = snd;
                std::cout << "VALUE: " << value << '\n';
                std::filesystem::path p {value};
                if (!std::filesystem::exists(p)) {
                    std::cout << "\033[31mNot Found|\033[0m " << key << ": " << value << '\n';
                } else {
                    std::cout << "\033[32mFound|\033[0m " << key << ": " << value << '\n';
                    soundList.push_back(value);
                }
            }
        }
        return soundList;
    }



}