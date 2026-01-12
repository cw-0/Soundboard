#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include "globals.h"
#include "sound.h"


namespace soundboardUtils {


    std::string stripSoundName(const std::string& path) {
        // std::cout << "PATH: " << path << '\n';
        const size_t lastSlashPos = path.rfind('\\');
        const size_t lastDotPos = path.rfind('.');
        std::string soundName = path.substr(lastSlashPos + 1, (lastDotPos - lastSlashPos - 1));
        std::ranges::transform(soundName, soundName.begin(),
                               [](unsigned char c){ return std::tolower(c); });
        return soundName;
    }


    void printLoadedSounds(std::vector<std::string>& soundList) {
        std::cout << "\n\nLoaded Sounds\n----------------------\n\n";

        if (soundList.empty()) {
            std::cout << "No Sounds.\n";
        } else {
            int count {1};
            for (const auto& sound : soundList) {
                std::cout << count << ". " << sound << std::endl;
                ++count;
            }
        }
    }



}
