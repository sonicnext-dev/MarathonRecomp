#pragma once

#include <Marathon.inl>
#include <Sonicteam/Particles/Particle.h>
#include <boost/smart_ptr/shared_ptr.h>

namespace Sonicteam::Spanverse
{
    class AckTrunkList;
    class AckResource;
    class AckTrunk; // AKTR sub_122E9B8(PS3)
    class SpkCustomObject;
    class SpangleObject;

    class AckTrunkObject : public Sonicteam::Particles::Particle
    {
    public:

        enum Flags
        {
            Flags_1 = 1
        };

        enum CreationFlag
        {
            CreationFlag_Destroy = 2,
            CreationFlag_8 = 0x8,
            CreationFlag_50 = 0x50,
            CreationFlag_100 = 0x100,
            CreationFlag_700 = 0x700,
            CreationFlag_900 = 0x900,
        };
        MARATHON_INSERT_PADDING(0x4);
        be<uint32_t> m_Flags;
        MARATHON_INSERT_PADDING(0x4);
        xpointer<AckTrunkList> m_pParent;
        MARATHON_INSERT_PADDING(0x8);
        xpointer<AckResource> m_pResource;
        be<uint32_t> m_CreationFlag;
        xpointer<AckTrunk> m_pTrunk;
        boost::shared_ptr<SpkCustomObject> m_pObject; // Or SpangleObject

    };

    MARATHON_ASSERT_OFFSETOF(AckTrunkObject, m_Flags, 0xC);
    MARATHON_ASSERT_OFFSETOF(AckTrunkObject, m_pParent, 0x14);
    MARATHON_ASSERT_OFFSETOF(AckTrunkObject, m_pResource,0x20);
    MARATHON_ASSERT_OFFSETOF(AckTrunkObject, m_CreationFlag,0x24);
    MARATHON_ASSERT_OFFSETOF(AckTrunkObject, m_pTrunk,0x28);
    MARATHON_ASSERT_OFFSETOF(AckTrunkObject, m_pObject,0x2C);
    MARATHON_ASSERT_SIZEOF(AckTrunkObject, 0x34);
}
