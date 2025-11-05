#pragma once

#include <Sonicteam/SoX/Audio/Cue.h>

class MyCue : public Sonicteam::SoX::Audio::Cue {};

MARATHON_ASSERT_SIZEOF(MyCue, 0x2C);
