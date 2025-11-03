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
        struct Vftable : public LuaNode::Vftable
        {
            be<uint32_t> fpGetTableString; // 4
            be<uint32_t> fpGetTableConstString; // 8
            be<uint32_t> fpGetTableConstStringFast; // C
            be<uint32_t> fpGetRoot; // 10
            be<uint32_t> fpGetEmptyNode; // 14
            be<uint32_t> fpIsTableExist; // 18
            be<uint32_t> fpIsRoot; //1C
            be<uint32_t> fpSetTablePath; //20
            be<uint32_t> fpCallFunction; // (path, custom_data, funcName) 24
            be<uint32_t> fpCallFunction2; // (path, custom_data, funcName) 28
            be<uint32_t> fpCallFunction3; // (funcName, custom_data) 2c
            be<uint32_t> fpIsNumber; // (path) //30
            be<uint32_t> fpGetNumber; // (path) // 34
            be<uint32_t> fpSetNumber; // (path, number) // 38
            be<uint32_t> fpGetNumberFloat; // (path) //3C
            be<uint32_t> fpSetNumberFloat; // (path, number) //40
            be<uint32_t> fpGetString; // (path) // 44
            be<uint32_t> fpSetString; // (path, string) //48
            be<uint32_t> fpIsBoolean; // (path) +- //4C
            be<uint32_t> fpGetBoolean; // (path)  //50
            be<uint32_t> fpSetBoolean; // (path)  //54
            be<uint32_t> fpSetNil; // (path)  //54
            be<uint32_t> fpRegisterFunctions; // (LuaTFunc array, count)  //58
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
}
