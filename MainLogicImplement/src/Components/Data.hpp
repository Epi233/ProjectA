#pragma once

#include "../Define/Define.hpp"
#include "../Util/Exception.hpp"

namespace ProjectA
{
	class Data
	{
	public:
		explicit Data(uint64_t size)
			: _size(size)
			, _dataBinary(_size, '0')
		{
		}

		Data(uint64_t size, uint64_t data)
			: _size(size)
		{
			generateFromNumber(data);
		}

		Data(uint64_t size, const std::string& data)
			: _size(size)
		{
			stringBinaryCheck(data);
			generateFromString(data);
		}

		Data(const Data& rhs) = default;

		Data& operator= (const Data& data)
		{
			if (_size != data._size)
				throw std::exception("Data Size Mismatch");
			_dataBinary = data._dataBinary;
			return *this;
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
			return std::bitset<64>(_dataBinary).to_ullong();
		}

		template <>
		int64_t getData() const
		{
			return static_cast<int64_t>(std::bitset<64>(_dataBinary).to_ullong());
		}

		template <>
		std::string getData() const
		{
			return _dataBinary;
		}

	private:
		void generateFromNumber(int64_t data)
		{
			generateFromString(std::bitset<64>(data).to_string());
		}

		void generateFromNumber(uint64_t data)
		{
			generateFromString(std::bitset<64>(data).to_string());
		}

		// 检查一个string是不是01序列
		void stringBinaryCheck(const std::string& str)
		{
			for (auto i : str)
				if (i != '0' && i != '1')
					throw std::exception("Binary String is not only 0 and 1");
		}

		void generateFromString(const std::string& str)
		{
			_dataBinary = "";
			// 输入string必须是一个01序列
			auto itr = str.end();
			--itr;
			uint64_t count = _size;
			while (count--)
			{
				_dataBinary += *itr;
				--itr;
			}
			std::reverse(_dataBinary.begin(), _dataBinary.end());
		}

	private:
		uint64_t _size;
		std::string _dataBinary;
	};
}