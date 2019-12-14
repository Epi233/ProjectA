#include "CppUnitTest.h"

#include "Components/LogicUnit.hpp"
#include "Components/ComponentInterface.hpp"
#include "ModuleTestResource/pureLogicTestScript.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace MainLogicTest 
{
	TEST_CLASS(emptyLogicTest)
	{
	public:
		TEST_METHOD(logicUnitTest1)
		{
			// test LogicUnit<EMPTY>
			vector<ProjectA::WidthSpec> inPortsSpec{ ProjectA::WidthSpec{ 32 }, ProjectA::WidthSpec{ 32 } };
			vector<ProjectA::WidthSpec> outPortsSpec{ ProjectA::WidthSpec{ 32 } };
			ProjectA::LogicUnit<ProjectA::PURE_LOGIC> pureLogicTest(inPortsSpec, outPortsSpec, uint64_t(0), ProjectA_test::pureLogicTestScript::logic);

			// make fake ports to send/load data to/from the real ports
			ProjectA::Port input1(ProjectA::WidthSpec{ 32 });
			ProjectA::Port input2(ProjectA::WidthSpec{ 32 });
			ProjectA::Port output1(ProjectA::WidthSpec{ 32 });

			// connect the fake ports with the real ports
			input1.setTargetPort(pureLogicTest.getInPortPtr(uint64_t(0)));
			input2.setTargetPort(pureLogicTest.getInPortPtr(uint64_t(1)));
			pureLogicTest.getOutPortPtr(uint64_t(0))->setTargetPort(&output1);

			// assign data to the fake input ports
			vector<uint64_t> widthspec = { 32 };
			//ProjectA::Data din1(ProjectA::WidthSpec{ 32 });
			//ProjectA::Data din2(ProjectA::WidthSpec{ 32 });
			//ProjectA::Data dout1(ProjectA::WidthSpec{ 32 });
			ProjectA::Data din1(widthspec);
			ProjectA::Data din2(widthspec);
			ProjectA::Data dout1(widthspec);

			din1.setValue(vector<int64_t>{ 20 });
			din2.setValue(vector<int64_t>{ 12 });

			input1.setData(din1);
			input2.setData(din2);

			//Logger::WriteMessage((input1.getData()).getDataString<int64_t>().c_str());

			//Logger::WriteMessage((input1.getData()).getDataString<int64_t>().c_str());
			//Logger::WriteMessage((input2.getData()).getDataString<int64_t>().c_str());

			input1.run();
			input2.run();

			//Logger::WriteMessage("get real input port value");
			//Logger::WriteMessage(pureLogicTest.getInPortPtr(uint64_t(0))->getData().getDataString<string>().c_str());
			//Logger::WriteMessage(pureLogicTest.getInPortPtr(uint64_t(1))->getData().getDataString<string>().c_str());

			pureLogicTest.run();

			Logger::WriteMessage((output1.getData()).getDataString<int64_t>().c_str());

			Assert::AreEqual(int64_t(32), (output1.getData()).getDataCells<int64_t>()[0]);  // vertify _dataCells[0] 
		}
	};
}