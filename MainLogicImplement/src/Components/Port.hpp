#pragma once

#include "DataPack.hpp"
#include "../Util/Exception.hpp"

namespace ProjectA
{
	
	enum class PortDirection : char
	{
		in,
		out
	};

	template <PortDirection D>
	class Port
	{
	};

#define IN PortDirection::in
#define OUT PortDirection::out

	template<>
	class Port<IN>
	{
	public:
		explicit Port(vector<uint64_t> widthSpec)
			: _widthSpec(widthSpec)
		{
		}

		vector<uint64_t> getWidthSpec() const
		{
			return _widthSpec;
		}

	private:
		const vector<uint64_t> _widthSpec;
	};

	template<>
	class Port<OUT>
	{
	public:
		explicit Port(vector<uint64_t> widthSpec)
			: _widthSpec(widthSpec)
			, _toOut(nullptr)
		{
		}

		void setToPort(const Port<IN>* ptr)
		{
			if (_widthSpec != ptr->getWidthSpec())
				throw PortSpecMismatchError("Error", ptr->getWidthSpec(), _widthSpec);
			_toOut = ptr;
		}

		vector<uint64_t> getWidthSpec() const
		{
			return _widthSpec;
		}

	private:
		const vector<uint64_t> _widthSpec;
		const Port<IN>* _toOut;
	};

	

	

	
}