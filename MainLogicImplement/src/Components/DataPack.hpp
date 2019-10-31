#pragma once

#include "../Define/Define.hpp"
#include "../Util/Exception.hpp"

namespace ProjectA
{
	using WidthSpec = vector<uint64_t>;

	class Data
	{
	public:
		explicit Data(uint64_t size)
			: _size(size)
			, _dataBinary(_size, '0')
		{
		}

		Data(uint64_t size, int64_t data)
			: _size(size)
		{
			generateFromNumber(data);
		}

		Data(uint64_t size, uint64_t data)
			: _size(size)
		{
			generateFromNumber(data);
		}

		Data(uint64_t size, const string& data)
			: _size(size)
		{
			stringBinaryCheck(data);
			generateFromString(data);
		}

		Data(const Data& rhs) = default;

		Data& operator= (const Data& data)
		{
			if (_size != data._size)
				throw DataSizeMismatchError("Data Size Mismatch");
			_dataBinary = data._dataBinary;
			return *this;
		}

		template <typename Type>
		Type getData() const
		{
			throw exception(("Can not convert to type " + static_cast<string>(typeid(Type).name())).c_str());
		}

		// GCC可能不支持类内特化，移植的时候需要注意		
		template <>
		uint64_t getData() const
		{
			return bitset<64>(_dataBinary).to_ullong();
		}

		template <>
		int64_t getData() const
		{
			return static_cast<int64_t>(bitset<64>(_dataBinary).to_ullong());
		}

		template <>
		string getData() const
		{
			return _dataBinary;
		}

	private:
		void generateFromNumber(int64_t data)
		{
			generateFromString(bitset<64>(data).to_string());
		}

		void generateFromNumber(uint64_t data)
		{
			generateFromString(bitset<64>(data).to_string());
		}

		// 检查一个string是不是01序列
		void stringBinaryCheck(const string& str)
		{
			for (auto i : str)
				if (i != '0' && i != '1')
					throw exception("Binary String is not only 0 and 1");
		}

		void generateFromString(const string& str)
		{
			// 输入string必须是一个01序列
			auto itr = str.end();
			--itr;
			for (size_t i = _size - 1; i >= 0; --i)
			{
				_dataBinary[i] = *itr;
				--itr;
			}
		}

	private:
		uint64_t _size;
		string _dataBinary;
	};
	
	class DataPack
	{
	public:
		explicit 
		DataPack(WidthSpec widthSpec)
			: _widthSpec(std::move(widthSpec))
		{
			_dataPack.reserve(_widthSpec.size());
			for (auto i : _widthSpec)
			{
				_dataPack.emplace_back(i);
			}
		}

		bool isSameType(const DataPack& dataPack) const
		{
			return _widthSpec == dataPack._widthSpec;
		}

	private:
		vector<Data> _dataPack;
		WidthSpec _widthSpec;
	};
}