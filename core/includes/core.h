#pragma once
#include <unordered_map>
#include <string>
#include <vector>
#include <set>
#include <unordered_set>
#include <algorithm>

// Custom functors for std::unordered_map<char*, >
// Comparator functor.
template <class _Tp, unsigned int N>  
struct my_equal_to 
{  
    bool operator()(const _Tp& __x, const _Tp& __y) const  
    { return strncmp( __x, __y, N) == 0; }  
};

// Hash functor.
template <class _Tp, unsigned int N>  
struct my_HashChar {

  	size_t operator()(const _Tp& c_str) const {
    	size_t hash = 0;
    	int c = 0;
		_Tp str = (_Tp)c_str;

    	while (c++ < N)
      		hash = *(str++) + (hash << 6) + (hash << 16) - hash;
    	return hash;
  	}
};



// N-gram(N-len substring) container
// stores all grams in container(map or hash table)
// gram length limited 5 because of memory limitation
template<unsigned int N>
class TGramConteiner
{
	static_assert(N < 5, "N must be less than 5");
	using gram_cont = std::unordered_map<char*, unsigned int, my_HashChar<char*, N>,  my_equal_to<char*, N> >;
	
public:

	gram_cont m_grams;
	
	// add new gram in conteiner
	void addgram(char* gram){
		auto itr = m_grams.find(gram);
		if(itr == m_grams.end())
			m_grams[gram] = 1;
		else
			m_grams[gram]++;
	};

	//remove gram from conteiner
	void removegram(char* gram){
		auto itr = m_grams.find(gram);
		if(itr != m_grams.end())
			m_grams.erase(itr);
	}

	//clear conteiner from less frequently appearing grams
	//void clearunfreqgram(int min_freq){
	//	for(auto gram_itr = m_grams.begin(); gram_itr != m_grams.end(); ){
	//		if(gram_itr->second < min_freq)
	//			gram_itr = m_grams.erase(gram_itr);
	//		else ++gram_itr;
	//	}
	//}

	// looking for most frequently appearing gram
	const char*get_most_freq_gram(){
		auto gram_pos = m_grams.begin();
		int maxfreq = gram_pos->second;
		
		for(auto gram_itr = m_grams.begin(); gram_itr != m_grams.end(); ++gram_itr){
			if(gram.second > maxfreq){
				gram_pos = gram_itr;
				maxfreq = gram.second;
			}
		}
	}

	// create sorted gram freq table
	void get_sorted_table(std::unordered_map<unsigned int, std::vector<char*>> &sorted, std::vector<unsigned int> &keys){
		std::unordered_set<unsigned int> tmp;
		for(auto gram : m_grams){
			sorted[gram.second].push_back(gram.first);
			tmp.insert(gram.second);
		}
		 
		keys.reserve(tmp.size());
    	for (auto i : tmp)                                         
        	keys.push_back(i);  

		// sort with descending order.
    	std::sort(keys.begin(), keys.end(), [&](unsigned int &a, unsigned int &b) { return a > b; });
	}
};
