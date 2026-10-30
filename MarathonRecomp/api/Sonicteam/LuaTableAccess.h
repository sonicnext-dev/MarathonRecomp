#pragma once

#include <Marathon.inl>

namespace Sonicteam
{
    class LuaTableAccess
    {
    public:
        struct Vftable
        {
            be<uint32_t> fpDtor;
            be<uint32_t> fpIterate;
            be<uint32_t> fpGetElement;
            MARATHON_INSERT_PADDING(0x1C);
            be<uint32_t> fpIsTable;
        };

        xpointer<Vftable> m_pVftable;

        bool Iterate()
        {
            return GuestToHostFunction<bool>(m_pVftable->fpIterate, this);
        }

        const char* GetElement(const char* in_pName)
        {
            const auto name = guest_stack_var<stdx::string>(in_pName);

            return GuestToHostFunction<const char*>(m_pVftable->fpGetElement, this, name->c_str());
        }

        bool IsTable()
        {
            return GuestToHostFunction<bool>(m_pVftable->fpIsTable, this);
        }
    };

    MARATHON_ASSERT_OFFSETOF(LuaTableAccess::Vftable, fpDtor, 0x00);
    MARATHON_ASSERT_OFFSETOF(LuaTableAccess::Vftable, fpIterate, 0x04);
    MARATHON_ASSERT_OFFSETOF(LuaTableAccess::Vftable, fpIsTable, 0x28);
    MARATHON_ASSERT_SIZEOF(LuaTableAccess::Vftable, 0x2C);

    MARATHON_ASSERT_OFFSETOF(LuaTableAccess, m_pVftable, 0x00);
    MARATHON_ASSERT_SIZEOF(LuaTableAccess, 0x04);
}
