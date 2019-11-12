#pragma once

#include "../../Define/Define.hpp"
#include "../Data.hpp"
#include "lua.hpp"
#include "LuaBridge.h"
#include "../Components/ComponentInterface.hpp"

namespace ProjectA
{
	/*
	 * 实现模板特化用的
	 * 不做实现
	 */
	template<ComponentType Type>
	class LogicUnit;

	/*
	 * Logic
	 * 所有LogicUnit的基类
	 * 主要处理Lua接口的初始化
	 */
	class Logic
	{
	public:
		explicit Logic(const string& luaAddr)
			: _luaScriptName(luaAddr)
			, _luaState(nullptr)
		{
			luaInit();
		}

		virtual ~Logic()
		{
			lua_close(_luaState);
		}

	public:

		virtual void run() = 0;

		void luaInit()
		{
			_luaState = luaL_newstate();
			luaL_openlibs(_luaState);
		}

		lua_State* getLuaStatePtr() const
		{
			return _luaState;
		}

	protected:
		string _luaScriptName;
		lua_State* _luaState;
	};


}
