#pragma once

#include <vector>
#include <string>
#include <chrono>
#include <random>
#include <cstdint>
#include <numeric>

using std::vector;
using std::string;

namespace ProjectA
{
	template <class Typename>
	class Singleton
	{
	public:
		Singleton(const Singleton& b_) = delete;
		Singleton(Singleton&& b_) = delete;
		Singleton& operator=(const Singleton& b_) = delete;
		Singleton& operator=(Singleton&& b_) = delete;
	private:
		class Garbo final
		{
		public:
			Garbo() = default;
			Garbo(const Garbo& b_) = delete;
			Garbo(Garbo&& b_) = delete;
			Garbo& operator=(const Garbo& b_) = delete;
			Garbo& operator=(Garbo&& b_) = delete;

			~Garbo()
			{
				if (_instance)
				{
					delete _instance;
					_instance = nullptr;
				}
			}
		};

		static Typename* _instance;
		static Garbo _garbo;

	protected:
		Singleton() = default;
		~Singleton() = default;

	public:
		static Typename* getInstance()
		{
			if (_instance == nullptr)
				_instance = new Typename();
			return _instance;
		}
	};

	template <class Typename>
	Typename* Singleton<Typename>::_instance = nullptr;

	class NonCopyable
	{
	protected:
		NonCopyable() = default;
		~NonCopyable() = default;
		NonCopyable(const NonCopyable& rhs) = delete;
		NonCopyable& operator= (const NonCopyable& rhs) = delete;
	};

	class NonMovable
	{
	protected:
		NonMovable() = default;
		~NonMovable() = default;
		NonMovable(NonMovable&& rhs) = delete;
		NonMovable& operator= (NonMovable&& rhs) = delete;
	};

	class Util final
	{
	public:

		/** generate a uniform random number in [a, b] */
		template<typename IntegerType>
		static IntegerType uRandom(IntegerType a_, IntegerType b_)
		{
			const auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
			std::default_random_engine e(seed);
			const std::uniform_int_distribution<IntegerType> u(a_, b_);
			return u(e);
		}

		/** calculate standard deviation */
		template<typename Type>
		static Type standardDeviation(const vector<Type> vec_)
		{
			Type sum = std::accumulate(vec_.begin(), vec_.end(), static_cast<Type>(0));
			Type mean = sum / vec_.size();
			Type accum = static_cast<Type>(0);
			std::for_each(vec_.begin(), vec_.end(), [&](Type num_)
			{
				accum += (num_ - mean) * (num_ - mean);
			});
			return std::sqrt(accum / vec_.size());
		}

		/** split string */
		static vector<string> splitString(const string& s_, const string& c_)
		{
			vector<string> v;
			auto pos2 = s_.find(c_);
			string::size_type pos1 = 0;

			while (string::npos != pos2)
			{
				v.push_back(s_.substr(pos1, pos2 - pos1));

				pos1 = pos2 + c_.size();
				pos2 = s_.find(c_, pos1);
			}

			if (pos1 != s_.length())
				v.push_back(s_.substr(pos1));

			return v;
		}

		static void deleteAllMark(string& s_, const string& mark_)
		{
			const auto n_size = mark_.size();

			while (true)
			{
				const auto pos = s_.find(mark_);
				if (pos == string::npos)
					return;

				s_.erase(pos, n_size);
			}
		}
	};

}