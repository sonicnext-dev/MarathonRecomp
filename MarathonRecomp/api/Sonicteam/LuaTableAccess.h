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
            be<uint32_t> fpPopString;
            be<uint32_t> fpPeekString;
            be<uint32_t> fpPopReal;
            be<uint32_t> fpPeekReal;
            be<uint32_t> fpPopInteger;
            be<uint32_t> fpPeekInteger;
            be<uint32_t> fpPopTable;
            be<uint32_t> fpPeekTable;
            be<uint32_t> fpIsTable;
        };

        xpointer<Vftable> m_pVftable;

        bool Iterate()
        {
            return GuestToHostFunction<bool>(m_pVftable->fpIterate, this);
        }

        const char* PopString(const char* in_pName)
        {
            const auto name = guest_stack_var<stdx::string>(in_pName);

            return GuestToHostFunction<const char*>(m_pVftable->fpPopString, this, name->c_str());
        }

        const char* PeekString()
        {
            return GuestToHostFunction<const char*>(m_pVftable->fpPeekString, this);
        }

        double PopReal(const char* in_pName)
        {
            const auto name = guest_stack_var<stdx::string>(in_pName);

            return GuestToHostFunction<double>(m_pVftable->fpPopReal, this, name->c_str());
        }

        double PeekReal()
        {
            return GuestToHostFunction<double>(m_pVftable->fpPeekReal, this);
        }

        int PopInteger(const char* in_pName)
        {
            const auto name = guest_stack_var<stdx::string>(in_pName);

            return GuestToHostFunction<int>(m_pVftable->fpPopInteger, this, name->c_str());
        }

        int PeekInteger()
        {
            return GuestToHostFunction<int>(m_pVftable->fpPeekInteger, this);
        }

        boost::shared_ptr<LuaTableAccess> PopTable(const char* in_pName)
        {
            const auto result = guest_stack_var<boost::shared_ptr<LuaTableAccess>>();
            const auto name = guest_stack_var<stdx::string>(in_pName);

            GuestToHostFunction<LuaTableAccess*>(m_pVftable->fpPopTable, result.get(), this, name->c_str());

            return *result;
        }

        boost::shared_ptr<LuaTableAccess> PeekTable()
        {
            const auto result = guest_stack_var<boost::shared_ptr<LuaTableAccess>>();

            GuestToHostFunction<LuaTableAccess*>(m_pVftable->fpPeekTable, result.get(), this);

            return *result;
        }

        bool IsTable()
        {
            return GuestToHostFunction<bool>(m_pVftable->fpIsTable, this);
        }
    };

    MARATHON_ASSERT_OFFSETOF(LuaTableAccess::Vftable, fpDtor, 0x00);
    MARATHON_ASSERT_OFFSETOF(LuaTableAccess::Vftable, fpIterate, 0x04);
    MARATHON_ASSERT_OFFSETOF(LuaTableAccess::Vftable, fpPopString, 0x08);
    MARATHON_ASSERT_OFFSETOF(LuaTableAccess::Vftable, fpPeekString, 0x0C);
    MARATHON_ASSERT_OFFSETOF(LuaTableAccess::Vftable, fpPopReal, 0x10);
    MARATHON_ASSERT_OFFSETOF(LuaTableAccess::Vftable, fpPeekReal, 0x14);
    MARATHON_ASSERT_OFFSETOF(LuaTableAccess::Vftable, fpPopInteger, 0x18);
    MARATHON_ASSERT_OFFSETOF(LuaTableAccess::Vftable, fpPeekInteger, 0x1C);
    MARATHON_ASSERT_OFFSETOF(LuaTableAccess::Vftable, fpPopTable, 0x20);
    MARATHON_ASSERT_OFFSETOF(LuaTableAccess::Vftable, fpPeekTable, 0x24);
    MARATHON_ASSERT_OFFSETOF(LuaTableAccess::Vftable, fpIsTable, 0x28);
    MARATHON_ASSERT_SIZEOF(LuaTableAccess::Vftable, 0x2C);

    MARATHON_ASSERT_OFFSETOF(LuaTableAccess, m_pVftable, 0x00);
    MARATHON_ASSERT_SIZEOF(LuaTableAccess, 0x04);
}
