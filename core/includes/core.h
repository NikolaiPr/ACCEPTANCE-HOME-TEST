#pragma once
#include <unordered_map>
#include <string>
#include <vector>
#include <set>
#include <unordered_set>
#include <algorithm>

// Custom functors for std::unordered_map<char*, unsigned int>
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

template< unsigned int N>
class TGramConteiner
{
	static_assert(N < 6, "N must be less than 6");
	using gram_cont = std::unordered_map<char*, unsigned int, my_HashChar<char*, N>,  my_equal_to<char*, N> >;
	
protected:
	gram_cont m_grams;

public:
	~TGramConteiner(){ 
		for(auto gr : m_grams)
			delete []gr.first;
	}

	const gram_cont& getgrams(){
		return m_grams;
	}

	// add new gram in conteiner
	void addgram(char* gr){
		auto gram = new char[N];
		memcpy(gram, gr, sizeof(char)*N);
		
		auto itr = m_grams.find(gram);
		if(itr == m_grams.end())
			m_grams[gram] = 1;
		else
			m_grams[gram]++;
	};

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

	bool ParseGram(std::string &path)
	{
		std::FILE* pFile = std::fopen(path.c_str(), "r");
		if(!pFile)
			return false;

		char gr[N];
		for(int i=0; i<N; i++)
			gr[i] = ' ';
		int c;

		while ( (c = getc (pFile)) != EOF){

			memcpy(gr, &gr[1], sizeof(char)*(N-1));
			gr[N-1] = (char)c; 

			// skip spaces
			bool bSpace = false;
			for(int i=0; i<N; i++)
				if(gr[i] == ' ' || gr[i] == '\n'){
					bSpace = true;
					continue;
				}
			if(bSpace)
				continue;

			addgram(gr);
		};

		std::fclose(pFile);
		return true;
	}
};
