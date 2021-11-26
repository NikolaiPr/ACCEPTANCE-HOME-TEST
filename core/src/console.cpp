#include <stdio.h>
#include <iostream>
#include <string>
#include <memory>
#include "..\..\core\includes\core.h"

using namespace std;
using namespace alg;


int main(int argc, char **file_arg)
{
	string file;

	if (argc > 1)
		file = string(file_arg[1]);
	auto format = GetFileFormat(file);
	if (format == textfile_format::error_file) {
		cout << "file error:" << file << endl;
		return -1;
	}


	unique_ptr<AAlgorithm> ChartProcessor;

	switch (format) {
	case textfile_format::ascii_file:
		ChartProcessor = make_unique<CAlgorithm<char>>(file);
		break;
	case textfile_format::utf8_file:
		ChartProcessor = make_unique<CAlgorithm<wchar_t>>(file);
		break;
	}


	if (argc > 2)
		ChartProcessor->m_iChartLength = atoi(file_arg[2]);

	bool bPercentage = false;
	if (argc > 3 && (file_arg[3][0] == '%'))
		bPercentage = true;



	if(ChartProcessor->Execute() != err_codes::no_err)
		return -1;


	ChartProcessor->PrintResults(bPercentage);

	ChartProcessor->SaveResults();

	return 0;
}