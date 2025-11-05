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

    MARATHON_ASSERT_OFFSETOF(MovieObject, m_pMovieObjectWmv, 0x0C);
    MARATHON_ASSERT_OFFSETOF(MovieObject, m_Language, 0x10);
    MARATHON_ASSERT_OFFSETOF(MovieObject, m_pMyGraphicsDevice, 0x2C);
    MARATHON_ASSERT_OFFSETOF(MovieObject, m_Width, 0x38);
    MARATHON_ASSERT_OFFSETOF(MovieObject, m_Height, 0x3C);
    MARATHON_ASSERT_OFFSETOF(MovieObject, m_Field40, 0x40);
    MARATHON_ASSERT_SIZEOF(MovieObject, 0x44);
}
