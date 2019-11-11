/*
 * Module
 * 内部的每个组件都有访问同Module所有组件的权限
 * 按插入顺序仿真，插入顺序必须正确
 *
 *  -- 行 2019.11.7
 */
#pragma once

#include "LogicUnit.hpp"
#include "Database.hpp"
#include "Port.hpp"
#include "tinyxml2.h"

using namespace tinyxml2;

namespace ProjectA
{
	// TODO 多输入输出口 Port连接 内部Port连接 
	class Module
	{
	public:
		explicit Module(const DataTypeRepo* _dataTypeRepoPtr)
			: _dataTypeRepo(_dataTypeRepoPtr)
		{
		}
		
		~Module()
		{
			for (auto ptr : _logicUnits)
				delete ptr;
		}

	public:
		void run()
		{
			for (auto& port : _inPorts)
				port.run();
			for (auto ptr : _logicUnits)
				ptr->run();
			for (auto& port : _outPorts)
				port.run();
		}
		
#pragma region 创建LogicUnit
	private:
		void createLogicUnitEmpty(const string& luaAddr, WidthSpec widthSpec)
		{
			// 创建EMPTY逻辑组件
			LogicUnit<EMPTY>* ptr = new LogicUnit<EMPTY>(luaAddr, widthSpec);
			// 逻辑组件加载Database函数
			_database.luaLoadDatabaseFunctions(ptr->getLuaStatePtr());
			// 向基类转换
			Logic* base_ptr = dynamic_cast<Logic*>(ptr);
			_logicUnits.push_back(base_ptr);
		}
		
		void createLogicUnitMem(const string& memName, const string& luaAddr, size_t memSize, WidthSpec widthSpec)
		{
			// 创建MEM逻辑组件
			LogicUnit<MEM>* ptr = new LogicUnit<MEM>(luaAddr, memSize, widthSpec);
			// Database更新新组件
			_database.insertComponentMem(memName, ptr->getPtr());
			// 逻辑组件加载Database函数
			_database.luaLoadDatabaseFunctions(ptr->getLuaStatePtr());
			// 向基类转换
			Logic* base_ptr = dynamic_cast<Logic*>(ptr);
			_logicUnits.push_back(base_ptr);
		}
#pragma endregion 

#pragma region  xml读取与成员构造
	private:
		void xmlRead(XMLElement* xmlModule)
		{
			// name
			_moduleName = xmlModule->FindAttribute("name")->Value();
			// in ports
			XMLElement* xmlInPort = xmlModule->FirstChildElement("InPort");
			while (xmlInPort)
			{
				const WidthSpec& widthSpec = _dataTypeRepo->getWidthSpec(xmlInPort->FindAttribute("specName")->Value());
				bool isBuffered = string(xmlInPort->FindAttribute("isBuffered")->Value()) == "true" ? true : false;
				_inPorts.emplace_back(isBuffered, widthSpec);
				
				xmlInPort = xmlInPort->NextSiblingElement("InPort");
			}
			// out ports
			XMLElement* xmlOutPort = xmlModule->FirstChildElement("OutPort");
			while (xmlInPort)
			{
				const WidthSpec& widthSpec = _dataTypeRepo->getWidthSpec(xmlOutPort->FindAttribute("specName")->Value());
				bool isBuffered = string(xmlOutPort->FindAttribute("isBuffered")->Value()) == "true" ? true : false;
				_outPorts.emplace_back(isBuffered, widthSpec);

				xmlOutPort = xmlOutPort->NextSiblingElement("OutPort");
			}
			// logic unit
			XMLElement* xmlLogicUnit = xmlModule->FirstChildElement("LogicUnit");
			while (xmlLogicUnit)
			{
				xmlLogicRead(xmlLogicUnit);
				xmlLogicUnit = xmlLogicUnit->NextSiblingElement("LogicUnit");
			}
		}

		void xmlLogicRead(XMLElement* xmlLogicUnit)
		{
			string componentType = xmlLogicUnit->FindAttribute("type")->Value();
			if (componentType == "MEM")
				xmlLogicReadMem(xmlLogicUnit);
			// TODO 添加其他类型
		}

		void xmlLogicReadMem(XMLElement* xmlLogicUnit)
		{
			string name = xmlLogicUnit->FindAttribute("name")->Value();
			string scriptAddr = xmlLogicUnit->FindAttribute("scriptName")->Value();
			
			XMLElement* xmlParameters = xmlLogicUnit->FirstChildElement("Parameter");
			uint64_t size = std::stoi(xmlParameters->FindAttribute("size")->Value());
			const WidthSpec& widthSpec = _dataTypeRepo->getWidthSpec(xmlParameters->FindAttribute("specName")->Value());

			createLogicUnitMem(name, scriptAddr, size, widthSpec);
		}
		
#pragma endregion 

	private:
		string _moduleName;

		vector<Logic*> _logicUnits;
		Database _database;

		vector<Port> _inPorts;
		vector<Port> _outPorts;

		const DataTypeRepo* _dataTypeRepo;
	};


}