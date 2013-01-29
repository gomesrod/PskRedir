#include <string>
#include <vector>
#include <sstream>

typedef std::vector<std::string> stringvector;

namespace myutils
{
	/**
	* Trims the string.
	*/
	void trim(std::string & str);

	/**
	* Splits the string on whitespaces, putting the resulting
	* tokens into the given vector.
	*/
	void splitInWhitespaces(std::string & str, stringvector & outputvec);

	/**
	* Parses the string to the given number type.
	* (Por ser um template a declaração deve estar definido no mesmo arquivo do header)
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
	* (Por ser um template a declaração deve estar definido no mesmo arquivo do header)
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

	/**
	* Prints an arbitrary message to the standard output.
	*/
	void sysout(const std::string& text);
}