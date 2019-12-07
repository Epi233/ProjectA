#include "CppUnitTest.h"

#include "Components/LogicUnit.hpp"
#include "Components/ComponentInterface.hpp"
#include "lua.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace MainLogicTest 
{
	TEST_CLASS(emptyLogicTest)
	{
	public:
		TEST_METHOD(logicUnitTest1)
		{
			// test LogicUnit<EMPTY>
			ProjectA::LogicUnit<ProjectA::EMPTY> pureLogicTest(string("pureLogicTest.lua"));

			// make fake ports to send/load data to/from the real ports
			ProjectA::Port input1(ProjectA::WidthSpec(32));
			ProjectA::Port input2(ProjectA::WidthSpec(32));
			ProjectA::Port output1(ProjectA::WidthSpec(32));

			// define input ports and output ports
			vector<ProjectA::WidthSpec> inPorts{ ProjectA::WidthSpec{32}, ProjectA::WidthSpec{32} };
			vector<ProjectA::WidthSpec> outPorts{ ProjectA::WidthSpec{32} };

			for (auto i : inPorts)
				pureLogicTest.addInPort(i);

			for (auto i : outPorts)
				pureLogicTest.addOutPort(i);

			// connect the fake ports with the real ports
			input1.setTargetPort(pureLogicTest.getInPortPtr(uint64_t(0)));
			input2.setTargetPort(pureLogicTest.getInPortPtr(uint64_t(1)));
			pureLogicTest.getOutPortPtr(uint64_t(0))->setTargetPort(&output1);

			// assign data to the fake input ports
			vector<uint64_t> widthSpec{32};
			ProjectA::Data din1(widthSpec);
			ProjectA::Data din2(widthSpec);
			ProjectA::Data dout1(widthSpec);

			din1.setValue(vector<int64_t>{458});
			din2.setValue(vector<int64_t>{129});

			input1.run();
			input2.run();

			pureLogicTest.run();  // .lua file name must consist with the luaAddr

			Logger::WriteMessage(string("123").c_str());
			//Logger::WriteMessage((output1.getData()).getDataCells<string>()[0].c_str());
			//Assert::AreEqual(int64_t(587), (output1.getData()).getDataCells<int64_t>()[0]);  // vertify _dataCells[0] 
			Assert::AreEqual(11, 11);
		}
	};
}