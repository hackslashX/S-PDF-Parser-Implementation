//
// SPDF VALIDATION
//

#include "spdf-structure.h"

using namespace std;

// Find the ending %%EOF tag. Abort sequence if tag was not found
bool spdf::performEOFCheck()
{
	char buffer[6];
	pdfFile.seekg(-1, ios::end);
	size_t i{}, returnFlag{ 1 };
	int byteCount = -1;
	while (returnFlag)
	{
		char * tempChar = new char;
		pdfFile.read(tempChar, 1);
		if (isalnum(*(tempChar))) // Extracting readable characters
		{
			buffer[5 - (i + 1)] = *(tempChar);
			++i;
		}
		if (*(tempChar) == '%') // Exception to the generic rule
		{
			buffer[5 - (i + 1)] = *(tempChar);
			++i;
		}
		delete tempChar;
		if (i == 5)
		{
			returnFlag = 0; // Set return flag to zero (indicate false)
		}
		--byteCount;
		pdfFile.seekg(byteCount, ios::end); // Seek 2 positions back
	}
	buffer[5] = '\n'; // Add EOL character
	if (!strcmp(buffer, "%%EOF"))
	{
		return false;
	} // else continue on
	eofLocaction = byteCount; // Save EOF location for later use (like locating eof pointer easily during rendering)
	pdfFile.seekg(byteCount, ios::end); // Seek to EOF start
	return true;
}