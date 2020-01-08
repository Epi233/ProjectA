#pragma once

#include "../Components/DataBase.hpp"

namespace ProjectA
{
	using ScriptFunction = function<vector<Data>(vector<Data>, Database*)>;

	class UserFunctions
	{
	public:
		UserFunctions()
		{
			// module测试1用，别动
			_functionSet["TestFun1"] = [](vector<Data> input, Database* databasePtr) -> vector<Data>
			{
				return input;
			};
			
			// module测试2用，别动
			_functionSet["TestFun2"] = [](vector<Data> input, Database* databasePtr) -> vector<Data>
			{
				int64_t addr = input[0].getDataCells<uint64_t>()[0];
				vector<Data> result{ Data{WidthSpec{ 32 }} };

				if (addr < 50)
					databasePtr->writeMem("mem1", addr, input[1]);
				else
					result[0] = databasePtr->readMem("mem1", addr - 50);
				
				return result;
			};
			
		}

		ScriptFunction operator[] (const string& funName)
		{
			return _functionSet[funName];
		}


	private:

		unordered_map<string, ScriptFunction> _functionSet;
	};


}