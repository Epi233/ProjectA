#pragma once
#include "Data.hpp"

namespace ProjectA
{
	enum class ComponentType : char
	{
		fifo,
		stack,
		memoryFile
		
	};

#define FIFO ComponentType::fifo
#define STACK ComponentType::stack
#define MEM ComponentType::memoryFile

	Interface ComponentInterface
	{
	public:
		virtual  ~ComponentInterface() = default;
	};

	template<ComponentType Type>
	class Component : public ComponentInterface
	{
	};

}