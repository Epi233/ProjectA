#pragma once

#include "DataPack.hpp"
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
		Component(uint64_t size, vector<WidthSpec> widthSpec)
		{
			_memFile.resize(size);
			for (uint64_t i = 0; i < size; i++)
			{
				_memFile[i] = DataPack{ widthSpec[i] };
			}
		}

		DataPack readFile(uint64_t addr) const
		{
			return _memFile[addr];
		}

		void writeFile(uint64_t addr, DataPack& data)
		{
			_memFile[addr] = data;
		}
		

	private:
		vector<DataPack> _memFile;
	};
	
}