#pragma once

#include <Marathon.h>
#include <stdx/type_info.h>

namespace stdx
{
    struct RTTIBaseClassDescriptor
    {
        stdx::type_info* typeDesc;
        size_t bcount;
        DWORD moffset;
        DWORD vftOffset;
        DWORD vftwOffset;
        DWORD attributes;
    };

    struct RTTIClassHierarchy 
    {
        be<uint32_t> signature;
        be<uint32_t> attributes;
        be<uint32_t> acount;
        xpointer<xpointer<RTTIBaseClassDescriptor*>> aclass;
    };

    struct RTTICompleteObjectLocator 
    {
        be<uint32_t> signature;
        be<uint32_t> offset;
        be<uint32_t> cdOffset;
        xpointer<stdx::type_info> typeDesc;
        xpointer<RTTIClassHierarchy>* hierarchyDesc;
    };

    // only works for vftable that have RTTI at (-4)
    inline static RTTICompleteObjectLocator* VftableToRTTI(void* pVftable)
    {
        return *(xpointer<RTTICompleteObjectLocator>*)((uintptr_t)pVftable - 4);
    }

    // only works for vftable that have RTTI at (-4)
    inline static std::string ClassToName(void* pClass)
    {
        auto vftable = ((xpointer<void>*)(pClass))->get();
        return VftableToRTTI(vftable)->typeDesc->name();
    }
};
