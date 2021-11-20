#include <iostream>
#include <string>
#include <memory>
#include "..\includes\core.h"

#define N_GRAM 3
#define MIN_FREQ 3
#define CHART_SIZE 5

void print_gram(char *gram, unsigned int freq, unsigned int total = 0){
	char str[N_GRAM+1];
	memcpy(&str, gram, sizeof(char)*N_GRAM);
	str[N_GRAM] = '\0';
	if(total == 0)
		std::cout << str << " = "<< freq << std::endl;
	else
		std::cout << str << " = "<< freq << (freq*100)/((double)total) << '%'<< std::endl;
}

void print_grams(TGramConteiner<N_GRAM> & grams){
	for(auto &gram : grams.m_grams)
		print_gram(gram.first, gram.second);
}

// count == 0 - print all
void print_sorted_grams(std::unordered_map<unsigned int, std::vector<char*>> &table, 
						std::vector<unsigned int> &keys, unsigned int count = 0){
	int num = count > 0 ? count : table.size();
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

	for(int i=0; i<num; ){
		auto key = keys[ikey++];
		for(auto &gram : table[key]){
			print_gram(gram, key, total);
			if(++i >= num)
				break;
		}
	}
}

int main(int argc, char **)
{
	std::cout << "open file" << std::endl;

	std::string text = "asd asdf asdfg sdfgh sdfgh asdfsdfsdfg dfgdfgh fghdsdfsasd asdsdfsghdsdf sdfgsdf";

	TGramConteiner<N_GRAM> grams;

	for(int ipos = 0; ipos < text.size()-N_GRAM; ipos++)
	{
		// skip spaces
		if(text[ipos + N_GRAM - 1] == ' '){
			ipos += N_GRAM-1;
			continue;
		}
		if(text[ipos] == ' ')
			continue;

		grams.addgram(&text[ipos]);
	}

	std::cout << "all grams:" << std::endl << std::endl;
	print_grams(grams);


	std::unordered_map<unsigned int, std::vector<char*>> sorted;
	std::vector<unsigned int> keys;
	grams.get_sorted_table(sorted, keys);

	std::cout << "grams top " << CHART_SIZE << std::endl;
	print_sorted_grams(sorted, keys, CHART_SIZE);
}