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
			XMLDocument doc;
			doc.LoadFile("ModuleTestConfig");
			XMLElement* xmlRoot = doc.RootElement();
			
			XMLElement* xmlDataTypeDefinition = xmlRoot->FirstChildElement("DataTypeDefinition");
			ProjectA::DataTypeRepo dataTypeRepo{};
			dataTypeRepo.generateFromXml(xmlDataTypeDefinition);

			XMLElement* xmlModule = xmlRoot->FirstChildElement("Module");
			ProjectA::Module pe(&dataTypeRepo);
			pe.generateFromXml(xmlModule);

			
		}
	};
}