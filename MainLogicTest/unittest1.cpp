#include "stdafx.h"
#include "CppUnitTest.h"
#include "Components/DataCell.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace MainLogicTest
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		
		TEST_METHOD(dataTest)
		{
			ProjectA::DataCell data1(5, 500);
			ProjectA::DataCell data2(5, 500);
			string data1Str = data1.getData<string>();
			string data2Str = data2.getData<string>();
			Logger::WriteMessage(data1Str.c_str());
			Logger::WriteMessage(data2Str.c_str());
			Assert::AreEqual(data1Str, data2Str);
		}

	};
}