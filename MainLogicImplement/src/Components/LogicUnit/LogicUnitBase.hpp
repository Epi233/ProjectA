/*
 * 逻辑块的实现
 * LogicUnitBase为所有逻辑块的基类
 * 不同类型有不同派生，靠模板实现泛型，依次特化
 *
 * 行 2019.11.5
 */
#pragma once

#include "../../Util/Exception.hpp"
#include "LogicUnitInterface.hpp"
#include "../../Util/Util.hpp"
#include "LuaBridge.h"
#include "Vector.h"
#include "../Port.hpp"

namespace ProjectA
{
	/*
	 * 其他类型的基类
	 * 单输入输出
	 */
	
	class LogicBase : public Logic
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
				.beginClass<Logic>("Self")
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
		Port _inPort;
		Port _outPort;
	};


	

	

	
}
