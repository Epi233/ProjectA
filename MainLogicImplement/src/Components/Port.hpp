/*
 * 
 */
#pragma once

#include "Data.hpp"
#include "../Util/Exception.hpp"

namespace ProjectA
{
	
	class Port
	{
	public:
		Port(bool isBuffered, WidthSpec widthSpec)
			: _isBuffered(isBuffered)
			, _prepareArea(widthSpec)
			, _bufferArea(widthSpec)
			, _sendArea(widthSpec)
			, _isTargetSet(false)
			, _target(nullptr)
		{
		}

		void setTargetPort(Port* target)
		{
			DEBUG_ASSERT(target != nullptr);
			_isTargetSet = true;
			_target = target;
		}

		void setPrepareArea(const Data& data)
		{
			_prepareArea = data;
		}

		void goFlow()
		{
			if (_isBuffered)
			{
				_sendArea = _bufferArea;
				_bufferArea = _prepareArea;
			}
			else
			{
				_sendArea = _prepareArea;
			}
			if (_isTargetSet)
			{
				_target->setPrepareArea(_sendArea);
			}
		}

		Data getSendArea() const
		{
			return _sendArea;
		}

	private:
		bool _isBuffered;

		Data _prepareArea;
		Data _bufferArea;
		Data _sendArea;

		bool _isTargetSet;
		Port* _target;
	};

}