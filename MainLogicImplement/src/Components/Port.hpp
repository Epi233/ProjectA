/*
 * Port的三个区设定好麻烦啊
 * 干脆取消掉好了
 * 用CycleBuffer搞把
 *
 *  -- 行 2019.11.14
 * 
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
 *  -- 行 2019.11.5
 */
#pragma once

#include "Data.hpp"
#include "../Util/Exception.hpp"

namespace ProjectA
{
	
	class Port
	{
	public:
		explicit Port(const WidthSpec& widthSpec)
			: _widthSpec(widthSpec)
			, _data(widthSpec)
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

		void setData(const Data& data)
		{
			_data = data;
		}

		void run() const
		{
			if (_isTargetSet)
			{
				_target->setData(_data);   //Send current port's data to the target port
			}
		}

		Data getData() const
		{
			return _data;
		}

		WidthSpec getWidthSpec() const
		{
			return _widthSpec;
		}

	private:
		WidthSpec _widthSpec;
		Data _data;

		bool _isTargetSet;
		Port* _target;
	};

}