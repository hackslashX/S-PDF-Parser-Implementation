//
// SPDF SUPPLEMENTARY
//

#include "spdf-structure.h"
#include <sstream>
#include <regex>

using namespace std;

// PDF version of getline that obeys PDF newline conventions \n \r or \r\n and returns the next string in PDF
std::string spdf::getline(fstream &source)
{
	string nextLine{};
	char tempChar{};
	while (source.get(tempChar))
	{
		nextLine += tempChar;
		if (tempChar == '\n')
			break;
		if (tempChar == '\r') {
			if (!source.eof() && source.peek() == '\n')
				nextLine += source.get();
			break;
		}
	}
	return nextLine;
}

// A simple string trim function specialised for our needs
string spdf::trim(string line)
{
	return line.substr(0, line.find_last_not_of("\r\t\n ") + 1);
}

// Remove common font descriptors leaving out pure font names
string spdf::cleanFontDescriptors(string &input)
{
	input = regex_replace(input, regex("#20"), " ");
	replace(input.begin(), input.end(), '+', ' ');
	input = regex_replace(input, regex("ABCDEE"), "");
	input = regex_replace(input, regex("BCDEEE"), "");
	input = regex_replace(input, regex("F([0-9][0-9]*)"), "");
	input = regex_replace(input, regex("<<"), "");
	input = regex_replace(input, regex(">>"), "");
	input = regex_replace(input, regex("\\["), "");
	input = regex_replace(input, regex("\\]"), "");
	string output(input);
	return output;
}