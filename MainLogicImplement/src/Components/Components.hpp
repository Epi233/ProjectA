#pragma once

#include "Data.hpp"
#include "Port.hpp"
#include "ComponentInterface.hpp"

namespace ProjectA
{

	template<>
	class Component<FIFO> : public ComponentInterface
	{
	public:


	private:

	};

	template<>
	class Component<STACK> : public ComponentInterface
	{
	public:


	private:

	};

	/*
	 * MEM组件
	 * 组件根据地址进行存储与读取，没有默认的读写逻辑
	 * 内部包含存储块_memFile和一个发送区_sendArea
	 * 这个发送区是给外包的LogicUnit读取输出用的
	 * Lua脚本中把MEM的输出写在发送区
	 * LogicUnit的outPort从发送去拿值
	 *
	 * 行 2019.11.6
	 */
	template<>
	class Component<MEM> : public ComponentInterface
	{
	public:
		Component(uint64_t size, WidthSpec widthSpec)
			: _widthSpec(widthSpec)
			, _sendArea(widthSpec)
		{
			Data temp(widthSpec);
			for (uint64_t i = 0; i < size; i++)
			{
				_memFile.push_back(temp);
			}
		}

		const Data& readFile(uint64_t addr) const
		{
			return _memFile[addr];
		}

		void writeFile(uint64_t addr, const Data& data)
		{
			_memFile[addr] = data;
		}

		Data getSendArea() const
		{
			return _sendArea;
		}

		void setSendArea(const Data& data)
		{
			_sendArea = data;
		}

		WidthSpec getWidthSpec() const
		{
			return _widthSpec;
		}

	private:
		WidthSpec _widthSpec;
		vector<Data> _memFile;
		Data _sendArea;
	};
	
}
