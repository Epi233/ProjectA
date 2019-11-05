#pragma once

#include "LogicUnit.hpp"
#include "DataBase.hpp"
#include "Port.hpp"

namespace ProjectA
{
	class Module
	{
	public:


	private:
		void loadLua(LogicUnitBase* logicPtr)
		{
			lua_State* luaState = logicPtr->getLuaStatePtr();
			_database.luaLoadDatabaseFunctions(luaState);

			luabridge::push(luaState, &_database);
			lua_setglobal(luaState, "database");
		}

	private:
		string _moduleName;

		vector<LogicUnitBase*> _memLogicUnits;
		Database _database;
	};


}