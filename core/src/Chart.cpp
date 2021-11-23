#include "..\includes\chart.h"

using namespace alg;


/// explicit template instantiation
/// solve the inctance redefinition problem
/// limits file data types to use only this
CORE_EXTERN template class CORE_EXPORT CChart<char>;
CORE_EXTERN template class CORE_EXPORT CChart<wchar_t>;


/// @return bottom item frequency
template<typename T>
int CChart<T>::GetBottomFreq()
{
	if (m_chart.empty())
		return 0;
	return m_chart.back().m_freq;
}


/// check is list full
template<typename T>
bool CChart<T>::IsFull()
{
	return (m_chart.size() == m_iLength);
}


/// check is list empty
template<typename T>
bool CChart<T>::IsEmpty()
{
	return m_chart.empty();
}


/// tries to add combination
/// @param comb new combination
template<typename T>
bool CChart<T>::AddCombination(const chart_item<T> &comb)
{
	if (IsFull() && GetBottomFreq() >= comb.m_freq)
		return false;

	// looking for place in chart by freq
	// put new item before one with lesser frequency
	for (auto itr = m_chart.begin(); itr != m_chart.end(); ++itr) {
		if (itr->m_freq < comb.m_freq) {
			m_chart.insert(itr, comb);
			return true;
		}
	}

	// combination has lesser freq, but still there is free place in chart -> add
	m_chart.push_back(comb);
	return true;
}


/// @return all chart items frequency sum
template<typename T>
int CChart<T>::GetTotal()
{
	int total = 0;

	for (auto &item : m_chart)
		total += item.m_freq;

	return total;
}


std::string CChart<char>::GetCombinationString(const std::vector<char> &comb)
{
	std::string str(comb.data(), comb.size());
	return std::move(str);
}

std::string CChart<wchar_t>::GetCombinationString(const std::vector<wchar_t> &comb)
{
	std::string str;// (comb.data(), comb.size());
	return std::move(str);
}

