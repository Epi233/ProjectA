#pragma once

#include "Data.hpp"
#include "Port.hpp"
#include "Components.hpp"

#include "lua.hpp"
#include "LuaBridge.h"
#include "Vector.h"
#include "../Util/Util.hpp"

namespace ProjectA
{
	// -------------------------------------------------------------------------------
	/*
	 * 实现模板特化用的
	 * 不做实现
	 */
	template <ComponentType Type>
	class LogicUnit;

	// -------------------------------------------------------------------------------
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

	// -------------------------------------------------------------------------------
	/*
	* EMPTY类型的LogicUnit
	*
	* 不外挂任何存储组件，单纯的逻辑
	* 输入Port不打拍，直接执行脚本逻辑
	* 输出Port不打拍
	*/
	// TODO Data类型变换功能
	template <>
	class LogicUnit<EMPTY> : public Logic, public NonCopyable, public NonMovable
	{
	public:
		explicit LogicUnit(const string& luaAddr)
			: Logic(luaAddr)
		{
			luaLoadLogicEmpty();
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

		void addInPort(bool isBuffered, const WidthSpec& widthSpec)
		{
			_inPorts.emplace_back(isBuffered, widthSpec);
		}

		void addOutPort(bool isBuffered, const WidthSpec& widthSpec)
		{
			_outPorts.emplace_back(isBuffered, widthSpec);
		}

	protected: /** for Lua */
		void luaLoadLogicEmpty()
		{
			luabridge::getGlobalNamespace(_luaState)
				.beginClass<LogicUnit<EMPTY>>("LogicUnitEmpty")
				.addFunction("getInput", &LogicUnit<EMPTY>::getInput)
				.addFunction("setOutput", &LogicUnit<EMPTY>::setOutput)
				.endClass();

			luabridge::push(_luaState, &*this);
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

	private:
		vector<Port> _inPorts;
		vector<Port> _outPorts;
	};

	// -------------------------------------------------------------------------------
	/*
	* 逻辑块的实现
	* LogicUnitBase为所有逻辑块的基类
	* 不同类型有不同派生，靠模板实现泛型，依次特化
	*
	* 行 2019.11.5
	*/
	/*
	 * 其他类型的基类
	 * 单输入输出
	 */

	class LogicBase : public Logic
	{
	public:
		explicit LogicBase(const string& luaAddr, const WidthSpec& widthSpec, bool isInBuffered, bool isOutBuffered)
			: Logic(luaAddr)
			, _inPort(isInBuffered, widthSpec)
			, _outPort(isOutBuffered, widthSpec)
		{
			luaInit();
		}

		virtual ~LogicBase()
		{
			lua_close(_luaState);
		}

	public:

		void run() override = 0;

	protected:

		void runInPorts()
		{
			_inPort.run();
		}

		void runOutPorts()
		{
			_outPort.run();
		}

		void luaInit()
		{
			_luaState = luaL_newstate();
			luaL_openlibs(_luaState);
			luaLoadLogicBase();
		}

	protected: /** for Lua */
		void luaLoadLogicBase() const
		{
			luabridge::getGlobalNamespace(_luaState)
				.beginClass<LogicBase>("LogicBase")
				.addFunction("getInput", &LogicBase::getInput)
				.addFunction("setOutput", &LogicBase::setOutput)
				.endClass();
		}

		vector<uint64_t> getInput(uint64_t index) const
		{
			return _inPort.getSendArea().getDataCellsUnit64();
		}

		void setOutput(uint64_t index, const vector<uint64_t>& data)
		{
			Data temp(_outPort.getSendArea()); // 拿Spec
			temp.setValue(data);
			_outPort.setPrepareArea(temp);
		}

	protected:
		Port _inPort;
		Port _outPort;
	};

	// -------------------------------------------------------------------------------
	/*
	 * MEM类型的LogicUnit
	 *
	 * 输入Port不打拍，直接执行脚本逻辑
	 * 输出Port打拍
	 *
	 */
	template<>
	class LogicUnit<MEM> : public LogicBase, public NonCopyable, public NonMovable
	{
	public:
		LogicUnit(const string& luaAddr, size_t memSize, WidthSpec widthSpec, bool isInBuffered, bool isOutBuffered)
			: LogicBase(luaAddr, widthSpec, isInBuffered, isOutBuffered)
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

	protected: /** for Lua */
		void luaLoadLogicUnitMem() const
		{
			luabridge::getGlobalNamespace(_luaState)
				.deriveClass<LogicUnit<MEM>, LogicBase>("LogicMem")
				.endClass();

			luabridge::push(_luaState, &*this);
			lua_setglobal(_luaState, "self");
		}

	private:
		Component<MEM>* _mem;
	};
}
