/*
Development Log:

--19.12.14 Chen Yin

1.继承框架
	Logic: 
		所有LogicUnit的基类，其实可以和LogicBase合并，保留的目的仅是留作日后进行脚本的初始化（当前版本没有脚本）
		定义输入输出端口

	LogicBase ：Logic 
		所有LogicUnit的基类
		定义cycleBuffer，及cycleBuffer接口函数

	LogicUnit<PURE_LOGIC> : LogicBase
		用来实现纯逻辑，不带有memory
		用户可以用这个类实现控制逻辑

2、开发任务：
	完成LogicUnit<MEM>
	实现所有Logic动态配置，即运行中配置可切换
	实现BP

*/

#pragma once

#include "Data.hpp"
#include "Port.hpp"
#include "Components.hpp"
#include "CycleBuffer.hpp"

#include "lua.hpp"
#include "LuaBridge.h"
#include "Vector.h"
#include "../Util/Util.hpp"

#include <functional>

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
	 */
	class Logic
	{
	public:
		explicit Logic(vector<WidthSpec> inPortsSpec, vector<WidthSpec> outPortsSpec)
		{
			// create each inPort according to the inPortsSpec
			for (auto i : inPortsSpec)
			{
				_inPorts.emplace_back(i);
			}

			// create each outPort according to the outPortsSpec
			for (auto i : outPortsSpec)
			{
				_outPorts.emplace_back(i);
			}

			// create outPorts widthSpec for script
			for (auto i : outPortsSpec)
			{
				scriptOutPortsSpec.emplace_back(i);
			}
		}


		virtual void run() = 0;   //Pure virtual function; Can't be instantiated, only be inherited.

		Port* getInPortPtr(uint64_t index)
		{
			return &_inPorts[index];
		}

		Port* getOutPortPtr(uint64_t index)
		{
			return &_outPorts[index];
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

	protected:
		vector<Port> _inPorts;
		vector<Port> _outPorts;
		vector<WidthSpec> scriptOutPortsSpec;  // send outPorts widthSpec to script, due to the script output data's widthSpec must match with the widthSpec of outPorts;
	};


	/*
	* -------------------------------------------------------------------------------
	* 逻辑块的实现
	* LogicUnitBase为所有逻辑块的基类
	* 不同类型有不同派生，靠模板实现泛型，依次特化
	*
	*  -- 行 2019.11.5
	* -------------------------------------------------------------------------------
	*/

	
	/*
	 * 所有类型的基类
	 * 
	 */

	class LogicBase : public Logic
	{
	public:
		explicit LogicBase(vector<WidthSpec> inPortsSpec, vector<WidthSpec> outPortsSpec, uint64_t cycleCount)
			: Logic(inPortsSpec, outPortsSpec)
			//, _cycleBuffer(cycleCount, widthSpec)
		{		
			// create cycleBuffer for each outPort
			_cycleBuffer.resize(outPortsSpec.size());  // cycleBuffer number equal to outPorts number

			for (size_t i = 0; i < outPortsSpec.size(); ++i)
			{
				_cycleBuffer.clear();
				_cycleBuffer.push_back(CycleBuffer(cycleCount, outPortsSpec[i]));
			}
		}

	public:

		void run() override = 0;

	protected:
		// send script's output data to cycleBuffer's prepareArea
		void setCycleBuffer(const vector<vector<int64_t>>& data)  // cycleBuffer number equals to outPorts number 
		{
			DEBUG_ASSERT(data.size() == _outPorts.size());
			for (size_t i = 0; i < data.size(); ++i)    // 还需测试当outPorts有多个时，temp变量是否能有效setValue！！！
			{
				Data temp(_outPorts[i].getWidthSpec());
				temp.setValue(data[i]);
				_cycleBuffer[i].setPrepareData(temp);
			}
		}

		// send cycleBuffers' sendArea to outPorts
		void setOutPorts()
		{
			for (size_t i = 0; i < _outPorts.size(); ++i)
			{
				_outPorts[i].setData(_cycleBuffer[i].getSendArea());
			}
		}

	protected:
		vector<CycleBuffer> _cycleBuffer;
	};


	// -------------------------------------------------------------------------------
	/*
	* PURE_LOGIC类型的LogicUnit
	*
	* 不带有任何存储组件的纯逻辑
	*/
	// TODO Data类型变换功能
	template <>
	class LogicUnit<PURE_LOGIC> : public LogicBase, public NonCopyable, public NonMovable
	{
	public:
		explicit LogicUnit(vector<WidthSpec> inPortsSpec, vector<WidthSpec> outPortsSpec, uint64_t cycleCount, function<vector<vector<int64_t>>(vector<vector<int64_t>>, vector<WidthSpec>)> func)
			: LogicBase(inPortsSpec, outPortsSpec, cycleCount)
			, scriptPureLogic(func)
		{
			sendScript.resize(_inPorts.size());
			receiveScript.resize(_outPorts.size());
		}

	public:
		void run() override
		{
			runInPorts();

			// 准备脚本输入数据
			setScriptInput();
			// 执行脚本
			receiveScript = scriptPureLogic(sendScript, scriptOutPortsSpec);
			// 将脚本输出数据发送到cycleBuffer的准备区
			setCycleBuffer(receiveScript);
			// 运行一下cycleBuffer，之后需要根据CLK自动run;
			for (auto &i : _cycleBuffer)
			{
				i.run();
			}

			// 将cycleBuffer的发送区数据发送到相应的outPort
			setOutPorts();

			runOutPorts();
		}


	protected:
		void setScriptInput()   // send inPorts data to script
		{
			DEBUG_ASSERT(sendScript.size() == _inPorts.size());  // sendScript size must match with inPorts number
			for (size_t i = 0; i < _inPorts.size(); ++i)
			{
				sendScript[i] = _inPorts[i].getData().getDataCells<int64_t>();
			}
		}

		void getScriptOutput()  // send script data to outPorts
		{
			DEBUG_ASSERT(receiveScript.size() == _outPorts.size());  // receiveScript size must match with outPorts number
			for (size_t i = 0; i < _outPorts.size(); ++i)
			{
				_outPorts[i].setData(Data(scriptOutPortsSpec[i], receiveScript[i]));
			}
		}

	private:
		vector<vector<int64_t>> sendScript;  // data send to the script
		vector<vector<int64_t>> receiveScript;  // data get from the script
		function<vector<vector<int64_t>>(vector<vector<int64_t>>, vector<WidthSpec>)> scriptPureLogic;  // parameter list{ input data; outPorts widthspec }; return output data;
	};


	//// -------------------------------------------------------------------------------
	///*
	// * MEM类型的LogicUnit
	// *
	// * 用CycleBuffer来计数拍子
	// *
	// */
	//template<>
	//class LogicUnit<MEM> : public LogicBase, public NonCopyable, public NonMovable
	//{
	//public:
	//	LogicUnit(const string& luaAddr, WidthSpec widthSpec, size_t memSize, uint64_t cycleCount)
	//		: LogicBase(luaAddr, widthSpec, cycleCount)
	//		, _mem(nullptr)
	//	{
	//		_mem = new Component<MEM>{ memSize, widthSpec };
	//	}

	//	~LogicUnit()
	//	{
	//		delete _mem;
	//	}

	//public:
	//	void run() override
	//	{
	//		// 执行脚本，脚本中通过注册的getInput函数拿到input的数据
	//		luaL_dofile(_luaState, _luaScriptName.c_str());
	//		// Lua中应该已经setOutput函数把输出放在了CycleBuffer的准备区
	//		_cycleBuffer.run();
	//		_outPort.setData(_cycleBuffer.getSendArea());
	//		// outPort把数据送到下一个input
	//		_outPort.run();
	//	}

	//	Component<MEM>* getPtr() const
	//	{
	//		return _mem;
	//	}

	//protected: /** for Lua */
	//	void luaLoadLogicUnitMem() const
	//	{
	//		luabridge::getGlobalNamespace(_luaState)
	//			.deriveClass<LogicUnit<MEM>, LogicBase>("LogicMem")
	//			.endClass();

	//		luabridge::push(_luaState, &*this);
	//		lua_setglobal(_luaState, "self");
	//	}

	//private:
	//	Component<MEM>* _mem;
	//};
}
