#include <stdio.h>
#include <iostream>
#include <string>
#include <memory>
#include "..\..\core\includes\core.h"

using namespace std;
using namespace alg;


int main(int argc, char **file_arg)
{
	string file = "L:\\PROEKSPERT-HOME-TEST\\core_test\\test_files\\large8.txt";

	if (argc > 1)
		file = string(file_arg[1]);
	auto format = GetFileFormat(file);
	if (format == textfile_format::error_file) {
		cout << "file error:" << file << endl;
		return -1;
	}


	shared_ptr<AAlgorithm> ChartProcessor;

	switch (format) {
	case textfile_format::ascii_file:
		ChartProcessor = make_shared<CAlgorithm<char>>(file);
		break;
	case textfile_format::utf8_file:
		ChartProcessor = make_shared<CAlgorithm<wchar_t>>(file);
		break;
	}


	if (argc > 2)
		ChartProcessor->m_iChartLength = atoi(file_arg[2]);
	//if (argc > 3)
	//	ChartProcessor->m_iCombinationLength = atoi(file_arg[3]);


	if(ChartProcessor->Execute() != err_codes::no_err)
		return -1;
	

	ChartProcessor->PrintResults();

	ChartProcessor->SaveResults();

	return 0;
}