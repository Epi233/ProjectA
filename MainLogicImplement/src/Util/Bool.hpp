#pragma once

namespace ProjectA
{
	class Bool
	{
	public:
		Bool()
			: _value()
		{
		}

		Bool(bool value_)
			: _value(value_)
		{
		}

		operator bool() const
		{
			return _value;
		}

		bool* operator& ()
		{
			return &_value;
		}

		const bool* operator&() const
		{
			return &_value;
		}

	private:

		bool _value;

	};

#define Bool Bool

}