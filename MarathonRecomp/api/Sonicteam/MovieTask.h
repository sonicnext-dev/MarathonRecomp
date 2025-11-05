#pragma once

#include <Marathon.inl>

namespace Sonicteam
{
    class MovieTask : public SoX::Engine::Task, public CsdLink
    {
    public:
        MARATHON_INSERT_PADDING(0x40);
        xpointer<MovieObject> m_pMovieObject;
        MARATHON_INSERT_PADDING(0x18);
    };

    MARATHON_ASSERT_OFFSETOF(MovieTask, m_pMovieObject, 0xA4);
    MARATHON_ASSERT_SIZEOF(MovieTask, 0xC0);
}
