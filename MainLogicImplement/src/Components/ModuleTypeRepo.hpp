#pragma once

#include "Module.hpp"

using namespace tinyxml2;

namespace ProjectA
{
	class ModuleTypeRepo : public NonCopyable, public NonMovable
	{
	public:
		explicit ModuleTypeRepo(const string& xmlAddr)
		{
			build(xmlAddr);
		}

		Module getModule(const string& name)
		{
			return _repo[name];
		}

	private:
		void build(const string& xmlAddr)
		{
			XMLDocument doc;
			doc.LoadFile(xmlAddr.c_str());
			XMLElement* xmlRoot = doc.RootElement();

			XMLElement* xmlDataTypeDefinition = xmlRoot->FirstChildElement("DataTypeDefinition");
			_dataTypeRepo.generateFromXml(xmlDataTypeDefinition);

			XMLElement* xmlModule = xmlRoot->FirstChildElement("Module");
			while (xmlModule)
			{
				string moduleName = xmlModule->FindAttribute("name")->Value();
				_repo[moduleName] = Module{ &_dataTypeRepo };
				_repo[moduleName].generateFromXml(xmlModule);
				
				xmlModule = xmlModule->NextSiblingElement("Module");
			}
		}

	private:
		DataTypeRepo _dataTypeRepo;
		unordered_map<string, Module> _repo;
		
	};
}