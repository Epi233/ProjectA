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
#include "Components.hpp"
#include "DataTypeRepo.hpp"

namespace ProjectA
{
	
	class Database
	{
	public:
		Database() = default;
		
		~Database()
		{
			for (auto p : _fifoDatabase)
				delete p.second;
			for (auto p : _stackDatabase)
				delete p.second;
			for (auto p : _memDatabase)
				delete p.second;
		}
		
	public:
		// Mem 相关接口
		void addComponentMem(const string& memName, uint64_t size, WidthSpec widthSpec)
		{
			Component<MEM>* ptr = new Component<MEM>{ size, widthSpec };
			_memDatabase[memName] = ptr;
		}
		
		Data readMem(const string& memName, uint64_t addr)
		{
			return _memDatabase[memName]->readFile(addr);
		}

		void writeMem(const string& memName, uint64_t addr, const Data& data)
		{
			_memDatabase[memName]->writeFile(addr, data);
		}


	private:
		unordered_map<string, Component<FIFO>*> _fifoDatabase;
		unordered_map<string, Component<STACK>*> _stackDatabase;
		unordered_map<string, Component<MEM>*> _memDatabase;
	};
	
}