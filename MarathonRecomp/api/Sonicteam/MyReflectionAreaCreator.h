#pragma once

#include <Marathon.inl>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <Sonicteam/Game.h>
#include <Sonicteam/ReflectionAreaCreator.h>

namespace Sonicteam
{
    class MyReflectionAreaCreator : public ReflectionAreaCreator
    {
    public:
        boost::shared_ptr<Game> m_spGame;

        inline MyReflectionAreaCreator* GetInstance()
        {
            return *reinterpret_cast<xpointer<MyReflectionAreaCreator>*>(MmGetHostAddress(0x82D3681C));
        }
    };
}
