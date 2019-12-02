#pragma once
#include "CppUnitTest.h"
#include <cstdint>
#include <vector>

namespace Microsoft {
	namespace VisualStudio {
		namespace CppUnitTestFramework
		{
			template <>
			std::wstring ToString<std::vector<int64_t>>(const class std::vector<int64_t>& t)
			{
				std::wstring temp;
				for (auto i : t)
					temp += std::to_wstring(i) + L"_";
				return temp;
			}
		}
	}
}

