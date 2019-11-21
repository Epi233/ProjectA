#include "CppUnitTest.h"

#include "Components/Data.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace MainLogicTest
{
	TEST_CLASS(DataCellTest)
	{
	public:
		TEST_METHOD(dataCellTest1)
		{
			// 测试初始化为0的情况
			ProjectA::DataCell dataCell(5);
			// Logger::WriteMessage(dataCell.getData<string>().c_str());
			Assert::AreEqual("00000", dataCell.getData<string>().c_str());
			Assert::AreEqual(uint64_t(0), dataCell.getData<uint64_t>());
			Assert::AreEqual(int64_t(0), dataCell.getData<int64_t>());
			Assert::AreEqual(uint64_t(5), dataCell.getSize());
		}

		TEST_METHOD(dataCellTest2)
		{
			// 测试正数初始化
			ProjectA::DataCell dataCell(10, 300);
			Assert::AreEqual("0100101100", dataCell.getData<string>().c_str());
			Assert::AreEqual(uint64_t(300), dataCell.getData<uint64_t>());
			Assert::AreEqual(int64_t(300), dataCell.getData<int64_t>());
			Assert::AreEqual(uint64_t(10), dataCell.getSize());
		}

		TEST_METHOD(dataCellTest3)
		{
			// 测试size小于初始化值的情况
			ProjectA::DataCell dataCell(5, 300);
			Assert::AreEqual("01100", dataCell.getData<string>().c_str());
			Assert::AreEqual(uint64_t(12), dataCell.getData<uint64_t>());
			Assert::AreEqual(int64_t(12), dataCell.getData<int64_t>());
			Assert::AreEqual(uint64_t(5), dataCell.getSize());
		}

		TEST_METHOD(dataCellTest4)
		{
			// 测试负数的情况
			ProjectA::DataCell dataCell(10, -300);
			// Logger::WriteMessage(dataCell.getData<string>().c_str());
			Assert::AreEqual("1011010100", dataCell.getData<string>().c_str());
			Assert::AreEqual(uint64_t(724), dataCell.getData<uint64_t>());
			Assert::AreEqual(int64_t(-300), dataCell.getData<int64_t>());
			Assert::AreEqual(uint64_t(10), dataCell.getSize());
		}

		TEST_METHOD(dataCellTest5)
		{
			// 测试负数的截断
			ProjectA::DataCell dataCell(3, -123);
			// Logger::WriteMessage(dataCell.getData<string>().c_str());
			Assert::AreEqual("101", dataCell.getData<string>().c_str());
			Assert::AreEqual(uint64_t(5), dataCell.getData<uint64_t>());
			Assert::AreEqual(int64_t(-3), dataCell.getData<int64_t>());
			Assert::AreEqual(uint64_t(3), dataCell.getSize());
		}
	};

	TEST_CLASS(DataTest)
	{
	public:
		TEST_METHOD(dataTest1)
		{
			ProjectA::WidthSpec widthSpec{3, 2, 1, 32};
			ProjectA::Data data(widthSpec);
			Assert::AreEqual("[0_0_0_0]", data.getDataString<int64_t>().c_str());

			// DataCell会截断数据
			vector<int64_t> value1{ 123, 321, 1234, 4321 };
			data.setValue(value1);
			Assert::AreEqual("[3_1_0_4321]", data.getDataString<int64_t>().c_str());

			// DataCell会截断数据
			vector<int64_t> value2{ -123, -321, -1234, -4321 };
			data.setValue(value2);
			Assert::AreEqual("[-3_-1_0_-4321]", data.getDataString<int64_t>().c_str());

			// clear测试
			data.clearValue();
			Assert::AreEqual("[0_0_0_0]", data.getDataString<int64_t>().c_str());
		}
	};
}
