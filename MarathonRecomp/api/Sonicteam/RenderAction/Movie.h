#pragma once

#include <Marathon.inl>
#include <Sonicteam/MyRenderProcess.h>

namespace Sonicteam::RenderAction
{
    class Movie : public MyRenderProcess
    {
    public:
        xpointer<MovieObject> m_pMovieObject;
    };

    MARATHON_ASSERT_SIZEOF(Movie, 0x3C);
    MARATHON_ASSERT_OFFSETOF(Movie, m_pMovieObject, 0x38);
}
