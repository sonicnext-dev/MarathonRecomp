#pragma once

#include <Marathon.inl>
#include <hk330/hkpShape.h>
#include <hk330/hkpShapeContainer.h>

namespace hk330
{
    class hkpSingleShapeContainer : public hkpShapeContainer
    {
    public:
        xpointer<hkpShape> m_childShape;
    };
}
