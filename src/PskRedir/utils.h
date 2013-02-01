#include <string>
#include <vector>
#include <sstream>

namespace pskutils
{
	/**
	* Trims the given string.
	*/
	void trim(std::string & str);

	/**
	* Splits the string on whitespaces, putting the resulting
	* tokens into the given vector.
	*/
	void splitInWhitespaces(std::string & str, std::vector<std::string>& outputvec);

	/**
	* Parses the string to the given number type.
	*/
	template<typename T>
	T parseNum(std::string & s) 
	{
		T num;
		stringstream(s) >> num;
		return num;
	}

	/**
	* Converts the number to its string representation.
	*/
	template<typename T>
	std::string numToString(T num) 
	{
		ostringstream ostrstr;
		ostrstr << num;
		return ostrstr.str();
	}

	/**
	* Builds a message by concatenating the two parts. It is intended
	* for messages like:
	* "Error doing XXXXX. Retcode = 100"
	*/
	std::string buildErrorMessage(const std::string& text, const int number);
}