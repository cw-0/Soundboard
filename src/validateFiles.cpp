#include <cstring>
#include <iostream>
#include "ini.h"

namespace validate {

    const mINI::INIFile soundIniFile("../assets/config.ini");
    mINI::INIStructure soundIniStruct;

    void createConfigIni() {
        // Find or Create .ini
        if (!soundIniFile.read(soundIniStruct)) {
            if (!soundIniFile.generate(soundIniStruct)) {
                std::cout << "Failed to generate config.ini\n";
                return;
            }
            soundIniStruct.clear();
            soundIniStruct["sounds"];
            soundIniFile.write(soundIniStruct);
            std::cout << "config.ini generated.\n";
        };

    }


    bool verifyMP3 (const std::filesystem::directory_entry& file) {
        if (file.is_regular_file()) {
            std::string ext = file.path().extension().string();
            std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
            if (ext == ".mp3") {
                // std::cout << "\033[32m" << file.path().filename() << " is a valid sound\033[0m" << "\n";
                return true;
            }
        }
        // std::cout << "\033[31m" << file.path().filename() << " is not a valid sound\033[0m" << "\n";
        return false;
    };



    std::vector<std::string> getAbsoluteSounds() {
        std::cout << "\nAbsolute Path Imports\n-----------------------------\n\n";
        std::vector<std::string> soundList;
        size_t soundCount {soundIniStruct["sounds"].size()};
        // std::cout <<  soundCount << " sounds found.\n";

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
                // std::cout << "VALUE: " << value << '\n';
                if (!std::filesystem::exists(std::filesystem::path(value))) {
                    std::cout << "\033[31mNot Found|\033[0m " << key << ": " << value << '\n';
                } else {
                    if (verifyMP3(std::filesystem::directory_entry(value))) {
                        std::cout << "\033[32mFound|\033[0m " << key << ": " << value << '\n';
                        soundList.push_back(value);
                    } else {
                        std::cout << "\033[31mNot .mp3|\033[0m " << key << ": " << value << '\n';
                    }
                }
            }
        }
        return soundList;
    }


    std::vector<std::string> getRelativeSounds() {
        std::cout << "\nRelative Path Imports\n-----------------------------\n\n";
        std::vector<std::string> relativeSounds;
        std::filesystem::path relativeSoundsPath {"../assets/sounds"};
        for (const auto& file: std::filesystem::directory_iterator(relativeSoundsPath)) {
            if (verifyMP3(file)) {
                std::cout << "\033[32mFound|\033[0m " << file.path().filename() << '\n';
                relativeSounds.push_back(file.path().string());
            } else {
                std::cout << "\033[31mNot .mp3|\033[0m " << file.path().filename() << '\n';
            }
        }
        return relativeSounds;
    }


}
