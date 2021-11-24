#include "..\includes\core.h"
#include "..\includes\gramparser.h"
#include "..\includes\sortedstorage.h"
#include <iostream>
#include <string>
#include <wchar.h>

#define CHART_MAX_BARLEN 74

using namespace std;
using namespace alg;


/// explicit template instantiations
/// solve the inctance redefinition problem
/// limits file data types to use only this
CORE_EXTERN template class CORE_EXPORT CAlgorithm<char>;
CORE_EXTERN template class CORE_EXPORT CAlgorithm<wchar_t>;


// open file
std::FILE* AAlgorithm::OpenFile(bool bWrite)
{
	auto format = GetFileFormat(m_file);
	if (!bWrite && format == textfile_format::error_file) {
		cout << "file open error:" << m_file << endl;
		return nullptr;
	}
	std::string file(m_file);
	if(bWrite)
		file += "res";

	std::FILE* pFile = nullptr;
	switch (format) {
	case textfile_format::ascii_file:
		fopen_s(&pFile, file.c_str(), bWrite ? "w" : "r");
		break;
	case textfile_format::utf8_file:
		fopen_s(&pFile, file.c_str(), bWrite ? "w, ccs = UTF-8" : "r, ccs = UTF-8");
		break;
	}

	return pFile;
}


// recursive gram search
template<typename T>
void FindExpanded(std::shared_ptr<CChart<T>> m_pChart, std::FILE* pFile);


/// default constructor
template<typename T>
CAlgorithm<T>::CAlgorithm(){

	m_pChart = std::make_shared<CChart<T>>(m_iChartLength);
}


/// explicit constructor
template<typename T>
CAlgorithm<T>::CAlgorithm(std::string &file, int iCombinationLength = 4, int iChartLength = MAX_CHART_LEN)
	: AAlgorithm(file, iCombinationLength, iChartLength) {

	m_pChart = std::make_shared<CChart<T>>(iChartLength);
}

// main function
template<typename T>
err_codes CAlgorithm<T>::Execute()
{
	if (m_file.empty())
		return err_codes::err_filename;

	//if (m_iCombinationLength < MIN_COMBINATION_LEN)
	//	return err_codes::err_comblen;

	if (m_iChartLength <= 0 || m_iChartLength > MAX_CHART_LEN)
		return err_codes::err_chartlen;

	auto pFile = OpenFile(false);
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
	}
	catch(...)
	{
		return err_codes::err_unknown;
	}


	std::fclose(pFile);

	return err_codes::no_err;
}


// finds expanded combinations in file
// get combination from chart, searchs in file combination len+1, 
// place back in chart most frequent combination
// @param pFile the same already oppened file
template<typename T>
void CAlgorithm<T>::FindExpanded(std::FILE* pFile)
{
	auto &chart = m_pChart->GetCombinations();
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


// print results into console
// suports only char type
template<typename T>
void CAlgorithm<T>::PrintResults()
{
	if (GetFileFormat(m_file) != textfile_format::ascii_file) { // console is only for ascii now
		cout << "processed unicode file: " << m_file << endl;
		cout << "see rusults in: " << m_file << "res" <<endl;
		return;
	}
	cout << "processed file: " << m_file << endl;

	// print header
	cout << "chart max length:" << m_iChartLength << endl;
	//cout << "min combination length:" << m_iCombinationLength << endl << endl;

	// print table
	cout << "Result table ------------------------------------------------------------------" << endl;
	auto chart = GetChart();
	auto total = chart->GetTotal();
	int ipos = 0;

	for (auto comb : chart->GetCombinations()) {
		T *buf = nullptr;
		int len = chart->GetCombinationString(comb, buf);
		buf[len] = '\0';

		cout << ipos++ << ". " << buf << " | " << (comb.m_freq * 100) / ((double)total) << '%' << endl;
		//cout << ipos++ << str << " = " << comb.m_freq << endl;

		delete[] buf;
	}

	//print bar chart
	cout << "Bar chart ---------------------------------------------------------------------" << endl;
	ipos = 0;
	for (auto comb : chart->GetCombinations()) {
		auto len = (int)(comb.m_freq * CHART_MAX_BARLEN) / ((double)total);
		cout << ipos++ << ". ";
		for (int f = 0; f < len; f++)
			cout << "*";

		cout << endl;
	}
}

// save results into file
err_codes CAlgorithm<char>::SaveResults()
{
	auto pFile = OpenFile(true);
	if (!pFile)
		return err_codes::err_file;

	// print table
	fwrite("Result table\n", sizeof(char), 13, pFile);
	auto chart = GetChart();
	auto total = chart->GetTotal();
	int ipos = 0;

	for (auto comb : chart->GetCombinations()) {

		char cbuf[20];
		int len =  sprintf_s(cbuf, 20, "%i. ", ipos++);

		fwrite(cbuf, sizeof(char), len, pFile);

		char *buf = nullptr;
		len = chart->GetCombinationString(comb, buf);
		fwrite(buf, sizeof(char), len, pFile);

		len = sprintf_s(cbuf, 20, " | %.3f", (comb.m_freq * 100) / ((double)total));
		fwrite(cbuf, sizeof(char), len, pFile);
		fwrite("%\n", sizeof(char), 2, pFile);

		delete[] buf;
	}

	//print bar chart
	fwrite("Bar chart\n", sizeof(char), 10, pFile);
	ipos = 0;
	for (auto comb : chart->GetCombinations()) {
		
		char cbuf[20];
		auto len = sprintf_s(cbuf, 20, "%i. ", ipos++);
		fwrite(cbuf, sizeof(char), len, pFile);

		len = (int)((comb.m_freq * CHART_MAX_BARLEN) / ((double)total));
		char *buf = new char[len];
		for (int f = 0; f < len; f++)
			buf[f] = '*';

		fwrite(buf, sizeof(char), len, pFile);
		fwrite("\n", sizeof(char), 1, pFile);
		delete[] buf;
	}

	std::fclose(pFile);

	return err_codes::no_err;
}

// save results into file
err_codes CAlgorithm<wchar_t>::SaveResults()
{
	auto pFile = OpenFile(true);
	if (!pFile)
		return err_codes::err_file;

	// print table
	fwrite(L"Result table\n", sizeof(wchar_t), 13, pFile);
	auto chart = GetChart();
	auto total = chart->GetTotal();
	int ipos = 0;

	for (auto comb : chart->GetCombinations()) {

		wchar_t cbuf[20];
		int len = swprintf(cbuf, 20, L"%i. ", ipos++);

		fwrite(cbuf, sizeof(wchar_t), len, pFile);

		wchar_t *buf = nullptr;
		len = chart->GetCombinationString(comb, buf);
		fwrite(buf, sizeof(wchar_t), len, pFile);

		len = swprintf(cbuf, 20, L" | %.3f", (comb.m_freq * 100) / ((double)total));
		fwrite(cbuf, sizeof(wchar_t), len, pFile);
		fwrite(L"%\n", sizeof(wchar_t), 2, pFile);

		delete[] buf;
	}

	//print bar chart
	fwrite(L"Bar chart\n", sizeof(wchar_t), 10, pFile);
	ipos = 0;
	for (auto comb : chart->GetCombinations()) {

		wchar_t cbuf[20];
		auto len = swprintf(cbuf, 20, L"%i. ", ipos++);
		fwrite(cbuf, sizeof(wchar_t), len, pFile);

		len = (int)((comb.m_freq * CHART_MAX_BARLEN) / ((double)total));
		wchar_t *buf = new wchar_t[len];
		for (int f = 0; f < len; f++)
			buf[f] = '*';

		fwrite(buf, sizeof(wchar_t), len, pFile);
		fwrite(L"\n", sizeof(wchar_t), 1, pFile);
		delete[] buf;
	}

	std::fclose(pFile);

	return err_codes::no_err;
}