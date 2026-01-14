#pragma once

#include <Marathon.inl>

namespace Sonicteam::SoX::Graphics
{
    class IAnimation 
    {
    public:
        struct Vftable
        {
            be<uint32_t> fpDestroy;
            be<uint32_t> fpFunc4;
            be<uint32_t> fpFunc8;
            be<uint32_t> fpFuncC;
            be<uint32_t> fpFunc10;
            be<uint32_t> fpFunc14;
            be<uint32_t> fpFunc18;

        };

        xpointer<void> m_pVftable;
    };

}
