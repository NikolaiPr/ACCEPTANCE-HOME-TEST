#pragma once
#include <string>
#include <memory>
#include "..\includes\dll_exports.h"
#include "..\includes\chart.h"


#define MIN_COMBINATION_LEN 4
#define MAX_COMBINATION_LEN 10
#define MAX_CHART_LEN 10

namespace alg{

	enum class err_codes {
		no_err = 1,
		err_unknown = 0,
		err_filename = -1,
		err_file = -2,
		err_comblen = -3,
		err_chartlen = -4,
	};

	// CAlgorithm class
	//uses any text file with random content as an input, and checks all the words contained 
	//in such text file for letter combinations consisting of more than 3 letters.
	//output will consist of up to 10 most common letter combinations and their difference 
	//in percentage displayed as a table and a bar chart
	class CORE_EXPORT CAlgorithm
	{
	public:
		std::string m_file;
		int m_iCombinationLength;
		int m_iChartLength;
		std::shared_ptr<CCharChart> m_pChart;


	public:
		CAlgorithm() : m_iCombinationLength(4), m_iChartLength(MAX_COMBINATION_LEN) {
			m_pChart = std::make_shared<CCharChart>(m_iChartLength);
		};

		CAlgorithm(std::string &file, int iCombinationLength = 4, char iChartLength = MAX_COMBINATION_LEN)
			: m_file(file), m_iCombinationLength(iCombinationLength), m_iChartLength(iChartLength) {

			m_pChart = std::make_shared<CCharChart>(iChartLength);
		};

		// main function
		err_codes Execute();
	};
};
