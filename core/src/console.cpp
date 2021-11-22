#include <stdio.h>
#include <iostream>
#include <string>
#include <memory>
#include "..\..\core\includes\core.h"

using namespace alg;


//print single gram
//total > 0 - add chart
void print_gram(std::string &str, int freq, int total = 0){
	if(total == 0)
		std::cout << str << " = "<< freq << std::endl;
	else
		std::cout << str << " = "<< (freq*100)/((double)total) << '%'<< std::endl;
}

//print all grams with their inclusion frequency
//template<typename TGram>
//void print_grams(TGram &grams){
//	for(auto &gram : grams.getgrams())
//		print_gram(gram.first, gram.second);
//}

//template<>
std::string GetCombinationString(const std::vector<char> &comb)
{
	std::string str(comb.data(), comb.size());
	return std::move(str);
}

//print grams with their inclusion frequency in descending order
// count == 0 - print all
void print_chart(std::shared_ptr<CCharChart> pChart){
	if (pChart.get() == nullptr || pChart->IsEmpty())
		return;

	auto total = pChart->GetTotal();

	for(auto &gr : pChart->GetChart())
		print_gram(GetCombinationString(gr.m_comb), gr.m_freq, total);
}


int main(int argc, char **file_arg)
{

	CAlgorithm ChartProcessor(std::string("L:\\PROEKSPERT-HOME-TEST\\core_test\\test_files\\twoletters.txt"));

	if (argc > 1)
		ChartProcessor.m_file = std::string(file_arg[1]);
	if (argc > 2)
		ChartProcessor.m_iChartLength = std::atoi(file_arg[3]);
	if (argc > 3)
		ChartProcessor.m_iCombinationLength = std::atoi(file_arg[3]);

	std::cout << "open file:" << ChartProcessor.m_file << std::endl;
	std::cout << "chart length:" << ChartProcessor.m_iChartLength << std::endl << std::endl;
	std::cout << "combination length:" << ChartProcessor.m_iCombinationLength << std::endl << std::endl;


	if(ChartProcessor.Execute() != err_codes::no_err)
		return -1;

	print_chart(ChartProcessor.m_pChart);


	return 0;
}