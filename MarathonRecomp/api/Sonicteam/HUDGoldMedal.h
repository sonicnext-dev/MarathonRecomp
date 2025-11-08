#pragma once

#include <Marathon.inl>

namespace Sonicteam
{
    class HUDGoldMedal : public SoX::RefCountObject, public SoX::Engine::Task
    {
    public:
        xpointer<CsdObject> m_pCsdObject;
    };
}
