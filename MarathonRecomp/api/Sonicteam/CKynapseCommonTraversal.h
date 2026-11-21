#pragma once

#include <Marathon.inl>
#include <Kaim/CPropagationGraphTraversal.h>

namespace Sonicteam
{
    class CKynapseCommonTraversal : public Kaim::CPropagationGraphTraversal
    {
    public:
        MARATHON_INSERT_PADDING(0x8C);
    };

    MARATHON_ASSERT_SIZEOF(CKynapseCommonTraversal, 0xC8);
}
