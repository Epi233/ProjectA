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
		void createLogicUnitMem(const string& memName, const string& luaAddr, size_t memSize, WidthSpec widthSpec)
		{
			// 创建MEM逻辑组件
			LogicUnit<MEM>* ptr = new LogicUnit<MEM>(luaAddr, memSize, widthSpec);
			// Database更新新组件
			_database.insertComponentMem(memName, ptr->getPtr());
			// 逻辑组件加载Database函数
			_database.luaLoadDatabaseFunctions(ptr->getLuaStatePtr());
			// 向基类转换
			LogicUnitBase* base_ptr = dynamic_cast<LogicUnitBase*>(ptr);
			_memLogicUnits.push_back(base_ptr);
		}
	};


}