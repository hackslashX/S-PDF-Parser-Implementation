//
//	SPDF FONTS EXTRACTION
//

#include"spdf-structure.h"
#include<regex>


using namespace std;

// Feed all object identifiers which represent fonts used in a page
bool spdf::loadPageFonts(int pg)
{
	using namespace std::string_literals;

	int pgObj = spdfPages.pageObjects[pg].pageObject;
	vector<xrefEntry>::iterator i = find_if(spdfXref.begin(), spdfXref.end(), [pgObj](const xrefEntry &a) {return a.objectNumber == pgObj; });

	int offset = i->startingOffset;
	int endingOffset = i->endingOffset;
	pdfFile.clear(); // Dirty Clear : in future this function will be rewritten to handle streams and avoid seek corruption
	pdfFile.seekg(offset);
	vector<char> objData(endingOffset - offset);
	pdfFile.read(&objData[0], endingOffset - offset);

	// Convert vector into string
	string objectData(objData.begin(), objData.end());
	objectData = trim(objectData);
	objectData = regex_replace(objectData, regex("\n"), "");

	string temp;

	objectData = loadDictionary(objectData);

	// Extract Page MediaBox
	if (objectData.find("/MediaBox") != string::npos)
	{
		temp = objectData;
		temp = temp.substr(temp.find("/MediaBox") + 1);
		// Extract the first relevant arrray
		auto arrayStart = "\\["s;
		auto arrayEnd = "\\]"s;
		regex aObj(arrayStart + "(.*)" + "[\\S\\s]*" + arrayEnd);
		smatch aMatch;
		if (regex_search(temp, aMatch, aObj)) // IF expression found i.e. array found
		{
			temp = aMatch.str();
			// Feed the data to MediaBox Array
			for (int i = 0; i < 4; i++)
			{
				if (temp[0] == ' ') temp = temp.substr(1);
				if (temp[0] == '[') temp = temp.substr(1);
				if (temp[temp.size()] == ']') temp = temp.substr(0, temp.size() - 1);
				if (temp[0] == ' ') temp = temp.substr(1); // Recleaning
				spdfPages.pageObjects[pg].pageSize[i] = stoi(temp.substr(0, temp.find_first_of(' ')));
				temp = temp.substr(temp.find_first_of(' ') + 1);
			}
		}
	}

	// Extract Fonts Location
	/*
	We have two cases for font extraction. Either the font is stored as direct reference like /Font 5 0 R
	or in a dictionary /Font <<\F1 5 0 R\F2 6 0 R>>.
	Furthermore some PDFs have /Resources flag which refer to page resources like fonts and images so we'll cater them too!
	*/
	/*
	To get to font we first need to find the /Resources flag. If multiple resources are present they are stored as dictionary. Otherwise a
	single reference will be found with no idea if it's a font or a image or anything basically (for files which have a SINGLE resource)
	*/
	// TODO : Are RESOURCES STORED AS AN ARRAY?

	// Constructing a Regex for the case /Resources<< ..... >>/
	auto startResources = "/Resources"s;
	auto endResources = ">>/"s;
	regex regExResources(startResources + "(.*)" + "[\\S\\s]*" + endResources);
	smatch rsMatch;
	bool foundRes = false;
	bool fndFonts = false;
	
	objectData = regex_replace(objectData, regex("\n"), "");

	while (regex_search(objectData, rsMatch, regExResources))
	{
		temp = rsMatch.str(); // We now have the string which will surely contain /Resources<<
		foundRes = true;
		break; // first case needed only
	}

	if (foundRes == true)
	{
		// Search for /Font in the resultant temp string
		if (temp.find("/Font") != string::npos) temp = temp.substr(temp.find("/Font") + 5);
		else return false; // Fonts didnt't successfully loaded up TODO Add exceptions

		 // Now we'll extract the Font Dictionary
		temp = temp.substr(temp.find_first_of("<<") + 2);
		temp = temp.substr(0, temp.find_first_of(">>"));

		// Perform cleaning
		regex reference(" 0 R");
		temp = regex_replace(temp, reference, "");
		regex fontRef("/F([0-9][0-9]*)");
		temp = regex_replace(temp, fontRef, "");
		// For Truetype
		regex fontRefTT("/TT([0-9][0-9]*)");
		temp = regex_replace(temp, fontRefTT, "");

		// Feed the data
		spdfPages.pageObjects[pg].totalFonts = 0;
		while (temp != "")
		{
			if (temp[0] == ' ') temp = temp.substr(1);
			fontDef entry;
			entry.fontObject = stoi(temp.substr(0, temp.find_first_of(' ')));
			spdfPages.pageObjects[pg].totalFonts++;
			spdfPages.pageObjects[pg].fontObjects.push_back(entry);
			if (temp.find(" ") == string::npos) break;
			temp = temp.substr(temp.find_first_of(' ') + 1);
		}
		fndFonts = true; // we successfully found the fonts
	}
	else
	{
		temp = objectData;
		// Simply find /Resources tag and load up the object
		if (temp.find("/Resources") != string::npos)
		{
			temp = temp.substr(temp.find("/Resources") + 10);
			if (temp[0] == ' ') temp = temp.substr(1);
			int objectRef = stoi(temp.substr(0, temp.find_first_of(" ")));

			// Goto object
			vector<xrefEntry>::iterator tempPos = find_if(spdfXref.begin(), spdfXref.end(), [objectRef](const xrefEntry &a) {return a.objectNumber == objectRef; });
			if (tempPos != spdfXref.end())
			{
				int offset = tempPos->startingOffset;
				int endingOffset = tempPos->endingOffset;
				pdfFile.clear();
				pdfFile.seekg(offset, ios::beg);

				vector<char> objData(endingOffset - offset);
				pdfFile.read(&objData[0], endingOffset - offset);

				// Convert vector into string
				string objectData(objData.begin(), objData.end());
				objectData = trim(objectData);
				objectData = regex_replace(objectData, regex("\n"), "");

				objectData = loadDictionary(objectData);

				// Perform a simple search and extract the required information
				if (objectData.find("/Font<<") != string::npos)
				{
					// Now we'll extract the Font Dictionary
					temp = temp.substr(temp.find_first_of("<<") + 2);
					temp = temp.substr(0, temp.find_first_of(">>"));

					// Perform cleaning
					regex reference(" 0 R");
					temp = regex_replace(temp, reference, "");
					regex fontRef("/F([0-9][0-9]*)");
					temp = regex_replace(temp, fontRef, "");
					// For Truetype
					regex fontRefTT("/TT([0-9][0-9]*)");
					temp = regex_replace(temp, fontRef, "");

					// Feed the data
					spdfPages.pageObjects[pg].totalFonts = 0;
					while (temp != "")
					{
						if (temp[0] == ' ') temp = temp.substr(1);
						fontDef entry;
						entry.fontObject = stoi(temp.substr(0, temp.find_first_of(' ')));
						spdfPages.pageObjects[pg].totalFonts++;
						spdfPages.pageObjects[pg].fontObjects.push_back(entry);
						if (temp.find(" ") == string::npos) break;
						temp = temp.substr(temp.find_first_of(' ') + 1);
					}
					fndFonts = true; // we successfully found the fonts
					
				}
				else if (objectData.find("/Font") != string::npos)
				{
					objectData = objectData.substr(objectData.find("/Font") + 5);
					if (objectData[0] == ' ') objectData = objectData.substr(1);
					spdfPages.pageObjects[pg].totalFonts = 1;
					int tempPos = stoi(objectData.substr(0, objectData.find_first_of(" ")));
					
					vector<xrefEntry>::iterator iter = find_if(spdfXref.begin(), spdfXref.end(), [tempPos](const xrefEntry &a) {return a.objectNumber == tempPos; });

					int offset = iter->startingOffset;
					int endingOffset = iter->endingOffset;
					pdfFile.clear();
					pdfFile.seekg(offset, ios::beg);

					vector<char> objData(endingOffset - offset);
					pdfFile.read(&objData[0], endingOffset - offset);

					// Convert vector into string
					objectData = string(objData.begin(), objData.end());
					objectData = trim(objectData);
					objectData = regex_replace(objectData, regex("\n"), "");

					objectData = loadDictionary(objectData);

					objectData = objectData.substr(objectData.find("/F0 ") + 4);

					fontDef entry;
					entry.fontObject = stoi(objectData.substr(0, objectData.find(" ")));
					spdfPages.pageObjects[pg].fontObjects.push_back(entry);
					spdfPages.pageObjects[pg].totalFonts = 1;
					fndFonts = true;
				}
			}
		}
	}
	return fndFonts;
}

// Feed data to the fonts contained in a page
void spdf::loadFontInformation(int pg)
{
	// A simple loop will suffice here to do our job
	for (int i = 0; i < spdfPages.pageObjects[pg].totalFonts; i++)
	{
		int fntObj = spdfPages.pageObjects[pg].fontObjects[i].fontObject;
		vector<xrefEntry>::iterator fontPos = find_if(spdfXref.begin(), spdfXref.end(), [fntObj](const xrefEntry &a) {return a.objectNumber == fntObj; }); // Check whether the font object entry exits in Xref - UI -
		if (fontPos != spdfXref.end())
		{
			// Font found!
			int offset = fontPos->startingOffset;
			int endingOffset = fontPos->endingOffset;
			pdfFile.clear(); // Dirty Clear : in future this function will be rewritten to handle streams and avoid seek corruption
			pdfFile.seekg(offset);
			vector<char> objData(endingOffset - offset);
			pdfFile.read(&objData[0], endingOffset - offset);

			// Convert vector into string
			string objectData(objData.begin(), objData.end());
			objectData = trim(objectData);

			// Load dictionary
			objectData = loadDictionary(objectData);
			objectData = regex_replace(objectData, regex("\n"), "");

			string temp(objectData);

			// Simple check to ensure it's a font object
			if (temp.find("/Type/Font") != string::npos || temp.find("/Type /Font") != string::npos
				|| temp.find("/Type\n/Font") != string::npos || temp.find("/Type\r/Font") != string::npos)
			{
				// Load Basefont
				temp = temp.substr(temp.find("/BaseFont") + 9);
				regex seq("/");
				temp = regex_replace(temp, seq, " ");
				while (true)
				{
					if (temp[0] == ' ') temp = temp.substr(1);
					else break;
				}
				temp = temp.substr(0,temp.find_first_of(" "));
				temp = cleanFontDescriptors(temp);
				if (temp[0] == ' ') temp = temp.substr(1);
				spdfPages.pageObjects[pg].fontObjects[i].baseFont = spdf::trim(temp);

				// Load Subtype
				temp = objectData;
				temp = temp.substr(temp.find("/Subtype") + 8);
				temp = regex_replace(temp, regex("/"), " ");
				while (true)
				{
					if (temp[0] == ' ') temp = temp.substr(1);
					else break;
				}
				temp = temp.substr(0,temp.find_first_of(" "));
				spdfPages.pageObjects[pg].fontObjects[i].fontType = spdf::trim(temp);
			}
		}
	}
}