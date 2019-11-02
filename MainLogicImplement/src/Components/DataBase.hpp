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
	class Database : public Singleton<Database>
	{
	public:
		struct DatabaseModule
		{
			unordered_map<string, Component<FIFO>*> fifoDatabase;
			unordered_map<string, Component<STACK>*> stackDatabase;
			unordered_map<string, Component<MEM>*> memDatabase;
		};

	public:
		~Database()
		{
			for (auto& everyDatabaseModuleItr : _database)
			{
				for (auto& itr : everyDatabaseModuleItr.second.fifoDatabase)
					delete itr.second;
				for (auto& itr : everyDatabaseModuleItr.second.stackDatabase)
					delete itr.second;
				for (auto& itr : everyDatabaseModuleItr.second.memDatabase)
					delete itr.second;
			}
		}
		
	public:
		// Mem 相关接口
		void insertComponentMem(const string& moduleName, const string& memName, uint64_t size, vector<WidthSpec> widthSpec)
		{
			_database[moduleName].memDatabase[memName] = new Component<MEM>(size, widthSpec);
		}

		DataPack readMem(const string& moduleName, const string& memName, uint64_t addr)
		{
			return _database[moduleName].memDatabase[memName]->readFile(addr);
		}

		void writeMem(const string& moduleName, const string& memName, uint64_t addr, DataPack& data)
		{
			_database[moduleName].memDatabase[memName]->writeFile(addr, data);
		}

	private:
		Database() = default;

	private:
		unordered_map<string, DatabaseModule> _database;
	};
	
}