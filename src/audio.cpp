#include <iostream>
#include <thread>

#include "globals.h"
#include "miniaudio.h"

ma_uint64 frames;
ma_sound sound;
ma_engine pEngine;
ma_uint32 sampleRate = pEngine.sampleRate;

double soundLength{};

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

        if (soundGlobals::shouldLoop) {
            ma_result soundInitVal = ma_sound_init_from_file(&pEngine, path, 0, NULL, NULL, &sound);
            if ( soundInitVal == MA_SUCCESS) {
                if (!(ma_sound_get_length_in_pcm_frames(&sound, &frames) == MA_SUCCESS)) {
                    std::cout << "Failed to get sound length\n";
                }
               soundLength = double(frames) / double(sampleRate);
            }
            while (true) {
                if (!soundGlobals::shouldLoop) {
                    return;
                }
                ma_engine_play_sound(&gEngine, path, nullptr);
                std::this_thread::sleep_for(std::chrono::seconds((int)soundLength + 3));
            }
        }
        ma_engine_play_sound(&gEngine, path, nullptr);

    }


}
