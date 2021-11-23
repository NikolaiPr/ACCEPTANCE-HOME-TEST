#pragma once
#include "chart.h"
#include "gramparser.h"
#include <unordered_set>

namespace alg {

	/// sorted gram storage 
	/// fillters less than chart botton frequency grams
	template<typename T, int N>
	class CSortedStorage
	{
		/// chosen type of storage has quick sort & quick acces with keys array
		std::unordered_map<int, std::vector<T*>> m_Storage;

		/// array helper conteins all storage keys in decreasing order
		std::vector<int> m_Keys;

		std::shared_ptr<CChart<T>> m_pChart; ///< gram chart

		CSortedStorage() {}; ///< private constructor

	public:
		CSortedStorage(std::shared_ptr<CChart<T>> pChart) : m_pChart(pChart) {}

		int getgramsize() { return N; }

		std::unordered_map<int, std::vector<T*>>& getgrams() { return m_Storage; };

		const std::shared_ptr<CChart<T>> getchart() { return m_pChart; };


		/// create sorted filtered gram freq table from unsorted grams hashtable
		/// fillters less than chart botton frequency grams
		/// frequency as table key
		/// @param [in]gram_hashtable<T, N> &table - unsorted hashtable
		void StoreTable(gram_hashtable<T, N> &table) {

			if (table.empty())
				return;

			int iChartLimit = 1;
			if (m_pChart.get() != nullptr) {
				iChartLimit = m_pChart->GetBottomFreq();
				if (m_pChart->IsFull()/* || iChartLimit == 0*/)
					iChartLimit++;
			}

			// use hash sorting
			std::unordered_set<int> tmp;
			for (auto itr = table.begin(); itr != table.end(); ) {

				auto &gram = *itr;
				if (gram.second < iChartLimit) { // skip unfrequent grams
					++itr;
					continue;
				}

				m_Storage[gram.second].push_back(gram.first); //std::move(gram.first) don't work on const!!!

				tmp.insert(gram.second);

				itr = table.erase(itr);
			}

			// copy m_Keys (need optimization)
			m_Keys.reserve(tmp.size());
			for (auto i : tmp)
				m_Keys.push_back(i);

			// sort with descending order.
			std::sort(m_Keys.begin(), m_Keys.end(), [&](int &a, int &b) { return a > b; });
		}
	};
};
