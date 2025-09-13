#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Object.h>

namespace Sonicteam::SoX
{
    namespace Engine
    {
        class DocMode;
    }

    class Component : public Object
    {
    public:
        xpointer<Engine::DocMode> m_pDocMode;
        MARATHON_INSERT_PADDING(0x18);

        template <typename T = Engine::DocMode>
        T* GetDocMode()
        {
            return (T*)m_pDocMode.get();
        }
    };
}
