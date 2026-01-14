#pragma once
#include <filesystem>

namespace validate {


    void createConfigIni();
    void getAbsoluteSounds();
    void getRelativeSounds();
    bool verifyMP3 (const std::filesystem::directory_entry& file);



}
