#pragma once

#include <exception>
#include <cstdint>
#include <vector>
#include <string>

namespace ProjectA
{
	using std::vector;
	using std::string;
	using std::exception;
	
	class PortSpecMismatchError final : public exception
	{
	public:
		PortSpecMismatchError(const string& e, const vector<uint64_t>& out, const vector<uint64_t>& in)
			: exception(e.c_str())
			, _outPortSpec(out)
			, _inPortSpec(in)
		{
		}

		const char* what() const override
		{
			string str = "Port Data Pack Spec Mismatch : Source Port Spec = [";
			for (auto i : _outPortSpec)
				str.append(std::to_string(i) + " ");
			str.pop_back();
			str.append("] Target Port Spec = [");
			for (auto i : _inPortSpec)
				str.append(std::to_string(i) + " ");
			str.pop_back();
			str.append("]");
			return str.c_str();
		}

	private:
		vector<uint64_t> _outPortSpec;
		vector<uint64_t> _inPortSpec;
	};
}