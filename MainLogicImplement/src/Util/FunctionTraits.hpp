#pragma once

/*
 * FunctionTraits.hpp
 * 获取函数的实际类型用
 */

#include <functional>
#include <tuple>

namespace ProjectA
{

	template <typename T>
	struct FunctionTraits;

	template <typename Ret, typename... Args>
	struct FunctionTraits<Ret(Args...)>
	{
		enum
		{
			arity = sizeof...(Args)
		};

		typedef Ret FunctionType(Args...);
		typedef Ret ReturnType;
		typedef Ret(*Pointer)(Args...);

		using stlFunctionType = std::function<FunctionType>;

		template<size_t I>
		struct args
		{
			static_assert(I < arity, "index must less than sizeof Args");
			using type = typename std::tuple_element<I, std::tuple<Args...>>::type;
		};

	};

	// 函数指针
	template <typename Ret, typename... Args>
	struct FunctionTraits<Ret(*)(Args...)> : FunctionTraits<Ret(Args...)> {};

	// std function
	template <typename Ret, typename... Args>
	struct FunctionTraits<std::function<Ret(Args...)>> : FunctionTraits<Ret(Args...)> {};

	// 成员函数
	template <typename ReturnType, typename ClassType, typename... Args>
	struct FunctionTraits<ReturnType(ClassType::*)(Args...)> : FunctionTraits<ReturnType(Args...)> {};

	template <typename ReturnType, typename ClassType, typename... Args>
	struct FunctionTraits<ReturnType(ClassType::*)(Args...) const> : FunctionTraits<ReturnType(Args...)> {};

	template <typename ReturnType, typename ClassType, typename... Args>
	struct FunctionTraits<ReturnType(ClassType::*)(Args...) volatile> : FunctionTraits<ReturnType(Args...)> {};

	template <typename ReturnType, typename ClassType, typename... Args>
	struct FunctionTraits<ReturnType(ClassType::*)(Args...) const volatile> : FunctionTraits<ReturnType(Args...)> {};

	// 函数对象
	template <typename Callable>
	struct FunctionTraits : FunctionTraits<decltype(&Callable::operator())> {};

	template <typename Function>
	typename FunctionTraits<Function>::stlFunctionType toFunction(const Function& lambda)
	{
		return static_cast<FunctionTraits<Function>::stlFunctionType>(lambda);
	}

	template <typename Function>
	typename FunctionTraits<Function>::stlFunctionType toFunction(const Function&& lambda)
	{
		return static_cast<FunctionTraits<Function>::stlFunctionType>(std::forward<Function>(lambda));
	}

	template <typename Function>
	typename FunctionTraits<Function>::Pointer toFunctionPointer(const Function& lambda)
	{
		return static_cast<typename FunctionTraits<Function>::Pointer>(lambda);
	}
}