#pragma once

#include <Marathon.inl>
#include <boost/smart_ptr/shared_ptr.h>
#include <Sonicteam/SoX/Engine/RenderProcess.h>
#include <Sonicteam/SoX/Math/Matrix.h>
#include <Sonicteam/SoX/MessageReceiver.h>
#include <stdx/map.h>
#include <stdx/string.h>

namespace Sonicteam
{
    struct SFXMatrixArray;

    struct SFXMatrixNode
    {
        xpointer<SoX::Math::Matrix4x4> pData[4];

        SFXMatrixArray* GetArray()
        {
            return reinterpret_cast<SFXMatrixArray*>((uint8_t*)this - 4);
        }
    };

    struct SFXMatrixArray
    {
        be<uint32_t> Length;
        SFXMatrixNode First;
    };

    class SFXAgent : public SoX::MessageReceiver
    {
    public:
        struct Vftable : public SoX::MessageReceiver::Vftable
        {
            be<uint32_t> fpReload;
            be<uint32_t> fpField0C;
            be<uint32_t> fpField10;
        };

        xpointer<SoX::Engine::Doc> m_pDoc;
        xpointer<SFXMatrixNode> m_aSFXMatrices1;
        xpointer<SFXMatrixNode> m_aSFXMatrices2;
        stdx::map<stdx::string, boost::shared_ptr<SoX::Engine::RenderProcess>> m_mRenderProcesses;
        MARATHON_INSERT_PADDING(8);

        void Reload()
        {
            GuestToHostFunction<void>(((Vftable*)m_pVftable.get())->fpReload, this);
        }
    };

    MARATHON_ASSERT_OFFSETOF(SFXAgent, m_mRenderProcesses, 0x10);
    MARATHON_ASSERT_SIZEOF(SFXAgent, 0x24);
}
