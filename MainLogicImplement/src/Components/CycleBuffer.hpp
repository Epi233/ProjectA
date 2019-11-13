/*
 * CycleBuffer是打拍子用的数据结构
 *
 * 取数前必须检查checkSendAreaValid()
 * 有效的时候才可以取数
 *
 * 只能每个周期塞一个数
 * 需要人为防止同一个周期塞入>1个数
 * 代码本身不对这个事情进行检测
 *
 *  -- 行 2019.11.13
 */
#pragma once
#include "../Define/Define.hpp"
#include "../Util/Exception.hpp"
#include "Data.hpp"

namespace ProjectA
{
	class CycleBuffer
	{
	public:
		struct CycleCounter
		{
			Data data;
			uint64_t count;

			CycleCounter(const Data& d, uint64_t cycle)
				: data(d)
				, count(cycle)
			{
			}
		};
		
	public:
		explicit CycleBuffer(uint64_t cycle, const WidthSpec& spec)
			: _cycleNum(cycle)
			, _widthSpec(spec)
			, _bufferList()
			, _isSendAreaValid(false)
			, _sendArea(spec)
		{
		}

		void addData(const Data& data)
		{
			_bufferList.emplace_back(data, _cycleNum);
		}

		void run()
		{
			_isSendAreaValid = false;
			if (_bufferList.begin()->count == 1)
			{
				_isSendAreaValid = true;
				_sendArea = _bufferList.front().data;
				_bufferList.pop_front();
			}
				
			for (auto& i : _bufferList)
				i.count--;
		}

		bool checkSendAreaValid() const
		{
			return _isSendAreaValid;
		}

		

	private:
		uint64_t _cycleNum;
		
		WidthSpec _widthSpec;
		list<CycleCounter> _bufferList;

		bool _isSendAreaValid;
		Data _sendArea;
	};
}