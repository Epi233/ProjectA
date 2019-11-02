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
		~DataBase()
		{
			for (auto itr = _fifoDataBase.begin(); itr != _fifoDataBase.end(); ++itr)
				delete itr->second;
			for (auto itr = _stackDataBase.begin(); itr != _stackDataBase.end(); ++itr)
				delete itr->second;
			for (auto itr = _memDataBase.begin(); itr != _memDataBase.end(); ++itr)
				delete itr->second;
		}
		
	public:
		
		void insertComponentMem(const string& str, uint64_t size, vector<WidthSpec> widthSpec)
		{
			_memDataBase[str] = new Component<MEM>(size, widthSpec);
		}

		// MemFile²Ù×÷½Ó¿Ú

	private:
		unordered_map<string, Component<FIFO>*> _fifoDataBase;
		unordered_map<string, Component<STACK>*> _stackDataBase;
		unordered_map<string, Component<MEM>*> _memDataBase;
	};
	
}