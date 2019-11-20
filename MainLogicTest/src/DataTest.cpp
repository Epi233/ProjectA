#include "CppUnitTest.h"

#include "Components/Data.hpp"

namespace Microsoft { namespace VisualStudio { namespace CppUnitTestFramework
{
	template <>
	std::wstring ToString<vector<int64_t>>(const class vector<int64_t>& t)
	{
		std::wstring temp;
		for (auto i : t)
			temp += std::to_wstring(i) + L"_";
		return temp;
	}
}}}

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
	};

	TEST_CLASS(DataTest)
	{
	public:
		TEST_METHOD(dataTest1)
		{
			ProjectA::WidthSpec widthSpec{3, 2, 1, 32};
			ProjectA::Data data(widthSpec);

			Assert::AreEqual(vector<int64_t>{0, 0, 0, 0}, data.getDataCellsInt64());
		}
	};
}
