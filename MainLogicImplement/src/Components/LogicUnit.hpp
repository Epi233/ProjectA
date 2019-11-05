/*
 * 逻辑块的实现
 * LogicUnitBase为所有逻辑块的基类
 * 不同类型有不同派生，靠模板实现泛型，依次特化
 *
 * 行 2019.11.5
 */
#pragma once

#include "Data.hpp"
#include "CompoentInterface.hpp"
#include "lua.hpp"
#include "Port.hpp"
#include "MemUnit.hpp"
#include "../Util/Util.hpp"
#include "../Util/Exception.hpp"
#include "LuaBridge.h"
#include "Vector.h"

namespace ProjectA
{
	/*
	 * LogicUnitBase
	 * 所有LogicUnit的基类
	 * 主要处理Lua接口的初始化
	 */
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


	/*
	 * 实现模板特化用的
	 * 不做实现
	 */
	template<ComponentType Type>
	class LogicUnit;

	/*
	 * MEM类型的LogicUnit
	 *
	 * 输入Port不打拍，直接执行脚本逻辑
	 * 输出Port打拍
	 *
	 * 
	 */
	template<>
	class LogicUnit<MEM> : public LogicUnitBase, public NonCopyable, public NonMovable
	{
	public:
		LogicUnit(const string& luaAddr, size_t memSize, WidthSpec widthSpec)
			: LogicUnitBase(luaAddr)
			, inPort(false, widthSpec)
			, outPort(true, widthSpec)
		{
			_mem = new Component<MEM>{ memSize, widthSpec };
		}

		~LogicUnit()
		{
			delete _mem;
		}

	public:
		void run()
		{
			// inPort不挂target，需要直接取发送区
			inPort.run();
			Data inputData = inPort.getSendArea();
			luabridge::push(_luaState, inputData);
			lua_setglobal(_luaState, "database");
			// 执行脚本
			luaDoFile();
			// TODO 这里的逻辑不是很自然 怎么把MEM部分的输入输出放在同一侧？？
		}

	private:
		Port inPort;
		Port outPort;
		Component<MEM>* _mem;
		
	};
}
