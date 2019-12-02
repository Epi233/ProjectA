/*
 * DataCell重新设计
 * 不再使用字符串存储来来实现功能
 * 内部用一个vector<bool>来存
 * (因为不会返回引用，出于效率考虑不用Bool用bool)
 * 所有数都按符号数来写入
 * 内部存补码
 * 输出可以以符号数输出，也可以强制解读为无符号，也可以输出字符串
 *
 * 取消了用字符串来构造的构造函数
 * 感觉没什么用
 * 
 *  -- 行 2019.11.20 -- 测试过
 *
 * DataCell 为基本数据单元
 * 没有默认构造函数
 * 单参数构造是比特位
 * 双参构造第二个参数是数值
 * 支持有符号，无信号，和01字符串
 * 支持到64位
 *
 * 内部用01字符串存储来实现功能
 *
 * 赋值时会自动检查size的匹配
 * 只有size匹配才可以赋值
 *
 *
 *  -- 行 2019.11.5
 */


#pragma once

#include "../Define/Define.hpp"
#include "../Util/Exception.hpp"

namespace ProjectA
{
	// 不从vector<bool>中返回引用，所以用bool不用Bool
	class DataCell
	{
	public:
		explicit DataCell(uint64_t size)
			: _data(size)
		{
		}

		DataCell(uint64_t size, int64_t data)
			: _data(size)
		{
			generateFromNumber(data);
		}

		DataCell(const DataCell& rhs) = default;

	public:
		uint64_t getSize() const
		{
			return _data.size();
		}
		
		template <typename Type>
		Type getData() const
		{
			throw std::exception(("Can not convert to type " + static_cast<std::string>(typeid(Type).name())).c_str());
		}

		// GCC可能不支持类内特化，移植的时候需要注意		
		template <>
		uint64_t getData() const
		{
			string str = "";
			for (auto i : _data)
				str += i ? '1' : '0';
			return bitset<32>(str).to_ullong();
		}

		template <>
		int64_t getData() const
		{
			if (!_data[0])
				return static_cast<int64_t>(getData<uint64_t>());
			else
			{
				string temp = getData<string>();
				reverseAndAddOne(temp);
				return -static_cast<int64_t>(bitset<32>(temp).to_ullong());
			}
		}

		template <>
		std::string getData() const
		{
			string str = "";
			for (auto i : _data)
				str += i ? '1' : '0';
			return str;
		}

		void setZero()
		{
			std::fill(begin(_data), end(_data), false);
		}

	private:
		void generateFromNumber(int64_t data)
		{
			std::bitset<64> tempSet(data);
			size_t pos = 0;
			int size = static_cast<int>(_data.size() - 1);
			while (size >= 0)
			{
				_data[size] = tempSet[pos];
				size--;
				pos++;
			}
		}

	private:
		vector<bool> _data;

	private:
		static void reverseAndAddOne(string& str)
		{
			for (auto& i : str)
				i = i == '0' ? '1' : '0';

			bool flag = true;
			int point = static_cast<int>(str.size() - 1);
			while (flag && point >= 0)
			{
				if (str[point] == '1')
					str[point] = '0';
				else
				{
					str[point] = '1';
					flag = false;
				}

				point--;
			}
		}
	};

}