#pragma once

#include "DataPack.hpp"
#include "Port.hpp"

namespace ProjectA
{
	enum class MemBehavior : char
	{
		Fifo,
		Stack,
		Memory
	};
	
	template <MemBehavior B>
	class MemUnit
	{
	};
	
#define FIFO MemBehavior::Fifo
#define STACK MemBehavior::Stack
#define MEMORY MemBehavior::Memory

	template<>
	class MemUnit<FIFO>
	{
		
	};

	template<>
	class MemUnit<STACK>
	{

	};

	template<>
	class MemUnit<MEMORY>
	{
	public:



	private:
		Port<IN> _inPort;
		Port<OUT> _outPort;
		vector<DataPack> _mem;


	};
	
}