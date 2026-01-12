#include <iostream>

#include "miniaudio.h"



namespace SBaudio {
    static ma_engine gEngine;
    static bool gEngineReady = false;

    bool Audio_Init() {
        if (ma_engine_init(nullptr, &gEngine) != MA_SUCCESS) return false;
        gEngineReady = true;
        return true;
    }

    void Audio_Shutdown() {
        if (gEngineReady) ma_engine_uninit(&gEngine);
        gEngineReady = false;
    }

    void Audio_Play(const char* path) {
        if (!gEngineReady) return;
        std::cout << "Playing: " << path << '\n';
        ma_engine_play_sound(&gEngine, path, nullptr);
    }
}
