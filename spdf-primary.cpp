//
// SPDF Primary Functions
//

#include "spdf-structure.h"

using namespace std;

spdf::spdf(const char * pathToFile)
{
		pdfFile.open(pathToFile, ios::in | ios::ate);
		if (pdfFile.is_open())
		{
			pdfFile.seekg(0, ios::beg);
			pdfFile.read(pdfVersion, 8); // Write the initial 8 bytes buffer which contains PDF version
			pdfVersion[8] = NULL;
			pdfPath = pathToFile;
		}
		else throw "Unable to open the PDF file";
}

spdf::~spdf()
{
	// Perform cleaning - WILL ADD LATER
}


