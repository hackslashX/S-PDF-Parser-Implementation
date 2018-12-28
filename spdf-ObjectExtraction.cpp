//
// SPDF OBJECT EXTRACTION AND MANIPULATION
//

#include "spdf-structure.h"
#include <sstream>
#include <regex>

using namespace std;

void spdf::resolveObject(vector<xrefEntry>::iterator i)
{
	vector<xrefEntry>::iterator j = i;
	j++;
	if (j == spdfXref.end())
	{
		i->endingOffset = xrefTableLocation - 1;
		return;
	}
	while (true)
	{
		if (j->objectType == 'f') j++;
		else
			break;
	}
	if ( j != spdfXref.end())
	{
		i->endingOffset = j->startingOffset - 1;
	}

}

void spdf::loadObjectStructure()
{
	/*
	To achieve this we'll start finding the ending offset of every object
	*/

	// Sort the Vector
	sort(spdfXref.begin(), spdfXref.end(), [](const xrefEntry &a, const xrefEntry &b) {return a.startingOffset < b.startingOffset; });
	for (vector<xrefEntry>::iterator i = spdfXref.begin(); i != spdfXref.end(); i++)
	{
		// Verify first if the object is free or in use (free objects are simply ignored)
		if (i->objectType == 'f')
			continue; // Jump to next object
		if (i->objectType == 'n')
		{
			resolveObject(i);
		}
		else
		{
			throw "Error in processing xref table. Most probably invalid PDF file!";
		}
	}

	// Start populating page structure
	int j{};
	for (vector<xrefEntry>::iterator i = spdfXref.begin(); i != spdfXref.end(); i++, j++)
	{
		if (i->objectType == 'f')
			continue; // Jump to next object
		if (i->objectType == 'n')
		{
			if(loadPageTree(i)) break;
		}
		if (j == 62)
		{
			cout << endl;
		}
	}

	// Now we'll iterate on every known page to find for fonts and images
	for (int i = 0; i < spdfPages.totalPages; i++)
	{
		loadPageFonts(i); // Start loading fonts for the specified page
		loadFontInformation(i); // Extract the font information
		loadPageImage(i); // Start loading images for the specified page
		loadImageInformation(i); // Extract the image information
	}
}

string spdf::loadDictionary(string &input)
{
	// Extract dictionary
	string output{};
	auto dictStart = "<<"s;
	auto dictEnd = ">>"s;
	regex dictRx(dictStart + "(.*)" + "[\\S\\s]*" + dictEnd);
	smatch rxMatch;
	while (regex_search(input, rxMatch, dictRx))
	{
		output = rxMatch.str(); // Apparently the first match is (in most of the cases) all we need
		break; // skip the other matches (they're probably useless)
	}
	return output;
}