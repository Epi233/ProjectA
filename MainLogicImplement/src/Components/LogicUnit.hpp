#pragma once

#include "Data.hpp"
#include "CompoentInterface.hpp"
#include "lua.hpp"
#include "Port.hpp"
#include "MemUnit.hpp"
#include "../Util/Util.hpp"
#include "../Util/Exception.hpp"
#include "LuaBridge.h"

namespace ProjectA
{

	class LogicUnitBase
	{
	public:
		explicit LogicUnitBase(const string& luaAddr)
			: _luaScriptName(luaAddr)
			, _luaState(nullptr)
		{
			luaInit();
		}

		virtual ~LogicUnitBase()
		{
			lua_close(_luaState);
		}

		void luaDoFile()
		{
			luaL_dofile(_luaState, _luaScriptName.c_str());
		}

		lua_State* getLuaStatePtr() const
		{
			return _luaState;
		}

	protected:
		void luaInit()
		{
			_luaState = luaL_newstate();
			luaL_openlibs(_luaState);
		}
		
	protected:
		string _luaScriptName;
		lua_State* _luaState;
	};
	
	template<ComponentType Type>
	class LogicUnit : public LogicUnitBase
	{
	};

	template<>
	class LogicUnit<MEM> : public LogicUnitBase, public NonCopyable, public NonMovable
	{
	public:
		LogicUnit(const string& luaAddr, size_t memSize, WidthSpec widthSpec)
			: LogicUnitBase(luaAddr)
		{
			_mem = new Component<MEM>{ memSize, widthSpec };
		}

		~LogicUnit()
		{
			delete _mem;
		}

	private:
		Component<MEM>* _mem;
		
	};
}
