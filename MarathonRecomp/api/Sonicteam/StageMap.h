#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Component.h>
#include <stdx/string.h>

namespace stdx
{
    //Gauge PR :|
    template <typename Type>
    struct vector
    {
        be<uint32_t> _MyProxy;
        xpointer<Type> _MyFirst;
        xpointer<Type> _MyLast;
        xpointer<Type> _MyEnd;
        
        Type& operator[](size_t pos)
        {
            return _MyFirst.get()[pos];
        }

        size_t size()
        {
            return _MyLast.get() - _MyFirst.get();
        }

    };
}

namespace Sonicteam
{

    class StageMap : public SoX::Component
    {
    public:

        stdx::string m_Name;
        stdx::string m_Text;
        stdx::vector<xpointer<StageMap>> m_vpStageMap;

    };
}
