#include <stdio.h>
#include <iostream>
#include <string>
#include <memory>
#include "..\..\core\includes\core.h"

using namespace alg;

int main(int argc, char **file_arg)
{
	//std::string file = "L:\\PROEKSPERT-HOME-TEST\\core_test\\test_files\\default.txt"; //unicide_wchar
	std::string file = "L:\\PROEKSPERT-HOME-TEST\\core_test\\test_files\\step3.txt"; //unicide_wchar

	
	if (argc > 1)
		file = std::string(file_arg[1]);
	if (GetFileFormat(file) != textfile_format::ascii_file) // console can print ascii text only
		return -1;

	CAlgorithm<char> ChartProcessor(file);


	if (argc > 2)
		ChartProcessor.m_iChartLength = std::atoi(file_arg[3]);
	if (argc > 3)
		ChartProcessor.m_iCombinationLength = std::atoi(file_arg[3]);

	std::cout << "open file:" << ChartProcessor.m_file << std::endl;
	std::cout << "chart length:" << ChartProcessor.m_iChartLength << std::endl << std::endl;
	std::cout << "combination length:" << ChartProcessor.m_iCombinationLength << std::endl << std::endl;




	if(ChartProcessor.Execute() != err_codes::no_err)
		return -1;


	// print chart
	auto chart = ChartProcessor.GetChart();
	auto total = chart->GetTotal();
	for (auto &gr : chart->GetChart()) {
		std::string str = chart->GetCombinationString(gr.m_comb);

		std::cout << str << " = " << (gr.m_freq * 100) / ((double)total) << '%' << std::endl;
		//std::cout << str << " = " << gr.m_freq << std::endl;
	}

	return 0;
}