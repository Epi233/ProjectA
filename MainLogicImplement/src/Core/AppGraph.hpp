#pragma once

#include "../Components/ModuleTypeRepo.hpp"

using namespace tinyxml2;

namespace ProjectA
{

	class AppNode
	{
	public:
		AppNode(const string& name, const Module& m)
			: _nodeName(name)
			, _module(m)
		{
		}

		Port* getInPort(uint64_t index)
		{
			return _module.getInPort(index);
		}

		Port* getOutPort(uint64_t index)
		{
			return _module.getOutPort(index);
		}

	private:
		string _nodeName;
		Module _module;
	};

	class AppGraph
	{
	public:




	private:
		void build(const string& xmlAddr)
		{
			
		}

	private:
		vector<AppNode> _nodes;
		
	};
}