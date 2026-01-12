#include <algorithm>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <map>

#include "utils.h"
#include "validateFiles.h"
#include "globals.h"
#include "sound.h"

int main() {
    validate::createConfigIni();
    validate::getAbsoluteSounds();
    validate::getRelativeSounds();

    std::cout << "\n\n";

    int count{1};
    for (const auto& pair : soundVector) {
        std::cout << count << ". " << pair->soundName << ": " << pair->filePath << '\n';
        ++count;
    }



    return 0;
}
