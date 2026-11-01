#pragma once

#include <Marathon.inl>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <stdx/string.h>

namespace Sonicteam
{
    class MovieTask : public SoX::Engine::Task, public CsdLink
    {
    public:
        MARATHON_INSERT_PADDING(0x08);
        boost::anonymous_shared_ptr m_spEventSkipListener;
        MARATHON_INSERT_PADDING(0x0C);
        stdx::string m_FilePath;
        MARATHON_INSERT_PADDING(0x08);
        xpointer<MovieObject> m_pMovieObject;
        MARATHON_INSERT_PADDING(0x0C);
        be<uint32_t> m_Width;
        be<uint32_t> m_Height;
        MARATHON_INSERT_PADDING(0x04);
    };

    MARATHON_ASSERT_OFFSETOF(MovieTask, m_spEventSkipListener, 0x6C);
    MARATHON_ASSERT_OFFSETOF(MovieTask, m_FilePath, 0x80);
    MARATHON_ASSERT_OFFSETOF(MovieTask, m_pMovieObject, 0xA4);
    MARATHON_ASSERT_OFFSETOF(MovieTask, m_Width, 0xB4);
    MARATHON_ASSERT_OFFSETOF(MovieTask, m_Height, 0xB8);
    MARATHON_ASSERT_SIZEOF(MovieTask, 0xC0);
}
