#include "CppUnitTest.h"

#include "Components/CycleBuffer.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace MainLogicTest
{
	TEST_CLASS(CycleBufferTest)
	{
	public:
		TEST_METHOD(cycleBufferTest1)
		{
			/* 测试0延迟的机制 */
			// 数据准备
			vector<uint64_t> widthSpec{ 32, 32, 32, 32 };
			int64_t clk = 0;
			ProjectA::CycleBuffer cycleBuffer(0, widthSpec);
			ProjectA::Data data(widthSpec);

			auto generateData = [&]() -> ProjectA::Data
			{
				ProjectA::Data data(widthSpec);
				data.setValue(vector<int64_t>{ clk + 100, clk + 200, -clk - 300, -clk - 400 });
				return data;
			};
			
			while (clk++ <= 25)
			{
				if (clk % 3 == 0)
				{
					data = generateData();
				}
				else
				{
					data.clearValue();
				}

				cycleBuffer.setPrepareData(data);
				cycleBuffer.run();

				string input = data.getDataString<int64_t>();
				string output = cycleBuffer.getSendArea().getDataString<int64_t>();
				Assert::AreEqual(input, output);

				// 打印
				std::string str = "Clk = " + std::to_string(clk) + "  input = ";
				str.append(input + "  output = " + output);
				Logger::WriteMessage(str.c_str());
			}
		}

		TEST_METHOD(cycleBufferTest2)
		{
			/* 测试1延迟 */
			// 数据准备
			vector<uint64_t> widthSpec{ 32, 32, 32, 32 };
			int64_t clk = 0;
			ProjectA::CycleBuffer cycleBuffer(1, widthSpec);
			ProjectA::Data data(widthSpec);

			auto generateData = [&]() -> ProjectA::Data
			{
				ProjectA::Data data(widthSpec);
				data.setValue(vector<int64_t>{ clk + 1, clk + 2, clk + 3, -clk + 4 });
				return data;
			};

			vector<string> inputLog;
			vector<string> outputLog;
			
			// 随机塞入值
			while (clk <= 50)
			{
				if (clk % 7 == 0)
				{
					data = generateData();
				}
				else
				{
					data.clearValue();
				}

				cycleBuffer.setPrepareData(data);
				cycleBuffer.run();

				string input = data.getDataString<int64_t>();
				string output = cycleBuffer.getSendArea().getDataString<int64_t>();
				inputLog.push_back(input);
				outputLog.push_back(output);

				// 打印
				std::string str = "Clk = " + std::to_string(clk) + "  input = ";
				str.append(input + "  output = " + output);
				Logger::WriteMessage(str.c_str());

				// 时钟增加
				clk++;
			}

			// 连续赛入值
			while (clk <= 75)
			{
				data = generateData();

				cycleBuffer.setPrepareData(data);
				cycleBuffer.run();

				string input = data.getDataString<int64_t>();
				string output = cycleBuffer.getSendArea().getDataString<int64_t>();
				inputLog.push_back(input);
				outputLog.push_back(output);

				// 打印
				std::string str = "Clk = " + std::to_string(clk) + "  input = ";
				str.append(input + "  output = " + output);
				Logger::WriteMessage(str.c_str());

				// 时钟增加
				clk++;
			}

			// Assert验证时序
			for (size_t i = 0; i < inputLog.size() - 2; i++)
				Assert::AreEqual(inputLog[i], outputLog[i + 1]);
		}

		TEST_METHOD(cycleBufferTest3)
		{
			/* 测试5延迟 */
			// 数据准备
			vector<uint64_t> widthSpec{ 32, 32, 32, 32 };
			int64_t clk = 0;
			ProjectA::CycleBuffer cycleBuffer(5, widthSpec);
			ProjectA::Data data(widthSpec);

			auto generateData = [&]() -> ProjectA::Data
			{
				ProjectA::Data data(widthSpec);
				data.setValue(vector<int64_t>{ clk + 1, clk + 2, clk + 3, -clk + 4 });
				return data;
			};

			vector<string> inputLog;
			vector<string> outputLog;

			// 随机塞入值
			while (clk <= 50)
			{
				if (clk % 7 == 0)
				{
					data = generateData();
				}
				else
				{
					data.clearValue();
				}

				cycleBuffer.setPrepareData(data);
				cycleBuffer.run();

				string input = data.getDataString<int64_t>();
				string output = cycleBuffer.getSendArea().getDataString<int64_t>();
				inputLog.push_back(input);
				outputLog.push_back(output);

				// 打印
				std::string str = "Clk = " + std::to_string(clk) + "  input = ";
				str.append(input + "  output = " + output);
				Logger::WriteMessage(str.c_str());

				// 时钟增加
				clk++;
			}

			// 连续赛入值
			while (clk <= 100)
			{
				data = generateData();

				cycleBuffer.setPrepareData(data);
				cycleBuffer.run();

				string input = data.getDataString<int64_t>();
				string output = cycleBuffer.getSendArea().getDataString<int64_t>();
				inputLog.push_back(input);
				outputLog.push_back(output);

				// 打印
				std::string str = "Clk = " + std::to_string(clk) + "  input = ";
				str.append(input + "  output = " + output);
				Logger::WriteMessage(str.c_str());

				// 时钟增加
				clk++;
			}

			// Assert验证时序
			for (size_t i = 0; i < inputLog.size() - 6; i++)
				Assert::AreEqual(inputLog[i], outputLog[i + 5]);
		}
	};
}
