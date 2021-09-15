#pragma once
#include "AudioDeviceChild.h"
#include "Sound.h"

class Music : public Sound
{
public:

    Music(std::string fname);
    Music(std::wstring fname);

};
