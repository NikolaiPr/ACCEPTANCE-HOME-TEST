/** @file  gramparser.h */
#pragma once

#include "..\includes\GramHash.h"
#include <unordered_set>

#define N_GRAM 4

namespace alg {

	const std::unordered_set<char> separators = { ' ', '\0', '\n', '\t' };
	const std::unordered_set<wchar_t> separators_utf8 = { L' ', L'\0', L'\n', L'\t' };

	/// @fn bool IsSeparator(T)
	/// Function for checking separator symbols
	template<typename T>
	bool IsSeparator(T) { return false; }
	template<>
	bool IsSeparator<char>(char ch) { return separators.find(ch) != separators.end(); }
	template<>
	bool IsSeparator<wchar_t>(wchar_t ch) { return separators_utf8.find(ch) != separators_utf8.end(); }

	/// @class TGramParser
	/// @brief N-gram(N-len substring) container.
	/// stores all grams in hash table
	/// gram length limited 5 because of memory limitation
	template<typename T, int N = N_GRAM>
	class TGramParser
	{
		static_assert(N < 6, "N should be less than 6");

	protected:
		gram_hashtable<T, N> m_grams;

		std::FILE* m_pFile;

		TGramParser() {}; ///< protected constructor
	public:
		TGramParser(std::FILE* pFile) : m_pFile(pFile) {};

		~TGramParser() {
			for (auto gr : m_grams)
				if (N > 1)
					delete[] gr.first;
		}

		const int getgramsize() { return N; }

		gram_hashtable<T, N>& getgrams() { return m_grams; }


		/// add new gram in conteiner (need optimization!! with move)
		/// @param gr[in] - gram characters sequence
		void addgram(T* gr) {
			auto itr = m_grams.find(gr);
			if (itr == m_grams.end()) {
				auto gram = new T[N];
				memcpy(gram, gr, sizeof(T)*N);
				m_grams.emplace_hint(itr, gram, 1);
			}
			else
				itr->second++;
		};


		/// open text file, extracting all K-grams, put them in m_grams hash table
		/// @param path[in] - text path file
		/// @return all grams count
		int FindAllGrams() {
			auto buf = new T[N];
			memset(buf, 0, sizeof(T)*N);
			int skip = N - 1;

			while (readsymb(m_pFile, buf[N - 1])) {

				// skip spaces
				if (IsSeparator(buf[N - 1])) // fseek - performance?
					skip = N; // find separator, skip addgram N steps

				if (skip == 0) // there is no separators, can add gram
					addgram(buf);

				if (skip > 0)
					skip--;

				// shift preview gram copy on 1 char(or T) left, add new char(or T) to gram end
				memcpy(buf, &buf[1], sizeof(T)*(N - 1));
			};

			delete[] buf;
			return m_grams.size();
		}
	};


	// template<T> 
	// inline bool readchar(std::FILE* pFile, T);
	/// @fn bool readsymb(std::FILE* pFile, char &ch)
	inline bool readsymb(std::FILE* pFile, char &ch) { return (ch = (char)getc(pFile)) != EOF; }
	/// @fn bool readsymb(std::FILE* pFile, wchar_t &ch)
	inline bool readsymb(std::FILE* pFile, wchar_t &ch) { return (ch = (wchar_t)fgetwc(pFile)) != WEOF; }

};
