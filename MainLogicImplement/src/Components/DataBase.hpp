#pragma once

#include "../Util/Util.hpp"
#include "../Util/Exception.hpp"
#include "DataPack.hpp"
#include "Port.hpp"
#include "MemUnit.hpp"
#include "CompoentInterface.hpp"
#include "LuaBridge.h"
#include "Vector.h"
#include <unordered_map>
#include "Data.hpp"

using std::unordered_map;

namespace ProjectA
{
	class Database
	{
	public:
		Database() = default;
		
		~Database()
		{
			for (auto& itr : _fifoDatabase)
				delete itr.second;
			for (auto& itr : _stackDatabase)
				delete itr.second;
			for (auto& itr : _memDatabase)
				delete itr.second;
		}
		
	public:
		// Mem 相关接口
		void insertComponentMem(const string& moduleName, const string& memName, uint64_t size, WidthSpec widthSpec)
		{
			_memDatabase[memName] = new Component<MEM>(size, widthSpec);
		}

	public: // Lua接口
		void luaLoadDatabaseFunctions(lua_State* luaState)
		{
			luabridge::getGlobalNamespace(luaState)
				.beginClass<Database>("Database")
				.addFunction("readMem", &readMem)
				.addFunction("writeMem", &writeMem)
				.endClass();
		}
		
		// Mem
		vector<uint64_t> readMem(const string& memName, uint64_t addr)
		{
			const DataPack& dataPack =  _memDatabase[memName]->readFile(addr);
			vector<uint64_t> result;
			for (auto& i : dataPack)
				result.push_back(i.getData<uint64_t>());
			return result;
		}

		void writeMem(const string& memName, uint64_t addr, const DataPack& data)
		{
			_memDatabase[memName]->writeFile(addr, data);
		}

	private:
		unordered_map<string, Component<FIFO>*> _fifoDatabase;
		unordered_map<string, Component<STACK>*> _stackDatabase;
		unordered_map<string, Component<MEM>*> _memDatabase;
	};
	
}