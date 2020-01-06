#pragma once

#include "../Components/Data.hpp"

namespace ProjectA
{

	class UserFunctions
	{
	public:
		UserFunctions()
		{
			_functionSet["TestFun"] = [](vector<Data> input) -> vector<Data>
			{
				return input;
			};
		}

		function<vector<Data>(vector<Data>)> operator[] (const string& funName)
		{
			return _functionSet[funName];
		}


	private:

		unordered_map<string, function<vector<Data>(vector<Data>)>> _functionSet;
	};


}