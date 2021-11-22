#include "..\includes\core.h"
#include "..\includes\gramparser.h"
#include "..\includes\sortedstorage.h"

using namespace alg;

// recursive gram depth first search
//void FindGramDFS(std::shared_ptr<CSortedCharStorage> pBaseStore, std::FILE* pFile, int iCombinationLength);

void FindExpanded(std::shared_ptr<CCharChart> m_pChart, std::FILE* pFile);

// main function
err_codes CAlgorithm::Execute()
{
	if (m_file.empty())
		return err_codes::err_filename;

	if (m_iCombinationLength < MIN_COMBINATION_LEN || m_iCombinationLength > MAX_COMBINATION_LEN)
		return err_codes::err_comblen;

	if (m_iChartLength <= 0 || m_iChartLength > MAX_CHART_LEN)
		return err_codes::err_chartlen;

	std::FILE* pFile = std::fopen(m_file.c_str(), "r");
	if (!pFile)
		return err_codes::err_file;


	try {
		auto pBaseStore = std::make_shared<CSortedCharStorage<N_GRAM>>(m_pChart);
		{
			TGramParser<N_GRAM> BaseTable(pFile);
			BaseTable.FindAllGrams();

			pBaseStore->StoreTable(BaseTable.getgrams());

			for (auto &vgram : pBaseStore->getgrams()) {
				for (auto gram : vgram.second) {
					std::vector<char> full_gram;
					for (int i = 0; i < N_GRAM; i++)
						full_gram.push_back(gram[i]);

					m_pChart->AddCombination({ full_gram, vgram.first });
				}
			}
		}

		FindExpanded(m_pChart, pFile);

		//FindGramDFS(pBaseStore, pFile, m_iCombinationLength);
	}
	catch(...)
	{
		return err_codes::err_unknown;
	}


	std::fclose(pFile);

	return err_codes::no_err;
}


void FindExpanded(std::shared_ptr<CCharChart> m_pChart, std::FILE* pFile)
{
	//int iminfreq = m_pChart->GetBottomFreq();
	auto &chart = m_pChart->GetChart();
	for (auto itr = chart.begin(); itr != chart.end(); ++itr) {

		auto &comb = *itr;

		if (comb.m_checked)
			continue;

		auto pStore = std::make_shared<CSortedCharStorage<1>>(m_pChart);
		{
			TGramParser<1> SubTable(pFile);

			// find expanded gram part
			SubTable.FindAllGramsAfterOne(comb.m_comb.data(), comb.m_comb.size());

			pStore->StoreTable(SubTable.getgrams());
		}
		comb.m_checked = true;

		for (auto &vgram : pStore->getgrams()) {
			for (auto gram : vgram.second) {
				std::vector<char> full_gram(comb.m_comb);
				for (int i = 0; i < pStore->getgramsize(); i++)
					full_gram.push_back(gram[i]);

				m_pChart->AddCombination({ full_gram, vgram.first, false });
				itr = chart.begin();
			}
		}
		if(pStore->getgrams().size() > 0)
			FindExpanded(m_pChart, pFile);	
	}
}


// recursive gram depth first search
/*
void FindGramDFS(std::shared_ptr<CSortedCharStorage> pBaseStore, std::FILE* pFile, int iCombinationLength)
{
	std::vector<char> full_basegram;
	full_basegram.reserve(pBaseStore->GetGramLen());
	pBaseStore->GetFullGram(full_basegram);

	for (auto &vgram : pBaseStore->getgrams()) {
		for (auto gram : vgram.second) {

			std::vector<char> full_gram(full_basegram);
			for (int i = 0; i < N_GRAM; i++)
				full_gram.push_back(gram[i]);

			if (full_gram.size() < iCombinationLength) {

				TGramParser<N_GRAM> SubTable(pFile);
				auto pSubStore = std::make_shared<CSortedCharStorage>(pBaseStore);

				// gram UNcompleted -> find next gram part
				SubTable.FindAllGramsAfterOne(full_gram.data(), full_gram.size(), iCombinationLength);

				pSubStore->StoreTable(SubTable.getgrams());

				// recursive down
				FindGramDFS(pSubStore, pFile, iCombinationLength);
			}
			else { 

				// gram completed -> add in chart
				pBaseStore->getchart()->AddCombination({ full_gram, vgram.first });
			}
		}
	}
}
*/