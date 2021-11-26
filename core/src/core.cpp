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
CAlgorithm<T>::CAlgorithm(const std::string &file, int iCombinationLength = 4, int iChartLength = MAX_CHART_LEN)
	: AAlgorithm(file, iCombinationLength, iChartLength) {

	m_pChart = std::make_shared<CChart<T>>(iChartLength);
}

// main function
template<typename T>
err_codes CAlgorithm<T>::Execute(bool bTestMode)
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

	std::unique_ptr<CChart<T>> TestChart;
	if (bTestMode)
		TestChart = make_unique<CChart<T>>(*m_pChart.get());
	
	m_pChart->Clear();

	try {
		auto pBaseStore = std::make_shared<CSortedStorage<T, N_GRAM>>(m_pChart);
		{
			TGramParser<T, N_GRAM> BaseTable(pFile);
			BaseTable.FindAllGrams();

			pBaseStore->StoreTable(BaseTable.getgrams());

			for (auto &vgram : pBaseStore->getgrams()) { // vgram.first - frequency, vgram.second - T* gram
				for (auto gram : vgram.second) {
					std::vector<T> full_gram;
					for (int i = 0; i < N_GRAM; i++)
						full_gram.push_back(gram[i]); // copy T* -> vector<T>

					m_pChart->AddCombination({ full_gram, vgram.first });
				}
			}
		}

		int iExpCombLen = N_GRAM;
		while (FindExpanded(pFile, iExpCombLen++));
	}
	catch(...)
	{
		return err_codes::err_unknown;
	}

	std::fclose(pFile);

	if (bTestMode && *m_pChart.get() != *TestChart.get())
		return err_codes::err_testfailed;

	return err_codes::no_err;
}


// finds expanded combinations in file
// get combination from chart, searchs in file combination len+1, 
// place back in chart most frequent combination
// @param pFile the same already oppened file
// @iExpCombLen combination length to expand
template<typename T>
bool CAlgorithm<T>::FindExpanded(std::FILE* pFile, int iExpCombLen)
{
	std::fseek(pFile, 0, SEEK_SET);

	std::list<chart_item<T>> &chart = m_pChart->GetCombinations();

	std::vector<std::unordered_map<T, int>> vcHash; // one char tables for each combination

	auto buf = new T[iExpCombLen + 1];
	memset(buf, 0, sizeof(T) * (iExpCombLen + 1));
	auto *ch = buf + iExpCombLen; // last buffer char
	int skip = iExpCombLen + 1;

	while (readsymb(pFile, *ch)) {

		// chech separators
		if (IsSeparator(*ch))
			skip = iExpCombLen + 1; // find separator, skip comparision for MIN_COMBINATION_LEN steps
	
		if (skip > 0) // skipping separators
			skip--;
		else { 
			// compare each combination in chart with buffer
			int icomb = 0;
			for (auto itr = chart.begin(); itr != chart.end(); ++itr) {

				auto &comb = itr->m_comb;
				if (comb.size() != iExpCombLen)
					continue;

				if (icomb >= vcHash.size())
					vcHash.push_back({});
				auto &hash = vcHash[icomb];

				if (memcmp(comb.data(), buf, sizeof(T)*iExpCombLen) == 0) {

					auto itr = hash.find(*ch);
					if (itr == hash.end())
						hash.emplace_hint(itr, *ch, 1);
					else
						itr->second++;
				}
				icomb++;
			}
		}

		// shift preview gram copy on 1 char(or T) left, add new char(or T) to gram end
		memcpy(buf, &buf[1], sizeof(T)*iExpCombLen);
	};

	delete[] buf;
	if (vcHash.empty())
		return false;

	bool bAdd = false;
	auto chart_copy = chart;
	int iChartFreqLimit = m_pChart->GetAcceptFreq();

	int icomb = 0; // could optimize without chart_copy
	for (auto itr_comb = chart_copy.begin(); itr_comb != chart_copy.end(); itr_comb++) {

		auto comb_copy = itr_comb->m_comb;
		auto comblen = comb_copy.size();
		if (comblen != iExpCombLen)
			continue;

		comb_copy.push_back(' ');

		auto &hash = vcHash[icomb];// hash.first - char, hash.second - frequency
	
		for (auto itr = hash.begin(); itr != hash.end(); ++itr) {

			if (itr->second < iChartFreqLimit) // skip unfrequent grams
				continue;

			comb_copy[comblen] = itr->first; // add last char to combination

			if (m_pChart->AddCombination({ comb_copy, itr->second })) {
				iChartFreqLimit = m_pChart->GetAcceptFreq();
				bAdd = true;
			}			
		}	
		icomb++;
	}
	
	return bAdd;
}


// print results into console
// suports only char type
template<typename T>
void CAlgorithm<T>::PrintResults(bool bPercentage)
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

		if(bPercentage)
			cout << ipos++ << ". " << buf << " | " << (comb.m_freq * 100) / ((double)total) << '%' << endl;
		else
			cout << ipos++ << ". " << buf << " | " << comb.m_freq << endl;

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

// load results into file
err_codes CAlgorithm<char>::LoadResults(const std::string &file)
{
	auto chart = GetChart();
	chart->Clear();

	auto file_copy = m_file;
	m_file = file;
	std::FILE *pFile = nullptr;

	pFile = OpenFile(false);
	if (!pFile) {
		m_file = file_copy;
		return err_codes::err_file;
	}

	try {
		// read table
		char buf[MIN_COMBINATION_LEN * MAX_CHART_LEN];
		fgets(buf, MIN_COMBINATION_LEN * MAX_CHART_LEN, pFile);
		std::string str(buf);

		int pos, pos2;	
		pos = str.find('=');
		if (pos == string::npos)
			throw err_codes::err_unknown;
		pos2 = str.find(')');
		if (pos == string::npos)
			throw err_codes::err_unknown;

		auto total = atoi(str.substr(pos+1, pos2 - 1).c_str());

		int icomb = 0;
		do
		{
			fgets(buf, MIN_COMBINATION_LEN * MAX_CHART_LEN, pFile);
			str = std::string(buf);

			pos = str.find('\t');
			if (pos == string::npos)
				throw err_codes::no_err;
			pos2 = str.find('|');
			if (pos == string::npos)
				throw err_codes::no_err;

			if (atoi(str.substr(0, pos - 1).c_str()) == 0)
				break;

			auto comb = str.substr(pos + 1, pos2 - pos - 2);
			int freq = (int)((atof(str.substr(pos2 + 1).c_str()) / 100.)*total + 0.5);
			std::vector<char> vcmb(comb.begin(), comb.end());

			chart->ForceInsert({ vcmb, freq });

		} while (icomb++ < 100); // safe exit
	}
	catch (err_codes &e)
	{
		if (pFile)
			std::fclose(pFile);
		m_file = file_copy;
		return e;
	};

	std::fclose(pFile);
	m_file = file_copy;
	return err_codes::no_err;
}

// load results into file
//TODO
err_codes CAlgorithm<wchar_t>::LoadResults(const std::string &file)
{
	return err_codes::err_unknown;
}


// save results into file
err_codes CAlgorithm<char>::SaveResults()
{
	auto pFile = OpenFile(true);
	if (!pFile)
		return err_codes::err_file;

	auto chart = GetChart();
	auto total = chart->GetTotal();
	char cbuf[20];

	// print table
	fwrite("Result table ", sizeof(char), 13, pFile);
	int len = sprintf_s(cbuf, 20, "(total=%i)\n", total);
	fwrite(cbuf, sizeof(char), len, pFile);

	int ipos = 0;

	for (auto comb : chart->GetCombinations()) {

		len =  sprintf_s(cbuf, 20, "%i.\t", ++ipos);
		fwrite(cbuf, sizeof(char), len, pFile);

		char *buf = nullptr;
		len = chart->GetCombinationString(comb, buf);
		fwrite(buf, sizeof(char), len, pFile);
		delete[] buf;

		len = sprintf_s(cbuf, 20, " | %.3f", (comb.m_freq * 100) / ((double)total));
		fwrite(cbuf, sizeof(char), len, pFile);
		fwrite("%\n", sizeof(char), 2, pFile);	
	}

	//print bar chart
	fwrite("Bar chart\n", sizeof(char), 10, pFile);
	ipos = 0;
	for (auto comb : chart->GetCombinations()) {
		
		char cbuf[20];
		auto len = sprintf_s(cbuf, 20, "%i.\t", ++ipos);
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

	auto chart = GetChart();
	auto total = chart->GetTotal();
	wchar_t cbuf[20];

	// print table
	fwrite(L"Result table ", sizeof(wchar_t), 13, pFile);
	int len = swprintf(cbuf, 20, L"(total=%i)\n", total);
	fwrite(cbuf, sizeof(char), len, pFile);
	int ipos = 0;

	for (auto comb : chart->GetCombinations()) {

		len = swprintf(cbuf, 20, L"%i.\t", ++ipos);
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
		auto len = swprintf(cbuf, 20, L"%i.\t", ++ipos);
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