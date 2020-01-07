#pragma once
#include "../Define/Define.hpp"

namespace ProjectA
{
	enum class ComponentType : char
	{
		pureLogic,
		fifo,
		stack,
		memoryFile
		
	};

#define PURE_LOGIC ComponentType::pureLogic
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
