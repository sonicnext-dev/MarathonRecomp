#include <Marathon.inl>

namespace lua50
{
    typedef xpointer<void> lua_State;
    typedef be<uint32_t> lua_CFunction;

    struct luaL_reg
    {
        xpointer<const char> name;
        lua_CFunction func;
    };

    inline extern const void* lua_topointer(lua_State* L, int idx)
    {
        return GuestToHostFunction<const void*>(sub_825D5800,L,idx);
    }
}
