#pragma once

#include <memory>
#include "DataPack.hpp"
#include "../Util/Exception.hpp"

namespace ProjectA
{

	using std::shared_ptr;
	
	enum class PortDirection : char
	{
		in,
		out
	};

	class PortBase
	{
	public:
		const vector<uint64_t> _widthSpec;

		explicit 
		PortBase(vector<uint64_t> widthSpec)
			: _widthSpec(std::move(widthSpec))
		{
		}

		virtual ~PortBase() = default;
	};

	template<PortDirection D>
	class Port
	{
	};

#define IN PortDirection::in
#define OUT PortDirection::out

	template<> class Port<IN>;
	template<> class Port<OUT>;

	using InPortPtr = shared_ptr<Port<IN>>;
	using OutPortPtr = shared_ptr<Port<OUT>>;
	using PortPtr = shared_ptr<PortBase>;

	template<>
	class Port<IN> : public PortBase
	{
	public:
		explicit 
		Port(vector<uint64_t> widthSpec)
			: PortBase(std::move(widthSpec))
			, _from(nullptr)
		{
		}

		void setFromPort(const PortPtr& ptr)
		{
			if (_widthSpec != ptr->_widthSpec)
			_from = ptr;
		}

	private:
		OutPortPtr _from;
	};

	template<>
	class Port<OUT> : public PortBase
	{
	public:
		explicit 
		Port(vector<uint64_t> widthSpec)
			: PortBase(std::move(widthSpec))
			, _to(nullptr)
		{
		}

		void setToPort(const PortPtr& ptr)
		{
			if (_widthSpec != ptr->_widthSpec)
				throw PortSpecMismatchError("Error", ptr->_widthSpec, _widthSpec);
			_to = ptr;
		}

	private:
		InPortPtr _to;
	};

	
}