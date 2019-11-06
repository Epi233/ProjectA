#pragma once

#include <map>
#include <functional>
#include <string>
#include "Any.hpp"
#include "FunctionTraits.hpp"
#include "Util.hpp"

using std::multimap;
using std::function;
using std::string;

namespace ProjectA
{

	class MessageBus : NonCopyable
	{
	public:
		template <typename F>
		void attach(F&& f, const string& strTopic = "")
		{
			auto func = toFunction(std::forward<F>(f));
			add(strTopic, std::move(func));
		}

		template <typename R>
		void sendRequest(const string& strTopic = "")
		{
			string strMessageType = strTopic + typeid(function<R()>).name();
			auto range = _map.equal_range(strMessageType);
			for (auto itr = range.first; itr != range.second; ++itr)
			{
				auto f = itr->second.anyCast<function<R()>>();
				f();
			}
		}

		template <typename R, typename... Args>
		void sendRequest(Args&&... args, const string& strTopic = "")
		{
			string strMessageType = strTopic + typeid(function<R()>).name();
			auto range = _map.equal_range(strMessageType);
			for (auto itr = range.first; itr != range.second; ++itr)
			{
				auto f = itr->second.anyCast<function<R()>>();
				f(std::forward<Args>(args)...);
			}
		}

		template <typename R, typename... Args>
		void remove(const string& strTopic = "")
		{
			string strMessageType = strTopic + typeid(function<R()>).name();
			auto range = _map.equal_range(strMessageType);
			_map.erase(range.first, range.second);
		}

	private:
		template <typename F>
		void add(const string& str, F&& f)
		{
			string strTopic = str + typeid(F).name();
			_map.emplace(std::move(strTopic), std::forward<F>(f));
		}


	private:
		multimap<string, Any> _map;

	};

}