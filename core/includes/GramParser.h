#pragma once
#include <unordered_map>
#include <algorithm>
#include <unordered_set>

#define N_GRAM 4

namespace alg {

	// Custom functors for std::unordered_map<T*, int>
	// Comparator functor.
	template <int N, class _Tp, typename ...>
	struct my_equal_to : public std::equal_to<_Tp*>
	{
		bool operator()(const char* __x, const char* __y) const
		{
			return strncmp(__x, __y, N) == 0;
		}

		bool operator()(const wchar_t* __x, const wchar_t* __y) const
		{
			return wcsncmp(__x, __y, N) == 0;
		}
	};


	// Hash functor.
	template <int N, class _Tp>
	struct my_HashChar : public std::hash<_Tp>
	{
		size_t operator()(const char* c_str) const {
			size_t hash = 0;
			int c = 0;
			char* str = (char*)c_str;

			while (c++ < N)
				hash = *(str++) + (hash << 6) + (hash << 16) - hash;
			return hash;
		}

		size_t operator()(const wchar_t* c_str) const {
			size_t hash = 0;
			int c = 0;
			wchar_t* str = (wchar_t*)c_str;

			while (c++ < N)
				hash = *(str++) + (hash << 6) + (hash << 16) - hash;
			return hash;
		}
	};


	template<typename T, int N>
	using gram_hashtable = std::unordered_map<T*, int, my_HashChar<N, T*>, my_equal_to<N, T*> >;


	// N-gram(N-len substring) container
	// stores all grams in container(map or hash table)
	// gram length limited 5 because of memory limitation

	template<typename T, int N>
	class TGramParser
	{
		static_assert(N < 6, "N should be less than 6");

		const std::unordered_set<T> separators = {L' ', L'\0', L'\n', L'\t'};
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

		gram_hashtable<T, N>& getgrams() { return m_grams;	}


		/// add new gram in conteiner (need optimization!! with move)
		/// @param [in]gr - gram characters sequence
		void addgram(T* gr) {
			auto itr = m_grams.find(gr);
			if (itr == m_grams.end()) {
				auto gram = new T[N];
				memcpy(gram, gr, sizeof(gr));
				m_grams[gram] = 1;
			}
			else
				m_grams[gr]++;
		};


		/// open text file, extracting all K-grams, put them in m_grams hash table
		/// @param [in]path - text path file
		/// @return all grams count
		int FindAllGrams() {
			auto buf = new T[N];
			memset(buf, 0, sizeof(T)*N);
			int skip = N-1;

			while (readsymb(m_pFile, buf[N - 1])) {

				// skip spaces
				if (separators.find(buf[N - 1]) != separators.end()) // fseek - performance?
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


		/// fill hashtable with suffixes after basegram
		/// reads file again from begining, search most freq grams, hash symbols behind basegram
		/// @param [in]gram_len basegram length
		int FindAllGramsAfterOne(const T* basegram, int gram_len) {

			std::fseek(m_pFile, 0, SEEK_SET);

			T ch;
			while (findinfile(basegram, gram_len, ch)) {

				addgram(&ch);
			}

			return m_grams.size();
		}


		/// find N-gram gram in file
		/// @param [in]Ngr - base gram
		/// @param [out]ch - symbol after gram 
		/// @return symbol found
		bool findinfile(const T *Ngr, int gram_len, T &ch) {

			auto buf = new T[gram_len];
			memset(buf, 0, sizeof(T)*gram_len);

			bool bres = false;

			while (readsymb(m_pFile, buf[gram_len - 1])) {

				int i = 0;
				// compare grams
				for (; i < gram_len; i++)
					if (buf[i] != Ngr[i])
						break;

				// Ngr found
				if (i == gram_len) {

					fpos_t pos = 0;
					fgetpos(m_pFile, &pos);

					if (readsymb(m_pFile, ch)) {
						if (separators.find(ch) != separators.end()) // fseek - performance?
							break;

						bres = true; // additional symbol found
					}

					fseek(m_pFile, gram_len + 1, SEEK_CUR); // move on next pos after Ngr gram starts
					break;
				}

				// shift preview gram copy on 1 char(or T) left, add new char(or T) to gram end
				memcpy(buf, &buf[1], sizeof(T)*(gram_len - 1));
			}

			delete[] buf;
			return bres;
		}
	};


	//template<T> 
	//inline bool readchar(std::FILE* pFile, T);
	inline bool readsymb(std::FILE* pFile, char &ch)	{ return (ch = (char)getc(pFile)) != EOF; }
	inline bool readsymb(std::FILE* pFile, wchar_t &ch) { return (ch = (wchar_t)fgetwc(pFile)) != WEOF; }

};
