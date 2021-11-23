#pragma once
#include <list>
#include <string>
#include <vector>
#include "..\includes\dll_exports.h"

namespace alg {

	/// chart inem structure template
	template<typename T>
	struct chart_item
	{
		std::vector<T> m_comb;	///< combination container
		int m_freq;				///< combination frequency
		bool m_checked;			///< internal check flag
	};


	/// chart class template for storing elements type of T with their frequency value
	template<typename T>
	class CChart
	{
		std::list<chart_item<T>> m_chart;	/// chart conteiner
		char m_iLength;						/// max items number

		CChart(); /// private constructor

	public:
		CChart(char iLength) : m_iLength(iLength) {};

		std::list<chart_item<T>>& GetCombinations() { return m_chart; }

		const char GetMaxLength() { return m_iLength; }


		/// @return bottom item frequency
		int GetBottomFreq();

		/// check is list full
		bool IsFull();

		/// check is list empty
		bool IsEmpty();

		/// tries to add combination
		/// @param comb new combination
		bool AddCombination(const chart_item<T> &comb);

		/// @return all chart items frequency sum
		int GetTotal();

		/// converts combination of eny type into string
		/// should be explicitly instantiate 
		/// @return buf length
		int GetCombinationString(const chart_item<T> &comb, T*& buf);
	};

};
