#pragma once

#include <Marathon.inl>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <lua50/lua.h>
#include <Sonicteam/LuaTableAccess.h>

namespace Sonicteam
{
    class LuaNode
    {
    public:
        struct Vftable
        {
            MARATHON_INSERT_PADDING(0x30);
            be<uint32_t> fpIsNumber;
            be<uint32_t> fpGetInteger;
            be<uint32_t> fpSetInteger;
            be<uint32_t> fpGetReal;
            be<uint32_t> fpSetReal;
            be<uint32_t> fpGetString;
            be<uint32_t> fpSetString;
            be<uint32_t> fpIsBoolean;
            be<uint32_t> fpGetBoolean;
            be<uint32_t> fpSetBoolean;
            be<uint32_t> fpUnregister;
            be<uint32_t> fpRegisterFunctions; // int (__fastcall *RegisterFunctions)(Sonicteam::LuaNode* this, luaL_reg* in_pFunctions, int in_count)
            be<uint32_t> fpGetTable;
        };

        xpointer<Vftable> m_pVftable;
        xpointer<lua50::lua_State> m_pLuaState;

        bool IsNumber(const char* in_pName)
        {
            const auto name = guest_stack_var<stdx::string>(in_pName);

            return GuestToHostFunction<bool>(m_pVftable->fpIsNumber.get(), this, name->c_str());
        }

        int GetInteger(const char* in_pName)
        {
            const auto name = guest_stack_var<stdx::string>(in_pName);

            return GuestToHostFunction<double>(m_pVftable->fpGetInteger.get(), this, name->c_str());
        }

        void SetInteger(const char* in_pName, int in_value)
        {
            const auto name = guest_stack_var<stdx::string>(in_pName);

            GuestToHostFunction<void>(m_pVftable->fpSetInteger.get(), this, name->c_str(), in_value);
        }

        double GetReal(const char* in_pName)
        {
            const auto name = guest_stack_var<stdx::string>(in_pName);

            return GuestToHostFunction<double>(m_pVftable->fpGetReal.get(), this, name->c_str());
        }

        void SetReal(const char* in_pName, double in_value)
        {
            const auto name = guest_stack_var<stdx::string>(in_pName);

            GuestToHostFunction<void>(m_pVftable->fpSetReal.get(), this, name->c_str(), in_value);
        }

        const char* GetString(const char* in_pName)
        {
            const auto name = guest_stack_var<stdx::string>(in_pName);

            return GuestToHostFunction<const char*>(m_pVftable->fpGetString.get(), this, name->c_str());
        }

        void SetString(const char* in_pName, const char* in_pValue)
        {
            const auto name = guest_stack_var<stdx::string>(in_pName);
            const auto value = guest_stack_var<stdx::string>(in_pValue);

            GuestToHostFunction<void>(m_pVftable->fpSetString.get(), this, name->c_str(), value->c_str());
        }

        bool IsBoolean(const char* in_pName)
        {
            const auto name = guest_stack_var<stdx::string>(in_pName);

            return GuestToHostFunction<bool>(m_pVftable->fpIsBoolean.get(), this, name->c_str());
        }

        bool GetBoolean(const char* in_pName)
        {
            const auto name = guest_stack_var<stdx::string>(in_pName);

            return GuestToHostFunction<bool>(m_pVftable->fpGetBoolean.get(), this, name->c_str());
        }

        void SetBoolean(const char* in_pName, bool in_value)
        {
            const auto name = guest_stack_var<stdx::string>(in_pName);

            GuestToHostFunction<void>(m_pVftable->fpSetBoolean.get(), this, name->c_str(), in_value);
        }

        void Unregister(const char* in_pName)
        {
            const auto name = guest_stack_var<stdx::string>(in_pName);

            GuestToHostFunction<void>(m_pVftable->fpUnregister.get(), this, name->c_str());
        }

        boost::shared_ptr<LuaTableAccess> GetTable(const char* in_pName)
        {
            const auto result = guest_stack_var<boost::shared_ptr<LuaTableAccess>>();
            const auto name = guest_stack_var<stdx::string>(in_pName);

            GuestToHostFunction<LuaTableAccess*>(m_pVftable->fpGetTable.get(), result.get(), this, name->c_str());

            return *result;
        }
    };

    MARATHON_ASSERT_OFFSETOF(LuaNode::Vftable, fpIsNumber, 0x30);
    MARATHON_ASSERT_OFFSETOF(LuaNode::Vftable, fpGetInteger, 0x34);
    MARATHON_ASSERT_OFFSETOF(LuaNode::Vftable, fpSetInteger, 0x38);
    MARATHON_ASSERT_OFFSETOF(LuaNode::Vftable, fpGetReal, 0x3C);
    MARATHON_ASSERT_OFFSETOF(LuaNode::Vftable, fpSetReal, 0x40);
    MARATHON_ASSERT_OFFSETOF(LuaNode::Vftable, fpGetString, 0x44);
    MARATHON_ASSERT_OFFSETOF(LuaNode::Vftable, fpSetString, 0x48);
    MARATHON_ASSERT_OFFSETOF(LuaNode::Vftable, fpIsBoolean, 0x4C);
    MARATHON_ASSERT_OFFSETOF(LuaNode::Vftable, fpGetBoolean, 0x50);
    MARATHON_ASSERT_OFFSETOF(LuaNode::Vftable, fpSetBoolean, 0x54);
    MARATHON_ASSERT_OFFSETOF(LuaNode::Vftable, fpUnregister, 0x58);
    MARATHON_ASSERT_OFFSETOF(LuaNode::Vftable, fpRegisterFunctions, 0x5C);
    MARATHON_ASSERT_OFFSETOF(LuaNode::Vftable, fpGetTable, 0x60);
    MARATHON_ASSERT_SIZEOF(LuaNode::Vftable, 0x64);

    MARATHON_ASSERT_OFFSETOF(LuaNode, m_pVftable, 0x00);
    MARATHON_ASSERT_OFFSETOF(LuaNode, m_pLuaState, 0x04);
    MARATHON_ASSERT_SIZEOF(LuaNode, 0x08);
}
