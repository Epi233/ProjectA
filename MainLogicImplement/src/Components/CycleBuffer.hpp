/*
 * -- 行 2019.11.21 -- 测试完毕
 *
 * 补充支持Cycle = 0的情况
 * 直接过CycleBuffer不打拍子
 *
 * 取消了查看准备区有效和发送区有效这个设计
 * CycleBuffer按照物理真实不管准备区是什么
 * 都每个时钟拉数进来
 * 发送区同理
 *
 * 外部打数进来以后，要在正确的时钟位置拿数
 * 不然数就没了
 *
 *  -- 行 2019.11.14
 *
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
		CycleBuffer() = default;

		explicit CycleBuffer(uint64_t cycle, const WidthSpec& spec)
			: _cycleNum(cycle)
			, _widthSpec(spec)
			, _prepareArea(spec)
			, _bufferList()
			, _sendArea(spec)
		{
		}

		void setPrepareData(const Data& data)
		{
			_prepareArea = data;
		}

		Data getSendArea() const
		{
			return _sendArea;
		}

		void run()
		{
			// 计数为0需要单独处理 不过缓冲区
			if (_cycleNum == 0)
			{
				_sendArea = _prepareArea;
			}

			else
			{
				if (!_bufferList.empty())
				{
					// 计数剩下1的pop出来
					if (_bufferList.front().count == 1)
					{
						_sendArea = _bufferList.front().data;
						_bufferList.pop_front();
					}
					// 全体计数
					for (auto& i : _bufferList)
						i.count--;
				}
				// 准备区进缓冲区
				_bufferList.emplace_back(_prepareArea, _cycleNum);
			}
		}

	private:
		uint64_t _cycleNum;
		WidthSpec _widthSpec;

		Data _prepareArea;
		list<CycleCounter> _bufferList;
		Data _sendArea;
	};
}