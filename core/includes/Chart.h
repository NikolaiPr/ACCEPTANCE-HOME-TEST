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
	};


	/// chart class template for storing elements type of T with their frequency value
	template<typename T>
	class CORE_EXPORT CChart
	{
		friend CChart;
		std::list<chart_item<T>> m_chart;	/// chart conteiner
		char m_iLength;						/// max items number

	public:
		CChart(char iLength = 0) : m_iLength(iLength) {};

		std::list<chart_item<T>>& GetCombinations() { return m_chart; }

		const char GetMaxLength() { return m_iLength; }

		/// clear chart
		void Clear();

		/// @return bottom item frequency
		int GetBottomFreq();

		/// @return min frequency to add in chart
		int GetAcceptFreq();

		/// check is list full
		bool IsFull();

		/// check is list empty
		bool IsEmpty();

		/// tries to add combination
		/// @param comb new combination
		bool AddCombination(const chart_item<T> &comb);

		/// force combination insert with increasing list length
		/// use mostly on loadresults
		/// @param comb new combination
		int ForceInsert(const chart_item<T> &comb);

		/// @return all chart items frequency sum
		int GetTotal();

		/// converts combination of eny type into string
		/// should be explicitly instantiate 
		/// @return buf length
		int GetCombinationString(const chart_item<T> &comb, T*& buf);


		bool operator!=(const CChart<T> &chart);
	};

};
