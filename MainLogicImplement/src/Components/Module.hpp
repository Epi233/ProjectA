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
		string _moduleName;

		vector<Port<IN>> _inPorts;
		vector<Port<OUT>> _outPorts;
	};


}