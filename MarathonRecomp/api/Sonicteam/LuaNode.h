#pragma once

#include <Marathon.inl>
#include <boost/smart_ptr/shared_ptr.h>
#include <lua50/lua.h>

namespace Sonicteam
{
    class LuaNode
    {
    public:
        struct Vftable
        {
            be<uint32_t> fpDestroy;
            be<uint32_t> fpGetTableString;
            be<uint32_t> fpGetTableConstString;
            be<uint32_t> fpGetTableConstStringFast;
            be<uint32_t> fpGetRoot;
            be<uint32_t> fpGetEmptyNode;
            be<uint32_t> fpIsTableExist;
            be<uint32_t> fpIsRoot;
            be<uint32_t> fpSetTablePath;
            be<uint32_t> fpCallFunction;      // (path, custom_data, funcName)
            be<uint32_t> fpCallFunction2;     // (path, custom_data, funcName)
            be<uint32_t> fpCallFunction3;     // (funcName, custom_data)
            be<uint32_t> fpIsNumber;          // (path) 
            be<uint32_t> fpGetNumber;         // (path)
            be<uint32_t> fpSetNumber;         // (path, number)
            be<uint32_t> fpGetNumberFloat;    // (path)
            be<uint32_t> fpSetNumberFloat;    // (path, number)
            be<uint32_t> fpGetString;         // (path)
            be<uint32_t> fpSetString;         // (path, string)
            be<uint32_t> fpIsBoolean;         // (path)
            be<uint32_t> fpGetBoolean;        // (path)
            be<uint32_t> fpSetBoolean;        // (path)
            be<uint32_t> fpSetNil;            // (path)
            be<uint32_t> fpRegisterFunctions; // (LuaTFunc array, count)
            be<uint32_t> fpFunc60;
        };

        xpointer<Vftable> m_pVftable;
        lua50::lua_State m_pLuaState;

        boost::shared_ptr<LuaNode> GetTable(stdx::string* tablePath)
        {
            auto result = guest_stack_var<boost::shared_ptr<LuaNode>>();

            GuestToHostFunction<void>(m_pVftable->fpGetTableString, result.get(), this, tablePath);

            return *result.get();
        }

        void CallFunction3(const char* functionName, void* customData)
        {
            GuestToHostFunction<void>(m_pVftable->fpCallFunction3, this, functionName, customData);
        }

        void RegisterFunctions(lua50::luaL_reg* functions, uint32_t count)
        {
            GuestToHostFunction<void>(m_pVftable->fpRegisterFunctions, this, functions, count);
        }
    };

    MARATHON_ASSERT_SIZEOF(LuaNode, 8);
}
