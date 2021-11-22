#pragma once
#include <list>
#include <string>
#include <vector>
#include "..\includes\dll_exports.h"

namespace alg {

	template<typename T>
	struct chart_item
	{
		std::vector<T> m_comb;
		int m_freq;
		bool m_checked;
	};

	//chart class template for storing elements type of T with their frequency value
	template<typename T>
	class CChart
	{
		std::list<chart_item<T>> m_chart;
		char m_iLength;

		CChart();
	public:
		CChart(char iLength) : m_iLength(iLength) {};

		std::list<chart_item<T>>& GetChart() { return m_chart; }
		const char GetMaxLength() { return m_iLength; }


		// returns bottom item frequency
		int GetBottomFreq()
		{
			if (m_chart.empty())
				return 0;
			return m_chart.back().m_freq;
		}

		// check is list full
		bool IsFull()
		{
			return (m_chart.size() == m_iLength);
		}

		// check is list empty
		bool IsEmpty()
		{
			return m_chart.empty();
		}

		// tries to add combination
		bool AddCombination(const chart_item<T> &comb)
		{
			if (IsFull() && GetBottomFreq() >= comb.m_freq)
				return false;

			// looking for place in chart by freq -> add
			for (auto itr = m_chart.begin(); itr != m_chart.end(); ++itr)
				if (itr->m_freq < comb.m_freq) {
					m_chart.insert(itr, comb);
					return true;
				}


			// combination has lesser freq, but still there is free place in chart -> add
			m_chart.push_back(comb);
			return true;
		}

		int GetTotal()
		{
			int total = 0;

			for (auto &item : m_chart)
				total += item.m_freq;

			return total;
		}

	};


	template<typename T>
	std::string GetCombinationString(const std::vector<T> &comb) { return ""; }

	using CCharChart = CChart<char>;
};
