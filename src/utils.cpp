#include <iostream>
#include <string>
#include <vector>

namespace soundboardUtils {


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
