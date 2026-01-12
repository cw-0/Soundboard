#include "sound.h"
#include "globals.h"
#include "utils.h"


Sound::Sound(const std::string& filePath) {
    this->filePath = filePath;
    this->soundName = soundboardUtils::stripSoundName(filePath);
}