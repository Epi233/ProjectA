#pragma once

#include "LogicUnit.hpp"
#include "Database.hpp"
#include "Port.hpp"

namespace ProjectA
{
	class Module
	{
	public:
		~Module()
		{
			for (auto ptr : _memLogicUnits)
				delete ptr;
		}

	private:
		string _moduleName;

		vector<LogicUnitBase*> _memLogicUnits;
		Database _database;

	private:
		void loadLua(LogicUnitBase* logicPtr)
		{
			lua_State* luaState = logicPtr->getLuaStatePtr();
			_database.luaLoadDatabaseFunctions(luaState);

			luabridge::push(luaState, &_database);
			lua_setglobal(luaState, "database");
		}
		
		void createLogicUnitMem(const string& memName, const string& luaAddr, size_t memSize, WidthSpec widthSpec)
		{
			LogicUnit<MEM>* ptr = new LogicUnit<MEM>(luaAddr, memSize, widthSpec);
			_database.insertComponentMem(memName, ptr->getPtr());
			LogicUnitBase* base_ptr = dynamic_cast<LogicUnitBase*>(ptr);
			loadLua(base_ptr);
			_memLogicUnits.push_back(base_ptr);
		}
	};


}