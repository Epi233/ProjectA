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
				delete ptr.second;
		}

	public:
		void run()
		{
			for (auto& port : _inPorts)
				port.run();
			for (auto ptr : _logicUnits)
				ptr.second->run();
			for (auto& port : _outPorts)
				port.run();
		}

		Port* getInPort(uint64_t index)
		{
			return &_inPorts[index];
		}

		Port* getOutPort(uint64_t index)
		{
			return &_outPorts[index];
		}
		
#pragma region 创建LogicUnit
	private:
		void createLogicUnitEmpty(const string& unitName, const string& luaAddr, WidthSpec widthSpec)
		{
			// 创建EMPTY逻辑组件
			LogicUnit<EMPTY>* ptr = new LogicUnit<EMPTY>(luaAddr);
			// 逻辑组件加载Database函数
			_database.luaLoadDatabaseFunctions(ptr->getLuaStatePtr());
			// 向基类转换
			Logic* base_ptr = dynamic_cast<Logic*>(ptr);
			_logicUnits[unitName] = base_ptr;
		}
		
		void createLogicUnitMem(const string& memName, const string& luaAddr, size_t memSize, WidthSpec widthSpec, uint64_t cycleCount)
		{
			// 创建MEM逻辑组件
			LogicUnit<MEM>* ptr = new LogicUnit<MEM>(luaAddr, widthSpec, memSize, cycleCount);
			// Database更新新组件
			_database.insertComponentMem(memName, ptr->getPtr());
			// 逻辑组件加载Database函数
			_database.luaLoadDatabaseFunctions(ptr->getLuaStatePtr());
			// 向基类转换
			Logic* base_ptr = dynamic_cast<Logic*>(ptr);
			_logicUnits[memName] = base_ptr;
		}
#pragma endregion 

#pragma region  xml读取与成员构造
	public:
		void generateFromXml(XMLElement* xmlModule)
		{
			// name
			_moduleName = xmlModule->FindAttribute("name")->Value();
			// in ports
			XMLElement* xmlInPort = xmlModule->FirstChildElement("InPort");
			while (xmlInPort)
			{
				const WidthSpec& widthSpec = _dataTypeRepo->getWidthSpec(xmlInPort->FindAttribute("specName")->Value());
				_inPorts.emplace_back(widthSpec);
				
				xmlInPort = xmlInPort->NextSiblingElement("InPort");
			}
			// out ports
			XMLElement* xmlOutPort = xmlModule->FirstChildElement("OutPort");
			while (xmlOutPort)
			{
				const WidthSpec& widthSpec = _dataTypeRepo->getWidthSpec(xmlOutPort->FindAttribute("specName")->Value());
				_outPorts.emplace_back(widthSpec);

				xmlOutPort = xmlOutPort->NextSiblingElement("OutPort");
			}
			// logic unit
			XMLElement* xmlLogicUnit = xmlModule->FirstChildElement("LogicUnit");
			while (xmlLogicUnit)
			{
				xmlLogicRead(xmlLogicUnit);
				
				xmlLogicUnit = xmlLogicUnit->NextSiblingElement("LogicUnit");
			}
			// 建立连接关系
			XMLElement* xmlConnection = xmlModule->FirstChildElement("Connection");
			xmlBuildConnection(xmlConnection);
		}

	private:
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
			uint64_t cycleCounter = std::stoi(xmlParameters->FindAttribute("cycleCounter")->Value());

			createLogicUnitMem(name, scriptAddr, size, widthSpec, cycleCounter);
		}

		void xmlBuildConnection(XMLElement* xmlConnection)
		{
			// 输入端口建立
			XMLElement* xmlInPortOut = xmlConnection->FirstChildElement("InPortOut");
			while (xmlInPortOut)
			{
				uint64_t index = std::stoi(xmlInPortOut->FindAttribute("index")->Value());
				string targetName = xmlInPortOut->FindAttribute("targetName")->Value();
				uint64_t targetInPortIndex = std::stoi(xmlInPortOut->FindAttribute("targetInPortIndex")->Value());
				_inPorts[index].setTargetPort(_logicUnits[targetName]->getInPortPtr(targetInPortIndex));

				xmlInPortOut = xmlInPortOut->NextSiblingElement("InPortOut");
			}
			// 自由连接建立
			XMLElement* xmlFreeConnection = xmlConnection->FirstChildElement("FreeConnection");
			while (xmlFreeConnection)
			{
				string sourceName = xmlFreeConnection->FindAttribute("sourceName")->Value();
				uint64_t sourceOutPortIndex = std::stoi(xmlFreeConnection->FindAttribute("sourceOutPortIndex")->Value());
				string targetName = xmlFreeConnection->FindAttribute("targetName")->Value();
				uint64_t targetInPortIndex = std::stoi(xmlFreeConnection->FindAttribute("targetInPortIndex")->Value());

				// 保留字__OUTPORT__用来设置输出
				if (targetName == "__OUTPORT__")
					_logicUnits[sourceName]->getOutPortPtr(sourceOutPortIndex)->setTargetPort(&_outPorts[targetInPortIndex]);
				else
					_logicUnits[sourceName]->getOutPortPtr(sourceOutPortIndex)->setTargetPort(_logicUnits[targetName]->getInPortPtr(targetInPortIndex));
				
				xmlFreeConnection = xmlFreeConnection->NextSiblingElement("FreeConnection");
			}
		}
		
#pragma endregion 

	private:
		string _moduleName;

		unordered_map<string, Logic*> _logicUnits;
		Database _database;

		vector<Port> _inPorts;
		vector<Port> _outPorts;

		const DataTypeRepo* _dataTypeRepo;
	};


}