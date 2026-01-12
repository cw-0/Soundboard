#pragma once
#include <vector>

namespace validate {


    void createConfigIni();
    std::vector<std::string> getAbsoluteSounds();
    std::vector<std::string> getRelativeSounds();
    bool verifyMP3 (const std::filesystem::directory_entry& file);



}
