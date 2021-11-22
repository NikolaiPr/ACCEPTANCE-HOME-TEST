#pragma once
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <set>
#include <algorithm>

#define N_GRAM 4
namespace alg {

	// Custom functors for std::unordered_map<char*, int>
	// Comparator functor.
	template <class _Tp, int N>
	struct my_equal_to
	{
		bool operator()(const _Tp& __x, const _Tp& __y) const
		{
			return strncmp(__x, __y, N) == 0;
		}
	};

	// Hash functor.
	template <class _Tp, int N>
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

	template< int N>
	using gram_hashtable = std::unordered_map<char*, int, my_HashChar<char*, N>, my_equal_to<char*, N> >;


	// N-gram(N-len substring) container
	// stores all grams in container(map or hash table)
	// gram length limited 5 because of memory limitation

	template< int N>
	class TGramParser
	{
		static_assert(N < 6, "N must be less than 6");
		//using gram_hashtable = std::unordered_map<char*, int, my_HashChar<char*, N>,  my_equal_to<char*, N> >;

	protected:
		gram_hashtable<N> m_grams;

		std::FILE* m_pFile;

		TGramParser() {};
	public:
		TGramParser(std::FILE* pFile) : m_pFile(pFile) {};

		~TGramParser() {
			for (auto gr : m_grams) {
				if (N > 1)
					delete[] gr.first;
				//else
				//	delete gr.first;
			}
		}

		int getgramsize() { return N; }
		gram_hashtable<N>& getgrams() {
			return m_grams;
		}

		// add new gram in conteiner
		//[in] gr - gram characters sequence
		// need optimization!! with move
		void addgram(char* gr) {
			auto itr = m_grams.find(gr);
			if (itr == m_grams.end()) {
				auto gram = new char[N];
				memcpy(gram, gr, sizeof(gr));
				m_grams[gram] = 1;
			}
			else
				m_grams[gr]++;
		};

		/// open text file, extracting all K-grams, put them in m_grams hash table
		/// [in] path - text path file
		/// return 
		int FindAllGrams()
		{
			char gr[N];
			for (int i = 0; i < N; i++)
				gr[i] = ' ';
			int c;

			while ((c = getc(m_pFile)) != EOF) {

				// shift preview gram copy on 1 char left, add new char to gram end
				memcpy(gr, &gr[1], sizeof(char)*(N - 1));
				gr[N - 1] = (char)c;

				// skip spaces, need optimization!
				bool bSpace = false;
				for (int i = 0; i < N; i++)
					if (gr[i] == ' ' || gr[i] == '\n') { // need optimization
						bSpace = true;
						continue;
					}
				if (bSpace)
					continue;

				addgram(gr);
			};

			return m_grams.size();
		}


		/// reads file again fron begining, searche most freq grams, checks K-N len suffixes behind gram to complete it to K size
		/// [in] K - max gram length
		int FindAllGramsAfterOne(const char* basegram, int gram_len) {

			std::fseek(m_pFile, 0, SEEK_SET);

			char ch;
			while (findinfile(basegram, gram_len, ch)) {

				addgram(&ch);
			}

			return m_grams.size();
		}

		//find N-gram gram in file, returns expanded K-gram
		//[in]Ngr - base gram
		//[out]ch - item after gram 
		bool findinfile(const char *Ngr, int gram_len, char &ch) {

			auto tmp = new char[gram_len];
			for (int i = 0; i < gram_len; i++)// need optimization
				tmp[i] = ' ';


			bool bres = false;
			int c;

			while ((c = getc(m_pFile)) != EOF) {

				// shift preview gram copy on 1 char left, add new char to gram end
				memcpy(tmp, &tmp[1], sizeof(char)*(gram_len - 1));
				tmp[gram_len - 1] = (char)c;

				int i = 0;
				// compare grams
				for (; i < gram_len; i++)
					if (tmp[i] != Ngr[i])
						break;

				// Ngr found
				if (i == gram_len) {

					fpos_t pos = 0;
					fgetpos(m_pFile, &pos);

					if ((c = getc(m_pFile)) != EOF ) {
						if (c == ' ' || c == '\n') // need optimization
							break;

						// additional Kgr found
						ch = c;
						bres = true;
					}

					fseek(m_pFile, gram_len + 1, SEEK_CUR); // move on next pos after Ngr gram starts
					break;
				}
			}

			return bres;
		}

		/// reads file again fron begining, searche most freq grams, checks K-N len suffixes behind gram to complete it to K size
		/// [in] K - max gram length
		/*int FindAllGramsAfterOne(const char* basegram, int gram_len, int K) {

			std::fseek(m_pFile, 0, SEEK_SET);

			// clear gram
			char gr[N];
			for (int i = 0; i < N; i++)// need optimization
				gr[i] = ' ';

			while (findinfile(basegram, gram_len, &gr[0], K)) {

				addgram(gr);
			}

			return m_grams.size();
		}
		*/

		//find N-gram gram in file, returns expanded K-gram
		//[in]Ngr - base gram
		//[out]Kgr -expanded gram 
		//[in] K - max gram length
		/*bool findinfile(const char *Ngr, int gram_len, char *Kgr, int K) {

			auto tmp = new char[gram_len];
			for (int i = 0; i < gram_len; i++)// need optimization
				tmp[i] = ' ';


			bool bres = false;
			int c;

			while ((c = getc(m_pFile)) != EOF) {

				// shift preview gram copy on 1 char left, add new char to gram end
				memcpy(tmp, &tmp[1], sizeof(char)*(gram_len - 1));
				tmp[gram_len - 1] = (char)c;

				int i = 0;
				// compare grams
				for (; i < gram_len; i++)
					if (tmp[i] != Ngr[i])
						break;

				// Ngr found
				if (i == gram_len) {

					fpos_t pos = 0;
					fgetpos(m_pFile, &pos);

					int j = 0;
					while ((c = getc(m_pFile)) != EOF && j < N && gram_len + j < K) {
						if (c == ' ' || c == '\n') // need optimization
							break;
						Kgr[j++] = c;
					}

					if (j == N || gram_len + j == K)
						bres = true; // additional Kgr found

					fseek(m_pFile, gram_len + 1, SEEK_CUR); // move on next pos after Ngr gram starts
					break;
				}
			}

			return bres;
		}
		*/
	};
};
