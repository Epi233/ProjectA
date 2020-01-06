#include "CppUnitTest.h"

#include "Components/LogicUnit.hpp"
#include "Components/ComponentInterface.hpp"
#include "UserFunctions/UserFunctions.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace MainLogicTest 
{
	using namespace ProjectA;
	
	TEST_CLASS(LogicUnitTest)
	{
	public:
		TEST_METHOD(pureLogicTest)
		{
			// User Functions
			auto testFun = [](vector<Data> input) -> vector<Data>
			{
				vector<int64_t> num1 = input[0].getDataCells<int64_t>();
				vector<int64_t> num2 = input[1].getDataCells<int64_t>();
				vector<int64_t> result{ num1[0] + num2[0] };

				vector<Data> resultData;
				resultData.emplace_back(vector<uint64_t>{32}, result);
				return resultData;
			};
			
			// test LogicUnit<EMPTY>
			vector<WidthSpec> inPortsSpec{ WidthSpec{ 32 }, WidthSpec{ 32 } };
			vector<WidthSpec> outPortsSpec{ WidthSpec{ 32 } };
			LogicUnit<PURE_LOGIC> pureLogicTest(inPortsSpec, outPortsSpec, uint64_t(0), testFun);

			// make fake ports to send/load data to/from the real ports
			Port input1(WidthSpec{ 32 });
			Port input2(WidthSpec{ 32 });
			Port output1(WidthSpec{ 32 });

			// connect the fake ports with the real ports
			input1.setTargetPort(pureLogicTest.getInPortPtr(uint64_t(0)));
			input2.setTargetPort(pureLogicTest.getInPortPtr(uint64_t(1)));
			pureLogicTest.getOutPortPtr(uint64_t(0))->setTargetPort(&output1);

			// assign data to the fake input ports
			vector<uint64_t> widthspec = { 32 };
			Data din1(widthspec);
			Data din2(widthspec);
			Data dout1(widthspec);

			din1.setValue(vector<int64_t>{ 20 });
			din2.setValue(vector<int64_t>{ 12 });

			input1.setData(din1);
			input2.setData(din2);

			input1.run();
			input2.run();

			pureLogicTest.run();

			Logger::WriteMessage((output1.getData()).getDataString<int64_t>().c_str());

			Assert::AreEqual(int64_t(32), (output1.getData()).getDataCells<int64_t>()[0]);  // vertify _dataCells[0] 
		}
	};
	
}