//-----------------------------------------------------------------------------
// File: Human.h
//
// 
//-----------------------------------------------------------------------------
#pragma once
#include "AudioDeviceChild.h"
#include "Sound.h"

class Music : public Sound
{
public:

    Music() {}
    Music(const std::string& filepath);

};
