#include "Lua.h"

//-----------------------------------------------------------------------------
// スタックに積まれている型を表示
//-----------------------------------------------------------------------------
void Lua::ShowStack(lua_State* state)
{
    DebugLog("ShowStack---------------------------------------\n");

    // スタック数を取得
    const int num = lua_gettop(state);
    if (num == 0) {
        DebugLog("No stack.\n");
        DebugLog("------------------------------------------------\n");
        return;
    }

    for (int i = num; i >= 1; i--)
    {
        std::string str = std::to_string(i) + "(" + std::to_string(-num + i - 1) + ") ";
        DebugLog(str.c_str());

        int type = lua_type(state, i);
        switch (type)
        {
        case LUA_TNIL:
            DebugLog("NIL\n");
            break;
        case LUA_TBOOLEAN:
            DebugLog("BOOLEAN ");
            DebugLog(lua_toboolean(state, i) ? "true" : "false");
            DebugLog("\n");
            break;
        case LUA_TLIGHTUSERDATA:
            DebugLog("LIGHTUSERDATA\n");
            break;
        case LUA_TNUMBER:
            DebugLog("NUMBER ");
            DebugLog(std::to_string(lua_tonumber(state, i)).c_str());
            DebugLog("\n");
            break;
        case LUA_TSTRING:
            DebugLog("STRING ");
            str = lua_tostring(state, i);
            DebugLog(str.c_str());
            DebugLog("\n");
            break;
        case LUA_TTABLE:
            DebugLog("TABLE\n");
            break;
        case LUA_TFUNCTION:
            DebugLog("FUNCTION\n");
            break;
        case LUA_TUSERDATA:
            DebugLog("USERDATA\n");
            break;
        case LUA_TTHREAD:
            DebugLog("THREAD\n");
            break;
        }
    }

    DebugLog("------------------------------------------------\n");
}
