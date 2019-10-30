#pragma once

#include "../Util/Util.hpp"
#include "../Util/Exception.hpp"
#include "DataPack.hpp"
#include "Port.hpp"
#include "MemUnit.hpp"
#include "CompoentInterface.hpp"
#include <unordered_map>

using std::unordered_map;

namespace ProjectA
{

	class DataBase
	{
	public:
		template<ComponentType Type>
		static void insertComponent(const string& str, Component<Type>* ptr)
		{
			if (Type == FIFO)
				_fifoDataBase[str] = ptr;
			else if (Type == STACK)
				_stackDataBase[str] = ptr;
			else if (Type == MEM)
				_memDataBase[str] = ptr;
			else
				throw EnumClassError("Database insertion enum class error");
		}

		// MemFile²Ù×÷½Ó¿Ú
		
		

	private:
		static unordered_map<string, Component<FIFO>*> _fifoDataBase;
		static unordered_map<string, Component<STACK>*> _stackDataBase;
		static unordered_map<string, Component<MEM>*> _memDataBase;
	};
	
}