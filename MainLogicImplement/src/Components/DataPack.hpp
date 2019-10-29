#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include "../Util/Bool.hpp"

using std::vector;
using std::string;

namespace ProjectA
{
	class Data final
	{
	public:
		explicit
		Data(uint32_t size)
			: _data(vector<Bool>(size, false))
		{
		}

		explicit
		Data(uint64_t value)
		{
			// TODO
		}

		explicit
		Data(int64_t value)
		{
			// TODO
		}

		string toString() const
		{
			string result = "";
			for (size_t i = _data.size() - 1; i > 0; --i)
				result += _data[i] ? '1' : '0';
			return result;
		}

		uint64_t toUnsignedValue() const
		{
			// TODO
		}

		int64_t toSignedValue() const
		{
			// TODO
		}

		uint64_t getWidth() const
		{
			return _data.size();
		}

		bool isSameWidth(const Data& data) const
		{
			return _data.size() == data.getWidth();
		}

	private:
		vector<Bool> _data;
	};
	
	class DataPack
	{
	public:
		explicit 
		DataPack(vector<uint64_t> widthSpec)
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
		vector<uint64_t> _widthSpec;
	};
}