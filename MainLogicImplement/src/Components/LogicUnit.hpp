#pragma once

#include "DataPack.hpp"
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

	protected:
		void luaInit()
		{
			_luaState = luaL_newstate();
			luaL_openlibs(_luaState);
			if (luaL_loadfile(_luaState, _luaScriptName.c_str()))
				throw LuaLoadError("Lua File Addr Error: " + _luaScriptName);
		}

		virtual void setLuaBridge() = 0;
		
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
		LogicUnit(const string& luaAddr, size_t memSize, vector<WidthSpec> widthSpec)
			: LogicUnitBase(luaAddr)
		{
			_mem = new Component<MEM>{ memSize, widthSpec };
		}

		~LogicUnit()
		{
			delete _mem;
		}

		void setLuaBridge() override
		{
			Component<MEM>& mem = *_mem;


			
		}

	private:
		vector<Port<IN>> _inPorts;
		vector<Port<OUT>> _outPorts;
		Component<MEM>* _mem;
		
	};
}
