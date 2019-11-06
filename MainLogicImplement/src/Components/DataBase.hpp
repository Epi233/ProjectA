/*
 * Database每个Module一个
 * 用来存这个Module所有的块
 * 每个类型的块可以又多个，靠不同名称索引
 * 因此每个块的逻辑脚本都可以访问同Module下所有的块内容
 * 同时这个Database也提供LuaBridge用的接口
 *
 * 行 2019.11.5
 */
#pragma once

#include "../Util/Util.hpp"
#include "../Util/Exception.hpp"
#include "Data.hpp"
#include "Port.hpp"
#include "MemUnit.hpp"
#include "CompoentInterface.hpp"
#include "LuaBridge.h"
#include "Vector.h"
#include "Data.hpp"

namespace ProjectA
{
	class Database
	{
	public:
		Database() = default;
		
		~Database()
		{
			// 所有指针由LogicUnit申请，由LogicUnit释放
			// 这里不需要做任何事情
		}
		
	public:
		// Mem 相关接口
		void insertComponentMem(const string& memName, Component<MEM>* ptr)
		{
			_memDatabase[memName] = ptr;
		}

	public: // Lua接口
		void luaLoadDatabaseFunctions(lua_State* luaState)
		{
			luabridge::getGlobalNamespace(luaState)
				.beginClass<Database>("Database")
				.addFunction("readMem", &readMem)
				.addFunction("writeMem", &writeMem)
				.addFunction("setOutput", &setOutput)
				.endClass();
		}
		
		/** MEM 相关接口 */
		vector<uint64_t> readMem(const string& memName, uint64_t addr)
		{
			const Data& data =  _memDatabase[memName]->readFile(addr);
			vector<uint64_t> result;
			for (auto& i : data.getDataCells())
				result.push_back(i.getData<uint64_t>());
			return result;
		}

		void writeMem(const string& memName, uint64_t addr, const vector<uint64_t>& data)
		{
			Data temp(_memDatabase[memName]->getWidthSpec());
			temp.setValue(data);
			_memDatabase[memName]->writeFile(addr, temp);
		}

		void setOutput(const string& memName, const vector<uint64_t>& data)
		{
			Data temp(_memDatabase[memName]->getWidthSpec());
			temp.setValue(data);
			_memDatabase[memName]->setSendArea(temp);
		}

		

	private:
		unordered_map<string, Component<FIFO>*> _fifoDatabase;
		unordered_map<string, Component<STACK>*> _stackDatabase;
		unordered_map<string, Component<MEM>*> _memDatabase;
	};
	
}