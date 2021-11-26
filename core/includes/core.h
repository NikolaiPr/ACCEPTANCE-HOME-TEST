#pragma once
#include <string>
#include <memory>
#include "..\includes\dll_exports.h"
#include "..\includes\chart.h"

#define MIN_COMBINATION_LEN 4	/// minimum combination start length
#define MAX_CHART_LEN 10		/// maximum chart length
//#define MAX_COMBINATION_LEN  (MIN_COMBINATION_LEN + 10)


namespace alg{

	/// file formats and result code
	enum textfile_format {
		ascii_file = 0,
		error_file = -1,
		utf8_file = 0xbfbbef,//0xefbbbf,
		utf7_file = 0x762f2b,//0x2b2f76,
		unicode_file = 0xfeff,//0xfffe,
		bigendianunicode_file = 0xfffe,//0xfeff,
		utf32_file = 0xfffe0000//0x0000feff,
	};

	/// return codes for main Execute function
	/// @see Execute()
	enum class err_codes {
		no_err = 1,
		err_unknown = 0,
		err_filename = -1,
		err_file = -2,
		err_comblen = -3,
		err_chartlen = -4,
		err_testfailed = -5,
	};


	// base algorithm class to separate working with templates
	class CORE_EXPORT AAlgorithm
	{
	public:
		std::string m_file;						///< file name
		//int m_iCombinationLength;				///< minimal start combination length
		int m_iChartLength;						///< chart length

	public:
		/// default constructor
		AAlgorithm() : m_iChartLength(MAX_CHART_LEN)/*, m_iCombinationLength(4)*/ {};

		/// explicit constructor
		/// @param file full file path
		/// @param iCombinationLength minimum combination start length
		/// @param iChartLength	maximum chart length
		AAlgorithm(const std::string &file, int iCombinationLength = MIN_COMBINATION_LEN, int iChartLength = MAX_CHART_LEN)
			: m_file(file), m_iChartLength(iChartLength)/*, m_iCombinationLength(iCombinationLength)*/{}

		/// open file
		/// @param bWrite write mode, else read mode
		std::FILE* OpenFile(bool bWrite);

		/// main algorithm function
		/// @return error code 
		/// @return no_err if all ok
		/// @see err_codes
		virtual err_codes Execute(bool bTestMode = false) = 0;

		// print results into console
		// suports only char type
		virtual void PrintResults(bool bPercentage = true) = 0;

		// save results into file
		virtual err_codes SaveResults() = 0;

		// load results from file
		virtual err_codes LoadResults(const std::string &file) = 0;
	};



	/// CAlgorithm class.
	/// Uses any text file with random content as an input, and checks all the words contained 
	/// in such text file for letter combinations consisting of more than 3 letters.
	/// output will consist of up to 10 most common letter combinations and their difference 
	/// in percentage displayed as a table and a bar chart

	// @param T internal data type(tested type - char, wchat_t)
	// TODO: type variants: char8_t, char16_t, char32_t

	template<typename T>
	class CAlgorithm : public AAlgorithm
	{
	protected:
		std::shared_ptr<CChart<T>> m_pChart;	///< the chart

	public:
		/// default constructor
		CAlgorithm();

		/// explicit constructor
		/// @param file full file path
		/// @param iCombinationLength minimum combination start length
		/// @param iChartLength	maximum chart length
		CAlgorithm(const std::string &file, int iCombinationLength = MIN_COMBINATION_LEN, int iChartLength = MAX_CHART_LEN);

		const std::shared_ptr<CChart<T>>& GetChart() { return m_pChart; };

		/// main algorithm function
		/// @param bTestMode test algoritm with loaded results
		/// @return error code 
		/// @return no_err if all ok
		/// @see err_codes
		err_codes Execute(bool bTestMode = false) override;


		// print results into console
		// suports only char type
		void PrintResults(bool bPercentage = true) override;

		// save results into file
		err_codes SaveResults() override;

		err_codes LoadResults(const std::string &file) override;

	protected:
		/// finds expanded combinations in file
		/// get combination from chart, searchs in file combination len+1, 
		/// place back in chart most frequent combination
		/// @param pFile the same already oppened file
		/// @iExpCombLen combination length to expand
		bool FindExpanded(std::FILE* pFile, int iExpCombLen);
	};



	/// get file format
	/// TODO: move func on cpp, fix problem with export
	textfile_format GetFileFormat(const std::string &file)
	{
		std::FILE* pFile = nullptr;
		fopen_s(&pFile, file.c_str(), "r");
		if (!pFile)
			return textfile_format::error_file;

		int c;
		int format = 0;
		int i = 0;

		while ((c = std::fgetc(pFile)) != EOF && i < 4)
		{
			format |= (c << i * 8);
			switch (format)
			{
			case 0xbfbbef:		return textfile_format::utf8_file;			//0xefbbbf
			case 0x762f2b:		return textfile_format::utf7_file;			//0x2b2f76
			case 0xfeff:		return textfile_format::unicode_file;		//0xfffe
			case 0xfffe:		return textfile_format::bigendianunicode_file;//0xfeff
			case 0xfffe0000:	return textfile_format::utf32_file;			//0x0000feff
			}
			i++;
		}

		std::fclose(pFile);
		return textfile_format::ascii_file;
	}
};
