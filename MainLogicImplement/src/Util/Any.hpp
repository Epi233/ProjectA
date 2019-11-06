#pragma once

#include <memory>
#include <typeindex>
#include <string>

namespace ProjectA
{
	class Any
	{
	public:
		Any()
			: _typeIndex(std::type_index(typeid(void)))
		{
		}

		Any(Any& rhs)
			: _ptr(rhs.clone())
			, _typeIndex(rhs._typeIndex)
		{
		}

		Any(Any&& rhs) noexcept
			: _ptr(std::move(rhs._ptr))
			, _typeIndex(rhs._typeIndex)
		{
		}

		template <typename U, class = typename std::enable_if<!std::is_same<typename std::decay<U>::type, Any>::value, U>::type>
		Any(U&& v)
			: _ptr(new Derived<typename std::decay<U>::type>(std::forward<U>(v)))
			, _typeIndex(std::type_index(type_id(typename std::decay<U>::type)))
		{
		}

	public:
		bool isNull() const
		{
			return !bool(_ptr);
		}

		template <class U>
		bool is() const
		{
			return _typeIndex == std::type_index(typeid(U));
		}

		template <class U>
		U& anyCast()
		{
			if (!is<U>)
			{
				// std::string str = "Can not cast " + std::string(typeid(U).name()) + " to " + std::string(_typeIndex.name());
				throw std::bad_cast();
			}
			auto derived = dynamic_cast<Derived<U>*> (_ptr.get());
			return derived->value;
		}

		Any& operator= (const Any& rhs)
		{
			if (_ptr == rhs._ptr)
				return *this;

			_ptr = rhs.clone();
			_typeIndex = rhs._typeIndex;
			return *this;
		}

	private:

		struct Base
		{
			virtual ~Base() = default;
			virtual std::unique_ptr<Base> clone() const = 0;
		};

		template <typename T>
		struct Derived : Base
		{
			T value;

			template <typename U>
			Derived(U&& v)
				: value(std::forward<U>(v))
			{
			}

			std::unique_ptr<Base> clone() const override
			{
				return std::unique_ptr<Base>(std::make_unique<Derived<T>>(value));
			}
		};

		std::unique_ptr<Base> clone() const
		{
			if (_ptr != nullptr)
				return _ptr->clone();
			return nullptr;
		}

		std::unique_ptr<Base> _ptr;
		std::type_index _typeIndex;

	};
}