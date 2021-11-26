#include "..\includes\chart.h"

using namespace alg;


/// explicit template instantiation
/// solve the inctance redefinition problem
/// limits file data types to use only this
CORE_EXTERN template class CORE_EXPORT CChart<char>;
CORE_EXTERN template class CORE_EXPORT CChart<wchar_t>;



/// clear chart
template<typename T>
void CChart<T>::Clear()
{
	m_chart.clear();
}

/// @return bottom item frequency
template<typename T>
int CChart<T>::GetBottomFreq()
{
	if (m_chart.empty())
		return 0;
	return m_chart.back().m_freq;
}

/// @return bottom item frequency
template<typename T>
int CChart<T>::GetAcceptFreq()
{
	return IsFull() ? GetBottomFreq() + 1 : 1;
}

/// check is list full
template<typename T>
bool CChart<T>::IsFull()
{
	return (m_chart.size() >= m_iLength);
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
			if (m_chart.size() > m_iLength)
				m_chart.pop_back();
			return true;
		}
	}

	// combination has lesser freq, but still there is free place in chart -> add
	m_chart.push_back(comb);
	return true;
}

// force combination insert with increasing list length
template<typename T>
int CChart<T>::ForceInsert(const chart_item<T> &comb)
{
	m_chart.push_back(comb);
	m_iLength = m_chart.size();
	return m_iLength;
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

template<typename T>
int CChart<T>::GetCombinationString(const chart_item<T> &comb, T*& buf)
{
	int len = comb.m_comb.size();
	buf = new T[len+1];

	for (int i = 0; i < len; i++)
		buf[i] = comb.m_comb[i];

	return len;
}

template<typename T>
bool CChart<T>::operator!=(const CChart<T> &chart)
{
	if (m_iLength != GetMaxLength())
		return true;

	if (m_chart.size() != chart.m_chart.size())
		return true;

	auto itr = chart.m_chart.begin();
	for (auto comb : m_chart) {

		if (comb.m_freq != itr->m_freq)
			return true;

		if(!(comb.m_comb == itr->m_comb))
			return true;
		itr++;
	}
	return false;
}