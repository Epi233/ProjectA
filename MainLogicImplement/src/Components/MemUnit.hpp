#pragma once

#include "Data.hpp"
#include "Port.hpp"
#include "CompoentInterface.hpp"

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

	
	template<>
	class Component<MEM> : public ComponentInterface
	{
	public:
		Component(uint64_t size, WidthSpec widthSpec)
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
		

	private:
		vector<Data> _memFile;
	};
	
}