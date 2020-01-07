/*
 * Module
 * 内部的每个组件都有访问同Module所有组件的权限
 * 按插入顺序仿真，插入顺序必须正确
 *
 *  -- 行 2019.11.7
 */
#pragma once

#include "LogicUnit.hpp"
#include "DataBase.hpp"
#include "Port.hpp"
#include "../UserFunctions/UserFunctions.hpp"
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
		void createLogicUnitPureLogic(const string& unitName, const vector<WidthSpec>& inPortsSpec, const vector<WidthSpec>& outPortsSpec, uint64_t cycleCount, const string& funName)
		{
			_logicUnits[unitName] = new Logic(inPortsSpec, outPortsSpec, &_database, cycleCount, _userFunctions[funName]);
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
			// component
			XMLElement* xmlComponentUnit = xmlModule->FirstChildElement("Component");
			while (xmlComponentUnit)
			{
				xmlComponentRead(xmlComponentUnit);

				xmlComponentUnit = xmlComponentUnit->NextSiblingElement("Component");
			}
			// logic unit
			XMLElement* xmlLogicUnit = xmlModule->FirstChildElement("Logic");
			while (xmlLogicUnit)
			{
				xmlLogicRead(xmlLogicUnit);
				
				xmlLogicUnit = xmlLogicUnit->NextSiblingElement("Logic");
			}
			// 建立连接关系
			XMLElement* xmlConnection = xmlModule->FirstChildElement("Connection");
			xmlBuildConnection(xmlConnection);
		}

	private:

		void xmlComponentRead(XMLElement* xmlComponentUnit)
		{
			string componentName = xmlComponentUnit->FindAttribute("name")->Value();
			string type = xmlComponentUnit->FindAttribute("type")->Value();
			if (type == "MEM")
			{
				XMLElement* xmlParameter = xmlComponentUnit->FirstChildElement("Parameter");
				WidthSpec widthSpec = _dataTypeRepo->getWidthSpec(xmlParameter->FindAttribute("specName")->Value());
				uint64_t size = std::stoi(xmlParameter->FindAttribute("size")->Value());
				_database.addComponentMem(componentName, size, widthSpec);
			}
		}
		
		void xmlLogicRead(XMLElement* xmlLogicUnit)
		{
			string name = xmlLogicUnit->FindAttribute("name")->Value();
			vector<WidthSpec> inPortsSpec;
			vector<WidthSpec> outPortsSpec;

			XMLElement* inputSpecXML = xmlLogicUnit->FirstChildElement("InputSpec");
			while (inputSpecXML)
			{
				inPortsSpec.push_back(_dataTypeRepo->getWidthSpec(inputSpecXML->FindAttribute("specName")->Value()));

				inputSpecXML = inputSpecXML->NextSiblingElement("InputSpec");
			}

			XMLElement* outputSpecXML = xmlLogicUnit->FirstChildElement("OutputSpec");
			while (outputSpecXML)
			{
				outPortsSpec.push_back(_dataTypeRepo->getWidthSpec(outputSpecXML->FindAttribute("specName")->Value()));

				outputSpecXML = outputSpecXML->NextSiblingElement("OutputSpec");
			}

			XMLElement* xmlParameters = xmlLogicUnit->FirstChildElement("Parameter");
			uint64_t cycleCount = std::stoi(xmlParameters->FindAttribute("cycleCounter")->Value());
			string funName = xmlParameters->FindAttribute("funName")->Value();

			createLogicUnitPureLogic(name, inPortsSpec, outPortsSpec, cycleCount, funName);
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
			XMLElement* xmlFreeConnection = xmlConnection->FirstChildElement("BuildConnection");
			while (xmlFreeConnection)
			{
				string sourceName = xmlFreeConnection->FindAttribute("sourceName")->Value();
				uint64_t sourceOutPortIndex = std::stoi(xmlFreeConnection->FindAttribute("sourceOutPortIndex")->Value());
				string targetName = xmlFreeConnection->FindAttribute("targetName")->Value();
				uint64_t targetInPortIndex = std::stoi(xmlFreeConnection->FindAttribute("targetInPortIndex")->Value());

				// 保留字__INPORT__用来设置输入 保留字__OUTPORT__用来设置输出
				if (sourceName == "__INPORT__")
				{
					if (targetName == "__OUTPORT__")
						_inPorts[sourceOutPortIndex].setTargetPort(&_outPorts[targetInPortIndex]);
					else
						_inPorts[sourceOutPortIndex].setTargetPort(_logicUnits[targetName]->getInPortPtr(targetInPortIndex));
				}
				else
				{
					if (targetName == "__OUTPORT__")
						_logicUnits[sourceName]->getOutPortPtr(sourceOutPortIndex)->setTargetPort(&_outPorts[targetInPortIndex]);
					else
						_logicUnits[sourceName]->getOutPortPtr(sourceOutPortIndex)->setTargetPort(_logicUnits[targetName]->getInPortPtr(targetInPortIndex));
				}

				xmlFreeConnection = xmlFreeConnection->NextSiblingElement("BuildConnection");
			}
		}
		
#pragma endregion 

	private:
		string _moduleName;

		unordered_map<string, Logic*> _logicUnits;
		UserFunctions _userFunctions;
		Database _database;

		vector<Port> _inPorts;
		vector<Port> _outPorts;

		const DataTypeRepo* _dataTypeRepo;
	};


}