#pragma once
#include "core.h"
#include "chart.h"
#include "gramparser.h"
#include <list>

namespace alg {

	// sorted storage 
	template<typename T, int N>
	class CSortedStorage
	{
		// chosen type of storage has quick sort & quick acces with keys array
		std::unordered_map<int, std::vector<T*>> m_Storage;
		// array helper conteins all storage keys in decreasing order
		std::vector<int> m_Keys;

		//std::shared_ptr<CSortedStorage> m_pBase;
		std::shared_ptr<CChart<T>> m_pChart; // gram chart
		//T* m_pCurGram;

		CSortedStorage() /*: m_pCurGram(nullptr)*/ {};

	public:
		CSortedStorage(std::shared_ptr<CChart<T>> pChart) 
			: /*m_pCurGram(nullptr),*/ m_pChart(pChart) {}
		//CSortedStorage(std::shared_ptr<CSortedStorage> pBase) 
		//	: m_pCurGram(nullptr), m_pBase(pBase), m_pChart(pBase->getchart()) {};

		int getgramsize() { return N; }

		std::unordered_map<int, std::vector<T*>>& getgrams() { return m_Storage; };

		const std::shared_ptr<CChart<T>> getchart() { return m_pChart; };
		//const T* getcurgram() { return m_pCurGram; };

		//int GetGramLen() { return m_pBase ? m_pBase->GetGramLen() + N : N; }

		////full_gram - reserved
		//void GetFullGram(std::vector<T> &full_gram) {
		//	if (m_pBase)
		//		m_pBase->GetFullGram(full_gram);

		//	if(m_pCurGram)
		//		for (int i = 0; i < N; i++)
		//			full_gram.push_back(m_pCurGram[i]);
		//}

		// create m_Storage gram freq table from m_grams stored by inclusions number
		// [in]gram_hashtable<N> &table - hashtable
		void StoreTable(gram_hashtable<N> &table) {

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


		//void RestoreGramFilePositions(std::FILE* pFile)
		//{
		//	for (auto &vgram : m_Storage) {
		//		for (auto gram : vgram.second) {


		//		}
		//	}
		//}

	};

	template<int N>
	using CSortedCharStorage = CSortedStorage<char, N>;
};
