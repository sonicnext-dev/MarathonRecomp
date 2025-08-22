#pragma once

#include <Marathon.inl>
#include <Sonicteam/MovieObjectWmv.h>

namespace Sonicteam
{
    class MovieObject : public SoX::RefCountObject
    {
    public:
        MARATHON_INSERT_PADDING(4);
        xpointer<MovieObjectWmv> m_pMovieObjectWmv;
        stdx::string m_Language;
        xpointer<MyGraphicsDevice> m_pMyGraphicsDevice;
        MARATHON_INSERT_PADDING(8);
        be<uint32_t> m_Width;
        be<uint32_t> m_Height;
        bool m_Field40;
    };
}
