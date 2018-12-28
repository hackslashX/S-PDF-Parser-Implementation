//
// SPDF Initialize Functions
//

#include "spdf-structure.h"

using namespace std;

void spdf::init() // TODO Deal with Linearized PDFs
{
    int xrefLocation{}; // Main document cross reference table location
    pdfFile.seekg(0, ios::end); // seek to the end of the file
    // Performing EOF check
    if (!performEOFCheck())
        throw "Unable to find %%EOF tag. Probably invalid PDF file.";

				
	// Performing search for startxref flag that'll point to xref location
    char charTemp{ '\0' };
	int pos{-1};
    do
    {
        pdfFile.seekg(pos, ios::end);
        if (charTemp == 'f') //startxref ends in f
            break;
		//if (charTemp == '\n')
		//	pdfFile.seekg(-1, ios::cur);
		--pos;
    } while (pdfFile.read(&charTemp,1));

    spdf::getline(pdfFile); // Move to next line
    string xrefLocationStr{};
    xrefLocationStr = getline(pdfFile);
    xrefLocation = stoi(xrefLocationStr);
	xrefTableLocation = xrefLocation;
    if (xrefLocation == 0)
    {
        throw "Unable to find Xref location. Probably an invalid PDF file.";
    }

    // Start loading xref (assuming uncompressed)
    loadXref(xrefLocation);
	
	// Start populating the object structure
	loadObjectStructure();
}