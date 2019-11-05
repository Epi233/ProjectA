/*
 * Data是由DataCell组成的数据
 * 是流程中数据传递的基本单位
 * WidthSpec是Data中每一个Cell的size组成的vector
 *
 * Data的构造必须指明WidthSpec
 * Data的赋值会自动检查Data的size匹配与Data中每一个Cell的size匹配
 * 
 */
#pragma once

#include "../Define/Define.hpp"
#include "../Util/Exception.hpp"
#include "DataCell.hpp"

namespace ProjectA
{
	using WidthSpec = vector<uint64_t>;

	class Data
	{
	public:
		explicit Data(const WidthSpec& spec)
			: _dataCells(vector<DataCell>{})
		{
			for (auto size : spec)
				_dataCells.emplace_back(size);
		}

		Data(const Data& rhs) = default;

		Data& operator= (const Data& rhs)
		{
			if (_dataCells.size() != rhs._dataCells.size())
				throw DataSizeMismatchError("Data Size Mismatch");
			for (size_t i = 0; i < _dataCells.size(); ++i)
				_dataCells[i] = rhs._dataCells[i];
			return *this;
		}

	public:
		DataCell& operator[] (size_t num)
		{
			return _dataCells[num];
		}

		void push_back(const DataCell& dataCell)
		{
			_dataCells.push_back(dataCell);
		}

		void push_back(DataCell&& dataCell)
		{
			_dataCells.push_back(std::forward<DataCell>(dataCell));
		}

		void emplace_back(uint64_t size)
		{
			_dataCells.emplace_back(size);
		}

		void emplace_back(uint64_t size, uint64_t data)
		{
			_dataCells.emplace_back(size, data);
		}

		void emplace_back(uint64_t size, const string& data)
		{
			_dataCells.emplace_back(size, data);
		}

		void emplace_back(uint64_t size, string&& data)
		{
			_dataCells.emplace_back(size, std::forward<string>(data));
		}

	private:
		vector<DataCell> _dataCells;
	};
	
}