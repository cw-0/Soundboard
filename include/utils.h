#pragma once
#include <string>
#include <vector>


namespace soundboardUtils {


    std::string stripSoundName(const std::string& path);
    void printLoadedSounds(std::vector<std::string>& soundList);

}


namespace soundboard {


    void playSound(size_t i);


}

