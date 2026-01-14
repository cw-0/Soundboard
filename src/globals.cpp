#include "globals.h"
#include "sound.h"

std::vector<std::unique_ptr<Sound>> soundVector;

namespace soundGlobals {

    bool hotkeysOn { false };
    bool shouldLoop { false };

}