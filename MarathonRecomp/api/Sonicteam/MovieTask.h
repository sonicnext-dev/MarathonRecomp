#pragma once

#include <Marathon.inl>

namespace Sonicteam
{
    class MovieTask : public SoX::Engine::Task, public CsdLink
    {
    public:
        MARATHON_INSERT_PADDING(0x40);
        xpointer<MovieObject> m_pMovieObject;
    };
}
