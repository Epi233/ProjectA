#pragma once

#include "../Define/Define.hpp"
#include "../Util/Exception.hpp"
#include "Data.hpp"

namespace ProjectA
{
	using WidthSpec = vector<uint64_t>;

	class DataPack
	{
	public:
		DataPack()
			: _dataPack(vector<Data>{})
			, _widthSpec(WidthSpec{})
		{
		}
		
		explicit DataPack(WidthSpec widthSpec)
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