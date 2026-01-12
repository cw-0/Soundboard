#include <algorithm>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <map>

#include "utils.h"
#include "validateFiles.h"
#include "globals.h"

int main() {
    validate::createConfigIni();
    validate::getAbsoluteSounds();
    validate::getRelativeSounds();

    std::cout << "\n\n";

    int count{1};
    for (const auto& pair : soundDict) {
        std::cout << count << ". " << pair.first << ": " << pair.second << '\n';
        ++count;
    }


    return 0;
}