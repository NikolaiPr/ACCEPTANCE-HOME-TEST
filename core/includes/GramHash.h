/** @file  gramhash.h */
#pragma once
#include <unordered_map>
#include <unordered_set>
#include <algorithm>

namespace alg {

	/// @fn inline bool gram_cmp(const _Tp* __x, const _Tp* __y, int N)
	/// template comparision functions
	template <class _Tp>
	inline bool gram_cmp(const _Tp* __x, const _Tp* __y, int N) noexcept
	{
		return false;
	}

	/// @fn inline bool gram_cmp<char>(const char* __x, const char* __y, int N)
	/// explicit char comparision function
	template<>
	inline bool gram_cmp<char>(const char* __x, const char* __y, int N) noexcept
	{
		return strncmp(__x, __y, N) == 0;
	}


	/// @fn inline bool gram_cmp<wchar_t>(const wchar_t* __x, const wchar_t* __y, int N)
	/// explicit wchar_t comparision function
	template<>
	inline bool gram_cmp<wchar_t>(const wchar_t* __x, const wchar_t* __y, int N) noexcept
	{
		return wcsncmp(__x, __y, N) == 0;
	}


	/// @defgroup hash_functors Custom hash table functors
	/// Custom functors for std::unordered_map<T*, int>

	/// @struct my_equal_to
	/// @ingroup hash_functors
	/// @brief Comparator functor.
	template <int N, class _Tp>
	struct my_equal_to //: public std::equal_to<_Tp*>
	{
		//static_assert(N != 1 && N != 4, "N != 1 && N != 4");
		bool operator()(const char* __x, const char* __y) const noexcept
		{
			return gram_cmp(__x, __y, N);
		}

		bool operator()(const wchar_t* __x, const wchar_t* __y) const noexcept
		{
			return gram_cmp(__x, __y, N);
		}
	};


	/// @struct my_HashChar
	/// @ingroup hash_functors
	/// @brief Custom hash functor.
	template <int N, class _Tp>
	struct my_HashChar //: public std::hash<_Tp>
	{
		//static_assert(N != 1 && N != 4, "N != 1 && N != 4");
		size_t operator()(const char* c_str) const noexcept {
			size_t hash = 0;
			int c = 0;
			char* str = (char*)c_str;

			while (c++ < N)
				hash = *(str++) + (hash << 6) + (hash << 16) - hash;
			return hash;
		}

		size_t operator()(const wchar_t* c_str) const noexcept {
			size_t hash = 0;
			int c = 0;
			wchar_t* str = (wchar_t*)c_str;

			while (c++ < N)
				hash = *(str++) + (hash << 6) + (hash << 16) - hash;
			return hash;
		}
	};


	/// @typedef gram_hashtable
	/// main gram hash table conteiner
	template<typename T, int N>
	using gram_hashtable = std::unordered_map<T*, int, my_HashChar<N, T*>, my_equal_to<N, T*> >;
};