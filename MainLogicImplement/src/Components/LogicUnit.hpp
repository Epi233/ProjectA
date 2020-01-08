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

#include "DataBase.hpp"
#include "CycleBuffer.hpp"

#include "../Util/Util.hpp"


namespace ProjectA
{
	using ScriptFunction = function<vector<Data>(vector<Data>, Database*)>;
	
	class Logic
	{
	public:
		Logic() = delete;
		
		Logic(vector<WidthSpec> inPortsSpec
			, vector<WidthSpec> outPortsSpec
			, Database* databasePtr
			, uint64_t cycleCount
			, ScriptFunction func)
			: _dataBasePtr(databasePtr)
			, _scriptFunction(func)
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

			_cycleBuffer.reserve(outPortsSpec.size());  // cycleBuffer number equals to outPorts number
			for (size_t i = 0; i < outPortsSpec.size(); ++i)
			{
				_cycleBuffer.clear();
				_cycleBuffer.emplace_back(cycleCount, outPortsSpec[i]);
			}
			
		}

		void setScriptFunction(ScriptFunction f)
		{
			_scriptFunction = f;
		}

		void run()
		{
			runInPorts();

			// 准备脚本输入数据
			vector<Data> sendScript;  // data send to the script
			vector<Data> receiveScript;  // data get from the script
			for (auto& port : _inPorts)
				sendScript.emplace_back(port.getWidthSpec());
			for (auto& port : _outPorts)
				receiveScript.emplace_back(port.getWidthSpec());
			
			for (size_t i = 0; i < _inPorts.size(); ++i)
			{
				sendScript[i] = _inPorts[i].getData();
			}
			
			// 执行脚本
			receiveScript = _scriptFunction(sendScript, _dataBasePtr);
			
			// 将脚本输出数据发送到cycleBuffer的准备区
			setCycleBuffer(receiveScript);
			
			// 运行一下cycleBuffer，之后需要根据CLK自动run;
			for (auto& i : _cycleBuffer)
			{
				i.run();
			}

			// 将cycleBuffer的发送区数据发送到相应的outPort
			setOutPorts();

			runOutPorts();
		}

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
		vector<Port> _inPorts;
		vector<Port> _outPorts;
		Database* _dataBasePtr;
		vector<CycleBuffer> _cycleBuffer;
		ScriptFunction _scriptFunction;  // parameter list{ input data }; return output data;
	};

}
