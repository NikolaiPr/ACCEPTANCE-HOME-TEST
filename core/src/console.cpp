#include <iostream>
#include <string>
#include <memory>
#include "..\..\core\includes\core.h"

#define N_GRAM 3
//#define MIN_FREQ 3
#define CHART_SIZE 10

//print single gram
//total > 0 - add chart
void print_gram(char *gram, unsigned int freq, unsigned int total = 0){
	char str[N_GRAM+1];
	memcpy(&str, gram, sizeof(char)*N_GRAM);
	str[N_GRAM] = '\0';
	if(total == 0)
		std::cout << str << " = "<< freq << std::endl;
	else
		std::cout << str << " = "<< (freq*100)/((double)total) << '%'<< std::endl;
}

//print all grams with their inclusion frequency
template<typename TGram>
void print_grams(TGram &grams){
	for(auto &gram : grams.getgrams())
		print_gram(gram.first, gram.second);
}

//print grams with their inclusion frequency in descending order
// count == 0 - print all
template<typename gr_type>
void print_sorted_grams(std::unordered_map<unsigned int, std::vector<gr_type>> &table, 
						std::vector<unsigned int> &keys, unsigned int count = 0){
	int num = (count > 0) ? count : table.size();
	if(count > table.size())
		count = table.size();

	if(keys.size() == 0)
		return;

	int ikey = 0;
	int total = 0;

	for(int i=0; i<num; ){
		auto key = keys[ikey++];
		for(auto &gram : table[key]){
			total += key;
			if(++i >= num)
				break;
		}
	}

	ikey = 0;
	for(int i=0; i<num; ){
		auto key = keys[ikey++];
		for(auto &gram : table[key]){
			print_gram(gram, key, total);
			if(++i >= num)
				break;
		}
	}
}


int main(int argc, char **file_arg)
{
	std::string file = "L:\\PROEKSPERT-HOME-TEST\\core_test\\test_files\\console.pdb";
	if(argc > 1){
		file.clear();
		file.append(file_arg[1]);
	}
	std::cout << "open file:" << file << std::endl;

	int chartSize = CHART_SIZE;
	if(argc > 2)
		chartSize = std::atoi(file_arg[2]);
	std::cout << "chart length:" << chartSize << std::endl << std::endl;

	TGramConteiner<N_GRAM> grams;

	if(!grams.ParseGram(file))
		return -1;

	std::cout << "all grams:" << std::endl;
	print_grams(grams);

	std::unordered_map<unsigned int, std::vector<char*>> sorted;
	std::vector<unsigned int> keys;
	grams.get_sorted_table(sorted, keys);

	std::cout << std::endl;
	std::cout << "top " << chartSize << " grams"<< std::endl;

	print_sorted_grams(sorted, keys, chartSize);

	return 0;
}