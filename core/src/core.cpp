#include "..\includes\core.h"
#include "..\includes\gramparser.h"
#include "..\includes\sortedstorage.h"

using namespace alg;


/// explicit template instantiations
/// solve the inctance redefinition problem
/// limits file data types to use only this
CORE_EXTERN template class CORE_EXPORT CAlgorithm<char>;
CORE_EXTERN template class CORE_EXPORT CAlgorithm<wchar_t>;


// recursive gram search
template<typename T>
void FindExpanded(std::shared_ptr<CChart<T>> m_pChart, std::FILE* pFile);


/// default constructor
template<typename T>
CAlgorithm<T>::CAlgorithm() : m_iCombinationLength(4), m_iChartLength(MAX_CHART_LEN) {
	m_pChart = std::make_shared<CChart<T>>(m_iChartLength);
}


/// explicit constructor
template<typename T>
CAlgorithm<T>::CAlgorithm(std::string &file, int iCombinationLength = 4, int iChartLength = MAX_CHART_LEN)
	: m_file(file), m_iCombinationLength(iCombinationLength), m_iChartLength(iChartLength) {

	m_pChart = std::make_shared<CChart<T>>(iChartLength);
}


// main function
template<typename T>
err_codes CAlgorithm<T>::Execute()
{
	if (m_file.empty())
		return err_codes::err_filename;

	if (m_iCombinationLength < MIN_COMBINATION_LEN)
		return err_codes::err_comblen;

	if (m_iChartLength <= 0 || m_iChartLength > MAX_CHART_LEN)
		return err_codes::err_chartlen;

	std::FILE* pFile = std::fopen(m_file.c_str(), "r");
	if (!pFile)
		return err_codes::err_file;


	try {
		auto pBaseStore = std::make_shared<CSortedStorage<T, N_GRAM>>(m_pChart);
		{
			TGramParser<T, N_GRAM> BaseTable(pFile);
			BaseTable.FindAllGrams();

			pBaseStore->StoreTable(BaseTable.getgrams());

			for (auto &vgram : pBaseStore->getgrams()) { // vgram.first - frequency, vgram.second - T* gram
				for (auto gram : vgram.second) {
					std::vector<T> full_gram;
					for (int i = 0; i < BaseTable.getgramsize(); i++)
						full_gram.push_back(gram[i]); // copy T* -> vector<T>

					m_pChart->AddCombination({ full_gram, vgram.first });
				}
			}
		}

		FindExpanded(pFile);

		//FindGramDFS(pBaseStore, pFile, m_iCombinationLength);
	}
	catch(...)
	{
		return err_codes::err_unknown;
	}


	std::fclose(pFile);

	return err_codes::no_err;
}


template<typename T>
void CAlgorithm<T>::FindExpanded(std::FILE* pFile)
{
	auto &chart = m_pChart->GetChart();
	for (auto itr = chart.begin(); itr != chart.end(); ++itr) {

		auto &comb = *itr;

		if (comb.m_checked)
			continue;

		auto pStore = std::make_shared<CSortedStorage<T, 1>>(m_pChart);
		{
			TGramParser<T, 1> SubTable(pFile);

			// find expanded gram part
			SubTable.FindAllGramsAfterOne(comb.m_comb.data(), comb.m_comb.size());

			pStore->StoreTable(SubTable.getgrams());
		}
		comb.m_checked = true;

		for (auto &vgram : pStore->getgrams()) { // vgram.first - frequency, vgram.second - T* gram
			for (auto gram : vgram.second) {
				std::vector<T> full_gram(comb.m_comb);
				for (int i = 0; i < pStore->getgramsize(); i++)
					full_gram.push_back(gram[i]); // copy T* -> vector<T>

				m_pChart->AddCombination({ full_gram, vgram.first, false });
				itr = chart.begin();
			}
		}
		if(pStore->getgrams().size() > 0)
			FindExpanded(pFile);	
	}
}

