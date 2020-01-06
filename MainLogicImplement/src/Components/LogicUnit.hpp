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
		Logic(vector<WidthSpec> inPortsSpec, vector<WidthSpec> outPortsSpec)
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
		LogicBase(vector<WidthSpec> inPortsSpec, vector<WidthSpec> outPortsSpec, uint64_t cycleCount)
			: Logic(inPortsSpec, outPortsSpec)
			//, _cycleBuffer(cycleCount, widthSpec)
		{		
			// create cycleBuffer for each outPort
			_cycleBuffer.reserve(outPortsSpec.size());  // cycleBuffer number equals to outPorts number

			for (size_t i = 0; i < outPortsSpec.size(); ++i)
			{
				_cycleBuffer.clear();
				_cycleBuffer.emplace_back(cycleCount, outPortsSpec[i]);
			}
		}

	public:

		void run() override = 0;

	protected:
		// send script's output data to cycleBuffer's prepareArea
		void setCycleBuffer(const vector<Data>& data)  // cycleBuffer number equals to outPorts number 
		{
			DEBUG_ASSERT(data.size() == _outPorts.size());
			for (size_t i = 0; i < data.size(); ++i)    // 还需测试当outPorts有多个时，temp变量是否能有效setValue！！！
			{
				_cycleBuffer[i].setPrepareData(data[i]);
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
		explicit LogicUnit(vector<WidthSpec> inPortsSpec, vector<WidthSpec> outPortsSpec, uint64_t cycleCount, function<vector<Data>(vector<Data>)> func)
			: LogicBase(inPortsSpec, outPortsSpec, cycleCount)
			, _scriptPureLogic(func)
		{
			for (auto& spec : inPortsSpec)
				_sendScript.emplace_back(spec);

			for (auto& spec : outPortsSpec)
				_receiveScript.emplace_back(spec);
		}

	public:
		void run() override
		{
			runInPorts();

			// 准备脚本输入数据
			setScriptInput();
			// 执行脚本
			_receiveScript = _scriptPureLogic(_sendScript);
			// 将脚本输出数据发送到cycleBuffer的准备区
			setCycleBuffer(_receiveScript);
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
			DEBUG_ASSERT(_sendScript.size() == _inPorts.size());  // sendScript size must match with inPorts number
			for (size_t i = 0; i < _inPorts.size(); ++i)
			{
				_sendScript[i] = _inPorts[i].getData();
			}
		}

	private:
		vector<Data> _sendScript;  // data send to the script
		vector<Data> _receiveScript;  // data get from the script
		function<vector<Data>(vector<Data>)> _scriptPureLogic;  // parameter list{ input data }; return output data;
	};

}
