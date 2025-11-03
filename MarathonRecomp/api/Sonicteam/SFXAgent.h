#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/MessageReceiver.h>
#include <Sonicteam/SoX/Math/Matrix.h>
#include <Sonicteam/SoX/Engine/RenderProcess.h>
#include <stdx/map.h>
#include <stdx/string.h>
#include <boost/smart_ptr/shared_ptr.h>

namespace Sonicteam
{
    class DocMarathonImp;

    struct SFXMatrixArrayCounted;
    struct SFXMatrixArrayNode;
    struct SFXMatrixArray;

    struct SFXMatrixArrayCounted
    {
    public:

        be<uint32_t> m_Capacity;
    };

    struct SFXMatrixArrayNode
    {
    public:
        xpointer<SoX::Math::Matrix4x4> m_pData[4];
        SFXMatrixArray* GetArray()
        {
            return reinterpret_cast<SFXMatrixArray*>((uint64_t*)((uint64_t)this - 4));
        }
    };

    struct SFXMatrixArray : SFXMatrixArrayCounted, SFXMatrixArrayNode
    {

    };


    class SFXAgent : public SoX::MessageReceiver
    {
    public:
        struct Vftable : public SoX::MessageReceiver::Vftable
        {
            be<uint32_t> fpReload;
            be<uint32_t> fpFieldC; //some with capture0
            be<uint32_t> fpField10; //some with capture0, yes again
        };

        xpointer<DocMarathonImp> m_pDoc;
        xpointer<SFXMatrixArrayNode> m_aSFXMatrices1;
        xpointer<SFXMatrixArrayNode> m_aSFXMatrices2;
        stdx::map<stdx::string, boost::shared_ptr<SoX::Engine::RenderProcess>> m_msspRenderProcess;
        MARATHON_INSERT_PADDING(0x8);

        void Reload()
        {
            auto vft = (Vftable*)(m_pVftable.get());
            GuestToHostFunction<void>(vft->fpReload, this);
        }
    };
    MARATHON_ASSERT_SIZEOF(SFXAgent, 0x24);
    MARATHON_ASSERT_OFFSETOF(SFXAgent, m_msspRenderProcess, 0x10);
}
