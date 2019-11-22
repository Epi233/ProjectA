#include "CppUnitTest.h"

#include "Components/Module.hpp"
#include "tinyxml2.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace tinyxml2;

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
			ProjectA::DataTypeRepo dataTypeRepo{};
			dataTypeRepo.generateFromXml(xmlDataTypeDefinition);

			XMLElement* xmlModule = xmlRoot->FirstChildElement("Module");
			ProjectA::Module pe(&dataTypeRepo);
			pe.generateFromXml(xmlModule);

			// 数据初始化
			vector<uint64_t> widthSpec{ 32, 1 };
			int64_t clk = 0;
			ProjectA::Data dataLeft(widthSpec);
			ProjectA::Data dataRight(widthSpec);

			auto generateDataLeft = [&]() -> ProjectA::Data
			{
				ProjectA::Data data(widthSpec);
				data.setValue(vector<int64_t>{ clk + 100, clk });
				return data;
			};

			auto generateDataRight = [&]() -> ProjectA::Data
			{
				ProjectA::Data data(widthSpec);
				data.setValue(vector<int64_t>{ clk + 100, clk + 1 });
				return data;
			};

			while (clk++ <= 30)
			{
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

				
			}

			
		}
	};
}