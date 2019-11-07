/*
 * Port类
 * 并不是物理硬件意义上的端口
 * 而是用来组织连接不同模块的一个抽象概念
 * 同时物理上的打拍子功能也靠Port实现
 *
 * 每个Port分三个区
 * 准备区 缓冲区 发送区
 *
 * 每个Clk对Port执行的事情为
 * 若Port不打拍子
 * 准备区数据进去发送区，缓冲区没用
 * 若打拍子
 * 缓冲区数据进去发送区
 * 准备区数据进去缓冲区
 *
 * 行 2019.11.5
 */
#pragma once

#include "Data.hpp"
#include "../Util/Exception.hpp"

namespace ProjectA
{
	
	class Port
	{
	public:
		Port(bool isBuffered, const WidthSpec& widthSpec)
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

		void run()
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