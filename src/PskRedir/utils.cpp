#include "utils.h"

#include <string>
#include <iostream>
#include <sstream>
#include <iterator>

namespace pskutils
{
	using namespace std;

	void trim(std::string & str) {
		size_t beginActualString = str.find_first_not_of(" \n\r\t");

		if (beginActualString == string::npos) {
			str.clear();
			return;
		}

		size_t endActualString = str.find_last_not_of(" \n\r\t");

		if (beginActualString > 0)
			str.erase(0, beginActualString);

		if ((endActualString) < (str.length() - 1))
			str.erase(endActualString + 1, string::npos);
	}

	void splitInWhitespaces(string & str, vector<string>& outputvec)
	{
		istringstream stream(str);
		istream_iterator<string> iterator(stream);
		istream_iterator<string> limit;
		while (iterator != limit) {
			outputvec.push_back(*iterator);
			iterator++;
		}
	}

	/*
	*
	* Lembrete: Este template esta definido no arquivo Header:

	template<typename T>
	T parseNum(std::string & s) {
		... 
	}
	*/

	string buildErrorMessage(const string& text, const int number)
	{
		stringstream ss;
		ss << text << number;
		return ss.str();
	}
}