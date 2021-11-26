#include "stdafx.h"
#include "CppUnitTest.h"
#include <iostream>
#include <string>
#include <wchar.h>
#include "..\..\core\includes\core.h"

//#define PATH "L:\\PROEKSPERT-HOME-TEST\\core_test\\test_files\\"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;
using namespace alg;


namespace UnitTest
{		
	TEST_CLASS(TestFiles)
	{
		const string path = "L:\\PROEKSPERT-HOME-TEST\\core_test\\test_files\\";
		const vector<string> files = { "default", "oneletter", "twoletters", "m_threeletters",
			"3res", "empty", "m_nores", "nores", "m_sixletters", "sixletters", "symbols",
		"large", "large2", "large4", "large8" };


		bool TestFile(const string file)
		{
			CAlgorithm<char> ChartProcessor(path + file + ".txt");
			auto test = ChartProcessor.LoadResults(path + file + ".txtres") == err_codes::no_err;

			auto res = ChartProcessor.Execute(test) == err_codes::no_err;
			if (!test && res) {
				Logger::WriteMessage(L"No test results - create new");
				ChartProcessor.SaveResults();
			}
			return res;
		}
	public:
	
		TEST_METHOD(CheckTestFiles)
		{
			auto allres = true;
			for (auto file : files) {
				auto res = (GetFileFormat(path + file + ".txt") == textfile_format::ascii_file);
				if (!res)
					allres = false;
				if (!res)
					Logger::WriteMessage((path + file + ".txt - " + (res ? "ok\n" : "error\n")).c_str());
			}
			Assert::IsTrue(allres);
		}
		TEST_METHOD(CheckResultFiles)
		{
			auto allres = true;
			for (auto file : files) {
				auto res = (GetFileFormat(path + file + ".txtres") == textfile_format::ascii_file);
				if (!res)
					allres = false;
				if (!res)
					Logger::WriteMessage((path + file + ".txtres - " + (res ? "ok\n" : "error\n")).c_str());
			}
			Assert::IsTrue(allres);
		}

		TEST_METHOD(TestFile_default)
		{
			Assert::IsTrue(TestFile("default"), L"Execute err");
		}
		TEST_METHOD(TestFile_oneletter)
		{
			Assert::IsTrue(TestFile("oneletter"), L"Execute err");
		}
		TEST_METHOD(TestFile_twoletters)
		{
			Assert::IsTrue(TestFile("twoletters"), L"Execute err");
		}
		TEST_METHOD(TestFile_m_threeletters)
		{
			Assert::IsTrue(TestFile("m_threeletters"), L"Execute err");
		}
		TEST_METHOD(TestFile_sixletters)
		{
			Assert::IsTrue(TestFile("sixletters"), L"Execute err");
		}
		TEST_METHOD(TestFile_m_sixletters)
		{
			Assert::IsTrue(TestFile("m_sixletters"), L"Execute err");
		}
		TEST_METHOD(TestFile_3res)
		{
			Assert::IsTrue(TestFile("3res"), L"Execute err");
		}
		TEST_METHOD(TestFile_empty)
		{
			Assert::IsTrue(TestFile("empty"), L"Execute err");
		}
		TEST_METHOD(TestFile_m_nores)
		{
			Assert::IsTrue(TestFile("m_nores"), L"Execute err");
		}
		TEST_METHOD(TestFile_nores)
		{
			Assert::IsTrue(TestFile("nores"), L"Execute err");
		}
		TEST_METHOD(TestFile_symbols)
		{
			Assert::IsTrue(TestFile("symbols"), L"Execute err"); 
		}
		TEST_METHOD(TestFile_large)
		{
			Assert::IsTrue(TestFile("large"), L"Execute err");
		}
		TEST_METHOD(TestFile_large2)
		{
			Assert::IsTrue(TestFile("large2"), L"Execute err");
		}
		TEST_METHOD(TestFile_large4)
		{
			Assert::IsTrue(TestFile("large4"), L"Execute err");
		}
		TEST_METHOD(TestFile_large8)
		{
			Assert::IsTrue(TestFile("large8"), L"Execute err");
		}

	};
}

