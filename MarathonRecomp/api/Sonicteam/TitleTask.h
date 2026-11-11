#pragma once

#include <Marathon.inl>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <Chao/CSD/Core/csdRCObject.h>
#include <Chao/CSD/Manager/csdmScene.h>
#include <Sonicteam/SoX/Engine/Doc.h>
#include <Sonicteam/SoX/Engine/DocMode.h>
#include <Sonicteam/SoX/Engine/Task.h>
#include <Sonicteam/CsdObject.h>
#include <Sonicteam/MovieTask.h>
#include <Sonicteam/TextBook.h>

namespace Sonicteam
{
    class AllStartMenuListener;

    class TitleTask : public SoX::Engine::Task
    {
    public:
        static constexpr float ms_DefaultMovieWaitTime = 30.0f;

        enum TitleState : uint32_t
        {
            TitleState_Open,
            TitleState_Wait,
            TitleState_PressStart,
            TitleState_OptionsOpen = 5,
            TitleState_OptionsWait,
            TitleState_OptionsProceed = 7,
            TitleState_Proceed,
            TitleState_Outro = 13
        };

        be<TitleState> m_State;
        xpointer<SoX::Engine::Doc> m_pDoc;
        boost::shared_ptr<AllStartMenuListener> m_pAllStartMenuListener;
        be<float> m_MovieWaitTime;
        be<uint32_t> m_Field60;
        MARATHON_INSERT_PADDING(0x08);
        xpointer<SoX::Engine::DocMode> m_pDocMode;
        MARATHON_INSERT_PADDING(0x0C);
        be<uint32_t> m_SelectedIndex;
        xpointer<MovieTask> m_pMovieTask;
        MARATHON_INSERT_PADDING(0x10);
        xpointer<TextBook> m_pTextBook;
        xpointer<CsdObject> m_pCsdObject;
        xpointer<Chao::CSD::RCObject<Chao::CSD::CScene>> m_pCsdScene;
        be<uint32_t> m_CastIndex;
        MARATHON_INSERT_PADDING(0x04);
    };

    MARATHON_ASSERT_OFFSETOF(TitleTask, m_State, 0x4C);
    MARATHON_ASSERT_OFFSETOF(TitleTask, m_pDoc, 0x50);
    MARATHON_ASSERT_OFFSETOF(TitleTask, m_pAllStartMenuListener, 0x54);
    MARATHON_ASSERT_OFFSETOF(TitleTask, m_MovieWaitTime, 0x5C);
    MARATHON_ASSERT_OFFSETOF(TitleTask, m_Field60, 0x60);
    MARATHON_ASSERT_OFFSETOF(TitleTask, m_pDocMode, 0x6C);
    MARATHON_ASSERT_OFFSETOF(TitleTask, m_SelectedIndex, 0x7C);
    MARATHON_ASSERT_OFFSETOF(TitleTask, m_pMovieTask, 0x80);
    MARATHON_ASSERT_OFFSETOF(TitleTask, m_pTextBook, 0x94);
    MARATHON_ASSERT_OFFSETOF(TitleTask, m_pCsdObject, 0x98);
    MARATHON_ASSERT_OFFSETOF(TitleTask, m_pCsdScene, 0x9C);
    MARATHON_ASSERT_OFFSETOF(TitleTask, m_CastIndex, 0xA0);
    MARATHON_ASSERT_SIZEOF(TitleTask, 0xA8);
}
