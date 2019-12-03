#pragma once

#include "Data.hpp"
#include "tinyxml2.h"

using namespace tinyxml2;

namespace ProjectA
{
	class DataTypeRepo
	{
	public:
		DataTypeRepo() = default;

		void insert(const string& name, WidthSpec widthSpec)
		{
			_repo[name] = widthSpec;
		}

		const WidthSpec& getWidthSpec(const string& name) const
		{
			auto itr = _repo.find(name);
			DEBUG_ASSERT(itr != _repo.end());
			return itr->second;
		}

		void generateFromXml(XMLElement* dataTypeDefinitionXml)
		{
			auto generateFromString = [](string& str) -> WidthSpec
			{
				WidthSpec widthSpec;
				vector<string> temp = Util::splitString(str, "][");
				Util::deleteAllMark(temp.front(), "[");
				Util::deleteAllMark(temp.back(), "]");
				for (auto& s : temp)
					widthSpec.push_back(std::stoi(s));
				return widthSpec;
			};

			
			XMLElement* dataTypeXml = dataTypeDefinitionXml->FirstChildElement("DataType");
			while (dataTypeXml)
			{
				string name = dataTypeXml->FindAttribute("name")->Value();
				string spec = dataTypeXml->FindAttribute("spec")->Value();
				
				if (_repo.find(spec) != _repo.end())
					throw std::exception((name + " has been defined, may be you make them same names").c_str());

				_repo[name] = generateFromString(spec);
				
				dataTypeXml = dataTypeXml->NextSiblingElement("DataType");
			}
		}

	private:
		unordered_map<string, WidthSpec> _repo;
	};
}

