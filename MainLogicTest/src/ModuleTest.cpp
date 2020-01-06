#include "CppUnitTest.h"

#include "Components/Module.hpp"
#include "tinyxml2.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace tinyxml2;
using namespace ProjectA;

namespace MainLogicTest
{
	TEST_CLASS(ModuleTest)
	{
	public:
		TEST_METHOD(moduleTest1)
		{
			// xml初始化
			XMLDocument doc;
			doc.LoadFile("../MainLogicTest/src/ModuleTestResource/ModuleTestConfig.xml");
			XMLElement* xmlRoot = doc.RootElement();
			
			XMLElement* xmlDataTypeDefinition = xmlRoot->FirstChildElement("DataTypeDefinition");
			DataTypeRepo dataTypeRepo{};
			dataTypeRepo.generateFromXml(xmlDataTypeDefinition);

			XMLElement* xmlModule = xmlRoot->FirstChildElement("Module");
			Module pe(&dataTypeRepo);
			pe.generateFromXml(xmlModule);

			// 数据初始化
			vector<uint64_t> widthSpec{ 32, 1 };
			int64_t clk = 0;
			Data dataLeft(widthSpec);
			Data dataRight(widthSpec);

			auto generateDataLeft = [&]() -> ProjectA::Data
			{
				Data data(widthSpec);
				data.setValue(vector<int64_t>{ clk + 100, clk });
				return data;
			};

			auto generateDataRight = [&]() -> ProjectA::Data
			{
				Data data(widthSpec);
				data.setValue(vector<int64_t>{ clk + 100, clk + 1 });
				return data;
			};
			
			// 做假端口
			Port inPortLeft(widthSpec);
			Port inPortRight(widthSpec);
			Port outPortLeft(widthSpec);
			Port outPortRight(widthSpec);

			inPortLeft.setTargetPort(pe.getInPort(0));
			inPortRight.setTargetPort(pe.getInPort(1));
			pe.getOutPort(0)->setTargetPort(&outPortLeft);
			pe.getOutPort(1)->setTargetPort(&outPortRight);

			vector<string> inputLeftLog;
			vector<string> outputLeftLog;
			vector<string> inputRightLog;
			vector<string> outputRightLog;
			
			// 主循环
			while (clk++ <= 30)
			{
				// 构造数据放在假端口
				if (clk % 3 == 0)
				{
					dataLeft = generateDataLeft();
					dataRight = generateDataRight();
				}
				else
				{
					dataLeft.clearValue();
					dataRight.clearValue();
				}
				
				inPortLeft.setData(dataLeft);
				inPortRight.setData(dataRight);

				inputLeftLog.push_back(dataLeft.getDataString<int64_t>());
				inputRightLog.push_back(dataRight.getDataString<int64_t>());

				// 执行体
				inPortLeft.run();
				inPortRight.run();
				pe.run();

				Data dataOutLeft = outPortLeft.getData();
				Data dataOutRight = outPortRight.getData();

				outputLeftLog.push_back(dataOutLeft.getDataString<int64_t>());
				outputRightLog.push_back(dataOutRight.getDataString<int64_t>());
				
				// 制作打印
				string str = "Input = " + dataLeft.getDataString<int64_t>() + dataRight.getDataString<int64_t>() + "  Output = ";
				str = str + dataOutLeft.getDataString<int64_t>() + dataOutRight.getDataString<int64_t>();
				Logger::WriteMessage(str.c_str());
				
			}

			// Assert验证时序
			for (size_t i = 0; i < inputLeftLog.size() - 4; i++)
				Assert::AreEqual(inputLeftLog[i], outputLeftLog[i + 3]);

			// Assert验证时序
			for (size_t i = 0; i < inputRightLog.size() - 2; i++)
				Assert::AreEqual(inputRightLog[i], outputRightLog[i + 1]);

			
		}
	};
}