/*
* EMPTY类型的LogicUnit
*
* 不外挂任何存储组件，单纯的逻辑
* 输入Port不打拍，直接执行脚本逻辑
* 输出Port不打拍
*/

#pragma once

#include "LuaBridge.h"
#include "Vector.h"
#include "LogicUnitInterface.hpp"
#include "../../Util/Util.hpp"
#include "../Port.hpp"

namespace ProjectA
{
	
	 // TODO Data类型变换功能
	template<>
	class LogicUnit<EMPTY> : public Logic, public NonCopyable, public NonMovable
	{
	public:
		explicit LogicUnit(const string& luaAddr)
			: Logic(luaAddr)
		{
			luaLoadSelf();
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
		void luaLoadSelf()
		{
			luabridge::getGlobalNamespace(_luaState)
				.beginClass<LogicUnit<EMPTY>>("Self")
				.addFunction("getInput", &LogicUnit<EMPTY>::getInput)
				.addFunction("setOutput", &LogicUnit<EMPTY>::setOutput)
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

	private:
		vector<Port> _inPorts;
		vector<Port> _outPorts;
	};


}