#include "CppUnitTest.h"

#include "Components/DataCell.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace MainLogicTest
{		
	TEST_CLASS(DataTest)
	{
	public:

		TEST_METHOD(dataCellStaticFunctions)
		{
			string str1 = "1010101110";
			ProjectA::DataCell::reverseAndAddOne(str1);
			string str2 = "0000111111";
			ProjectA::DataCell::reverseAndAddOne(str2);
			string str3 = "0000000";
			ProjectA::DataCell::reverseAndAddOne(str3);
			Assert::AreEqual("0101010010", str1.c_str());
			Assert::AreEqual("1111000001", str2.c_str());
			Assert::AreEqual("0000000", str3.c_str());
		}
		
		TEST_METHOD(dataCellTest_1)
		{
			// 测试初始化为0的情况
			ProjectA::DataCell dataCell(5);
			Assert::AreEqual("00000", dataCell.getData<string>().c_str());
			Assert::AreEqual(uint64_t(0), dataCell.getData<uint64_t>());
			Assert::AreEqual(int64_t(0), dataCell.getData<int64_t>());
			Assert::AreEqual(uint64_t(5), dataCell.getSize());

		}

		TEST_METHOD(dataCellTest_2)
		{
			// 测试正数初始化
			ProjectA::DataCell dataCell(10, 300);
			Assert::AreEqual("0100101100", dataCell.getData<string>().c_str());
			Assert::AreEqual(uint64_t(300), dataCell.getData<uint64_t>());
			Assert::AreEqual(int64_t(300), dataCell.getData<int64_t>());
			Assert::AreEqual(uint64_t(10), dataCell.getSize());
		}
		
		TEST_METHOD(dataCellTest_3)
		{
			// 测试size小于初始化值的情况
			ProjectA::DataCell dataCell(5, 300);
			Assert::AreEqual("01100", dataCell.getData<string>().c_str());
			Assert::AreEqual(uint64_t(12), dataCell.getData<uint64_t>());
			Assert::AreEqual(int64_t(12), dataCell.getData<int64_t>());
			Assert::AreEqual(uint64_t(5), dataCell.getSize());
		}

		TEST_METHOD(dataCellTest_4)
		{
			// 测试负数的情况
			ProjectA::DataCell dataCell(10, -300);
			Logger::WriteMessage(dataCell.getData<string>().c_str());
			Assert::AreEqual("1011010100", dataCell.getData<string>().c_str());
			Assert::AreEqual(uint64_t(724), dataCell.getData<uint64_t>());
			Assert::AreEqual(int64_t(-300), dataCell.getData<int64_t>());
			Assert::AreEqual(uint64_t(10), dataCell.getSize());
		}
	};
}