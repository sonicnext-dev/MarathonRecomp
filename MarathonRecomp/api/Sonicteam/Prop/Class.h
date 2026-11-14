#pragma once

#include <Marathon.inl>
#include <Sonicteam/Prop/AllocatedData.h>
#include <Sonicteam/SoX/RefCountObject.h>

namespace Sonicteam::Prop
{
    class Class : public SoX::RefCountObject
    {
    public:
        enum PropParamType : uint32_t
        {
            PropParamType_Boolean,
            PropParamType_Int32,
            PropParamType_Single,
            PropParamType_String,
            PropParamType_Vector3,
            PropParamType_Object = 6
        };

        struct PropParamInfo
        {
            char Name[16];
            be<PropParamType> Type;
            be<uint32_t> Flags;
        };

        struct PropInfo
        {
            xpointer<const char> pName;
            be<uint32_t> ParamCount;
            xpointer<PropParamInfo> pParams;
        };

        xpointer<PropInfo> m_pPropInfo;
        xpointer<AllocatedData> m_pAllocatedData;
    };

    MARATHON_ASSERT_OFFSETOF(Class::PropParamInfo, Name, 0x00);
    MARATHON_ASSERT_OFFSETOF(Class::PropParamInfo, Type, 0x10);
    MARATHON_ASSERT_OFFSETOF(Class::PropParamInfo, Flags, 0x14);
    MARATHON_ASSERT_SIZEOF(Class::PropParamInfo, 0x18);

    MARATHON_ASSERT_OFFSETOF(Class::PropInfo, pName, 0x00);
    MARATHON_ASSERT_OFFSETOF(Class::PropInfo, ParamCount, 0x04);
    MARATHON_ASSERT_OFFSETOF(Class::PropInfo, pParams, 0x08);
    MARATHON_ASSERT_SIZEOF(Class::PropInfo, 0x0C);

    MARATHON_ASSERT_OFFSETOF(Class, m_pPropInfo, 0x08);
    MARATHON_ASSERT_OFFSETOF(Class, m_pAllocatedData, 0x0C);
    MARATHON_ASSERT_SIZEOF(Class, 0x10);
}
