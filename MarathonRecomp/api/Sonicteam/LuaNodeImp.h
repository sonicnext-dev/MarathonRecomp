#pragma once

#include <Marathon.inl>
#include <Sonicteam/LuaNode.h>
#include <stdx/string.h>
#include <boost/smart_ptr/shared_ptr.h>

namespace Sonicteam
{
    class LuaNodeImp : public LuaNode
    {
    public:
        struct Vftable : LuaNode::Vftable
        {
            be<uint32_t> fpGetTableString;
            be<uint32_t> fpGetTableConstString;
            be<uint32_t> fpGetTableConstStringFast;
            be<uint32_t> fpGetRoot;
            be<uint32_t> fpGetEmptyNode;
            be<uint32_t> fpIsTableExist;
            be<uint32_t> fpIsRoot;
            be<uint32_t> fpSetTablePath;
            be<uint32_t> fpCallFunction;
            be<uint32_t> fpCallFunction2;
            be<uint32_t> fpCallFunction3;
            be<uint32_t> fpIsNumber;
            be<uint32_t> fpGetNumber;
            be<uint32_t> fpSetNumber;
            be<uint32_t> fpGetNumberFloat;
            be<uint32_t> fpSetNumberFloat;
            be<uint32_t> fpGetString;
            be<uint32_t> fpSetString;
            be<uint32_t> fpIsBoolean;
            be<uint32_t> fpGetBoolean;
            be<uint32_t> fpSetBoolean;
            be<uint32_t> fpSetNil;
            be<uint32_t> fpRegisterFunctions;
            be<uint32_t> fpFunc60;
        };

        boost::shared_ptr<LuaNodeImp> GetTable(stdx::string* tablePath)
        {
            auto result = guest_stack_var<boost::shared_ptr<LuaNodeImp>>();
            auto vft = (Vftable*)m_pVftable.get();
            GuestToHostFunction<void>(vft->fpGetTableString,result.get(), this, tablePath);
            return *result.get();
        }

        void CallFunction3(const char* functionName, void* customData)
        {
            auto result = guest_stack_var<boost::shared_ptr<LuaNodeImp>>();
            auto vft = (Vftable*)m_pVftable.get();
            GuestToHostFunction<void>(vft->fpCallFunction3, this, functionName, customData);
        }

        void RegisterFunctions(lua50::luaL_reg* functions, uint32_t count)
        {
            auto result = guest_stack_var<boost::shared_ptr<LuaNodeImp>>();
            auto vft = (Vftable*)m_pVftable.get();
            GuestToHostFunction<void>(vft->fpRegisterFunctions, this, functions, count);
        }

        stdx::string m_LuaTable;
    };

    MARATHON_ASSERT_OFFSETOF(LuaNodeImp::Vftable, fpGetTableString, 0x4);
    MARATHON_ASSERT_OFFSETOF(LuaNodeImp::Vftable, fpGetTableConstString, 0x8);
    MARATHON_ASSERT_OFFSETOF(LuaNodeImp::Vftable, fpGetTableConstStringFast, 0xC);
    MARATHON_ASSERT_OFFSETOF(LuaNodeImp::Vftable, fpGetRoot, 0x10);
    MARATHON_ASSERT_OFFSETOF(LuaNodeImp::Vftable, fpGetEmptyNode, 0x14);
    MARATHON_ASSERT_OFFSETOF(LuaNodeImp::Vftable, fpIsTableExist, 0x18);
    MARATHON_ASSERT_OFFSETOF(LuaNodeImp::Vftable, fpIsRoot, 0x1C);
    MARATHON_ASSERT_OFFSETOF(LuaNodeImp::Vftable, fpSetTablePath, 0x20);
    MARATHON_ASSERT_OFFSETOF(LuaNodeImp::Vftable, fpCallFunction, 0x24);
    MARATHON_ASSERT_OFFSETOF(LuaNodeImp::Vftable, fpCallFunction2, 0x28);
    MARATHON_ASSERT_OFFSETOF(LuaNodeImp::Vftable, fpCallFunction3, 0x2C);
    MARATHON_ASSERT_OFFSETOF(LuaNodeImp::Vftable, fpIsNumber, 0x30);
    MARATHON_ASSERT_OFFSETOF(LuaNodeImp::Vftable, fpGetNumber, 0x34);
    MARATHON_ASSERT_OFFSETOF(LuaNodeImp::Vftable, fpSetNumber, 0x38);
    MARATHON_ASSERT_OFFSETOF(LuaNodeImp::Vftable, fpGetNumberFloat, 0x3C);
    MARATHON_ASSERT_OFFSETOF(LuaNodeImp::Vftable, fpSetNumberFloat, 0x40);
    MARATHON_ASSERT_OFFSETOF(LuaNodeImp::Vftable, fpGetString, 0x44);
    MARATHON_ASSERT_OFFSETOF(LuaNodeImp::Vftable, fpSetString, 0x48);
    MARATHON_ASSERT_OFFSETOF(LuaNodeImp::Vftable, fpIsBoolean, 0x4C);
    MARATHON_ASSERT_OFFSETOF(LuaNodeImp::Vftable, fpGetBoolean, 0x50);
    MARATHON_ASSERT_OFFSETOF(LuaNodeImp::Vftable, fpSetBoolean, 0x54);
    MARATHON_ASSERT_OFFSETOF(LuaNodeImp::Vftable, fpSetNil, 0x58);
    MARATHON_ASSERT_OFFSETOF(LuaNodeImp::Vftable, fpRegisterFunctions, 0x5C);
    MARATHON_ASSERT_OFFSETOF(LuaNodeImp::Vftable, fpFunc60, 0x60);
}
