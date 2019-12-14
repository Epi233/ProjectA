#include "Components/LogicUnit.hpp"

namespace ProjectA_test
{
	class pureLogicTestScript
	{	
	public:
		static vector<vector<int64_t>> logic(vector<vector<int64_t>> inData, vector<ProjectA::WidthSpec> outPortsSpec)
		{
			// define outData
			vector<vector<int64_t>> outData(outPortsSpec.size());  // outData size must equal to outPorts size


			/******* User Logic Region *******/
			outData[0].push_back(inData[0][0] + inData[1][0]);  // inPort[0].dataCell[0] + inPort[1].dataCell[0]


			/*********************************/

			return outData;
		}
	};
}
