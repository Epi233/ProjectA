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

	public:
		
		virtual void run() = 0;

		lua_State* getLuaStatePtr() const
		{
			return _luaState;
		}

		void addInPort(bool isBuffered, const WidthSpec& widthSpec)
		{
			_inPorts.emplace_back(isBuffered, widthSpec);
		}

		void addOutPort(bool isBuffered, const WidthSpec& widthSpec)
		{
			_outPorts.emplace_back(isBuffered, widthSpec);
		}

	protected:
		
		void runInPorts()
		{
			for (auto& port : _inPorts)
				port.run();
		}

		void runOutPorts()
		{
			for (auto& port : _outPorts)
				port.run();
		}
		
		void luaInit()
		{
			_luaState = luaL_newstate();
			luaL_openlibs(_luaState);
			luaLoadSelf();
		}

	protected: /** for Lua */
		void luaLoadSelf()
		{
			luabridge::getGlobalNamespace(_luaState)
				.beginClass<LogicUnitBase>("Self")
				.addFunction("getInput", getInput)
				.addFunction("setOutput", &setOutput)
				.endClass();

			luabridge::push(_luaState, this);
			lua_setglobal(_luaState, "self");
		}

		vector<uint64_t> getInput(uint64_t index)
		{
			if (index >= _inPorts.size())
				throw exception("Input Index Out of Boundary");
			return _inPorts[index].getSendArea().getDataCellsUnit64();
		}

		void setOutput(uint64_t index, const vector<uint64_t>& data)
		{
			if (index >= _outPorts.size())
				throw exception("Input Index Out of Boundary");
			Data temp(_outPorts[index].getSendArea()); // 拿Spec
			temp.setValue(data);
			_outPorts[index].setPrepareArea(temp);
		}
		
	protected:
		string _luaScriptName;
		
		lua_State* _luaState;

		vector<Port> _inPorts;
		vector<Port> _outPorts;
	};


	/*
	 * 实现模板特化用的
	 * 不做实现
	 */
	template<ComponentType Type>
	class LogicUnit;

	/*
	 * EMPTY类型的LogicUnit
	 *
	 * 不外挂任何存储组件，单纯的逻辑
	 * 输入Port不打拍，直接执行脚本逻辑
	 * 输出Port不打拍
	 */
	template<>
	class LogicUnit<EMPTY> : public LogicUnitBase, public NonCopyable, public NonMovable
	{
	public:
		explicit LogicUnit(const string& luaAddr)
			: LogicUnitBase(luaAddr)
		{
		}
	public:
		void run() override
		{
			runInPorts();
			// 执行脚本
			luaL_dofile(_luaState, _luaScriptName.c_str());
			// Lua中应该已经setOutput函数把输出放在了outPort的准备区
			runOutPorts();
		}
	};

	/*
	 * MEM类型的LogicUnit
	 *
	 * 输入Port不打拍，直接执行脚本逻辑
	 * 输出Port打拍
	 *
	 */
	template<>
	class LogicUnit<MEM> : public LogicUnitBase, public NonCopyable, public NonMovable
	{
	public:
		LogicUnit(const string& luaAddr, size_t memSize, WidthSpec widthSpec)
			: LogicUnitBase(luaAddr)
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
			runInPorts();
			// 执行脚本
			luaL_dofile(_luaState, _luaScriptName.c_str());
			// Lua中应该已经setOutput函数把输出放在了outPort的准备区
			runOutPorts();
		}

		Component<MEM>* getPtr() const
		{
			return _mem;
		}

	private:
		Component<MEM>* _mem;
		
	};
}
