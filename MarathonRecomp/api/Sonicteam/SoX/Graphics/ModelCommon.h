#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Graphics/Model.h>
#include <stdx/map.h>
#include <stdx/string.h>


namespace Sonicteam::SoX::Graphics
{
    class ModelCommon : public Model
    {
    public:
        xpointer<void> m_Model; // (raw data), used in real-time
        stdx::map<stdx::string, be<uint32_t>> m_BoneIndices; // ??
        stdx::map<stdx::string, be<uint32_t>> m_MaterialIndices;
    };

    MARATHON_ASSERT_OFFSETOF(ModelCommon, m_Model, 0x64);
    MARATHON_ASSERT_OFFSETOF(ModelCommon, m_BoneIndices, 0x68);
    MARATHON_ASSERT_OFFSETOF(ModelCommon, m_MaterialIndices, 0x74);
    MARATHON_ASSERT_SIZEOF(ModelCommon, 0x80);
}
