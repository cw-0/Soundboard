#include <algorithm>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <map>
#include <windows.h>

#include "audio.h"
#include "utils.h"
#include "validateFiles.h"
#include "globals.h"
#include "sound.h"
#include "GUI.h"

int WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nShowCmd
    ){
    validate::createConfigIni();
    validate::getAbsoluteSounds();
    validate::getRelativeSounds();

    SBaudio::Audio_Init();

    GUI::Run();

    SBaudio::Audio_Shutdown();

    return 0;
}
