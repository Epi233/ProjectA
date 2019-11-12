#pragma once


#include "LogicUnitBase.hpp"
#include "../MemUnit.hpp"

namespace ProjectA
{

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
		LogicUnit(const string& luaAddr, size_t memSize, WidthSpec widthSpec)
			: LogicBase(luaAddr)
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