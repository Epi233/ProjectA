#pragma once
#include "Data.hpp"

namespace ProjectA
{
	enum class ComponentType : char
	{
		empty,
		fifo,
		stack,
		memoryFile
		
	};

#define EMPTY ComponentType::empty
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