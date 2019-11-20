/*
 * Data是由DataCell组成的数据
 * 是流程中数据传递的基本单位
 * WidthSpec是Data中每一个Cell的size组成的vector
 *
 * Data的构造必须指明WidthSpec
 * 
 * Data的赋值会自动检查Data的size匹配与Data中每一个Cell的size匹配
 *
 *  -- 行 2019.11.6
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

		Data(const WidthSpec& spec, const vector<int64_t>& value)
			: _dataCells(vector<DataCell>{})
		{
			DEBUG_ASSERT(spec.size() == value.size());
			for (size_t i = 0; i < spec.size(); ++i)
				_dataCells.emplace_back(spec[i], value[i]);
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

		void emplace_back(uint64_t size, int64_t data)
		{
			_dataCells.emplace_back(size, data);
		}

		const vector<DataCell>& getDataCells() const
		{
			return _dataCells;
		}

		vector<int64_t> getDataCellsInt64() const
		{
			vector<int64_t> result(_dataCells.size());
			for (size_t i = 0; i < result.size(); i++)
				result[i] = _dataCells[i].getData<int64_t>();
			return result;
		}

		void setValue(const vector<int64_t>& value)
		{
			DEBUG_ASSERT(_dataCells.size() == value.size());
			for (size_t i = 0; i < value.size(); ++i)
				_dataCells[i] = DataCell(_dataCells[i].getSize(), value[i]);
		}

	private:
		vector<DataCell> _dataCells;
	};
	
}