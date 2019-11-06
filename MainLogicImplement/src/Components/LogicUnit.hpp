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

		virtual void run() = 0;

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
			, _inPort(false, widthSpec)
			, _outPort(true, widthSpec)
			, _mem(nullptr)
		{
			_mem = new Component<MEM>{ memSize, widthSpec };
		}

		~LogicUnit()
		{
			delete _mem;
		}

	public:
		void run() override
		{
			// inPort不挂target，需要直接取发送区
			_inPort.run();
			Data inputData = _inPort.getSendArea();
			luabridge::push(_luaState, inputData.getDataCells());
			lua_setglobal(_luaState, "inputData");
			// 执行脚本
			luaDoFile();
			// 拿MEM的输出
			_outPort.setPrepareArea(_mem->getSendArea());
			_outPort.run();
		}

		Component<MEM>* getPtr() const
		{
			return _mem;
		}

	private:
		Port _inPort;
		Port _outPort;
		Component<MEM>* _mem;
		
	};
}
