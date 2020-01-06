/*
 * -- 行 2019.11.21 -- 测试过
 *
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
		Data() = delete;
		
		explicit Data(const WidthSpec& spec)
			: _dataCells(vector<DataCell>{})
		{
			for (auto size : spec)
				_dataCells.emplace_back(size);   //Construct a new "Datacell" with parameter "size"
		}

		Data(const WidthSpec& spec, const vector<int64_t>& value)
			: _dataCells(vector<DataCell>{})
		{
			DEBUG_ASSERT(spec.size() == value.size());  //Whether the number of DataCells is equal
			for (size_t i = 0; i < spec.size(); ++i)
				_dataCells.emplace_back(spec[i], value[i]); //Construct a new "Datacell" with parameter "size" and "value"
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

	public: // 对内部vector进行封装用的函数
		DataCell& operator[] (size_t num)
		{
			return _dataCells[num];
		}

	public:
		// 设置值
		void setValue(const vector<int64_t>& value)
		{
			DEBUG_ASSERT(_dataCells.size() == value.size());
			for (size_t i = 0; i < value.size(); ++i)
				_dataCells[i] = DataCell(_dataCells[i].getSize(), value[i]);
		}

		// 所有cell清0
		void clearValue()
		{
			for (auto& i : _dataCells)
				i.setZero();
		}
		
		// 返回内部vector的常引用
		const vector<DataCell>& getDataCells() const
		{
			return _dataCells;
		}

		// 返回内部vector的不同形式的值
		// GCC可能不支持类内特化，移植的时候需要注意
		template <typename Type>
		vector<Type> getDataCells() const
		{
			throw std::exception(("Can not convert to type " + static_cast<std::string>(typeid(Type).name())).c_str());
		}

		template <>
		vector<int64_t> getDataCells<int64_t>() const
		{
			vector<int64_t> result(_dataCells.size());
			for (size_t i = 0; i < result.size(); i++)
				result[i] = _dataCells[i].getData<int64_t>();
			return result;
		}

		template <>
		vector<uint64_t> getDataCells<uint64_t>() const
		{
			vector<uint64_t> result(_dataCells.size());
			for (size_t i = 0; i < result.size(); i++)
				result[i] = _dataCells[i].getData<uint64_t>();
			return result;
		}
		
		template <>
		vector<string> getDataCells<string>() const
		{
			vector<string> result(_dataCells.size());
			for (size_t i = 0; i < result.size(); i++)
				result[i] = _dataCells[i].getData<string>();
			return result;
		}

		// 内部vector整体输出成string
		// GCC可能不支持类内特化，移植的时候需要注意
		template <typename Type>
		string getDataString() const
		{
			throw std::exception(("Can not convert to type " + static_cast<std::string>(typeid(Type).name())).c_str());
		}

		template<>
		string getDataString<string>() const
		{
			string str = "[";
			for (auto& cell : _dataCells)
			{
				str.append(cell.getData<string>());
				str += '_';
			}
			str.pop_back();
			str += "]";
			return str;
		}

		template<>
		string getDataString<int64_t>() const
		{
			string str = "[";
			for (auto& cell : _dataCells)
			{
				str.append(std::to_string(cell.getData<int64_t>()));
				str += '_';
			}
			str.pop_back();
			str += "]";
			return str;
		}

		template<>
		string getDataString<uint64_t>() const
		{
			string str = "[";
			for (auto& cell : _dataCells)
			{
				str.append(std::to_string(cell.getData<uint64_t>()));
				str += '_';
			}
			str.pop_back();
			str += "]";
			return str;
		}

	private:
		vector<DataCell> _dataCells;
	};
	
}