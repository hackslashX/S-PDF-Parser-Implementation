//
//	SPDF IMAGE EXTRACTION
// 

#include "spdf-structure.h"

using namespace std;

// FreeImage Utility Function
/*
	Swaps Red Blue channels on 24 and 32 bpp raw images
*/
BOOL SwapRedBlue32a(FIBITMAP* dib) {
	if (FreeImage_GetImageType(dib) != FIT_BITMAP) {
		return FALSE;
	}

	const unsigned bytesperpixel = FreeImage_GetBPP(dib) / 8;
	if (bytesperpixel > 4 || bytesperpixel < 3) {
		return FALSE;
	}

	const unsigned height = FreeImage_GetHeight(dib);
	const unsigned pitch = FreeImage_GetPitch(dib);
	const unsigned lineSize = FreeImage_GetLine(dib);

	BYTE* line = FreeImage_GetBits(dib);
	for (unsigned y = 0; y < height; ++y, line += pitch) {
		for (BYTE* pixel = line; pixel < line + lineSize; pixel += bytesperpixel) {
			INPLACESWAP(pixel[0], pixel[2]);
		}
	}

	return TRUE;
}

bool spdf::loadPageImage(int pg)
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

	// Constructing a Regex for the case /Resources<< ..... >>/
	auto startResources = "/Resources"s;
	auto endResources = ">>/"s;
	regex regExResources(startResources + "(.*)" + "[\\S\\s]*" + endResources);
	smatch rsMatch;
	bool foundRes = false;
	bool fndImage = false;

	objectData = regex_replace(objectData, regex("\n"), "");

	while (regex_search(objectData, rsMatch, regExResources))
	{
		temp = rsMatch.str(); // We now have the string which will surely contain /Resources<<
		foundRes = true;
		break; // first case needed only
	}

	if (foundRes == true)
	{
		// Search for /XObject in the resultant temp string
		if (temp.find("/XObject") != string::npos) temp = temp.substr(temp.find("/XObject") + 5);
		else return false; // Image Object Lookup didnt't successfully loaded up TODO Add exceptions

		temp = temp.substr(temp.find_first_of("<<") + 2);
		temp = temp.substr(0, temp.find_first_of(">>"));

		// Perform cleaning
		regex reference(" 0 R");
		temp = regex_replace(temp, reference, "");
		regex imgRef("/Im([0-9][0-9]*)");
		temp = regex_replace(temp, imgRef, "");

		// Feed the data
		spdfPages.pageObjects[pg].totalImages = 0;
		while (temp != "")
		{
			if (temp[0] == ' ') temp = temp.substr(1);
			imageDef entry;
			entry.imageObject = stoi(temp.substr(0, temp.find_first_of(' ')));
			spdfPages.pageObjects[pg].totalImages++;
			spdfPages.pageObjects[pg].imageObjects.push_back(entry);
			if (temp.find(" ") == string::npos) break;
			temp = temp.substr(temp.find_first_of(' ') + 1);
		}
		fndImage = true; // we successfully found the images
	}

	return fndImage;
}

bool spdf::loadImageInformation(int pg)
{
	// A simple loop will suffice here to do our job
	for (int i = 0; i < spdfPages.pageObjects[pg].totalImages; i++)
	{
		int imgObj = spdfPages.pageObjects[pg].imageObjects[i].imageObject;
		vector<xrefEntry>::iterator imgPos = find_if(spdfXref.begin(), spdfXref.end(), [imgObj](const xrefEntry &a) {return a.objectNumber == imgObj; });
		if (imgPos != spdfXref.end())
		{
			// Image object located
			int offset = imgPos->startingOffset;
			int endingOffset = imgPos->endingOffset;
			pdfFile.clear(); // Dirty Clear : in future this function will be rewritten to handle streams and avoid seek corruption
			pdfFile.seekg(offset);
			vector<char> objData(endingOffset - offset);
			pdfFile.read(&objData[0], endingOffset - offset);

			// Convert vector into string
			string objectData(objData.begin(), objData.end());
			objectData = trim(objectData);

			objectData = objectData.substr(0, objectData.find("stream"));

			// Load dictionary
			objectData = loadDictionary(objectData);
			objectData = regex_replace(objectData, regex("\n"), "");

			string temp(objectData);

			// Simple check to ensure it's an image object
			if (temp.find("/Subtype/Image") != string::npos || temp.find("/Subtype /Image") != string::npos
				|| temp.find("/Subtype\n/Image") != string::npos || temp.find("/Subtype\r/Image") != string::npos)
			{
				// Load Image Properties
				temp = temp.substr(temp.find("/Length") + 7);
				regex seq("/");
				temp = regex_replace(temp, seq, " ");
				while (true)
				{
					if (temp[0] == ' ') temp = temp.substr(1);
					else break;
				}
				temp = temp.substr(0, temp.find_first_of(" "));
				
				if (temp[0] == ' ') temp = temp.substr(1);
				spdfPages.pageObjects[pg].imageObjects[i].imageLength = stoi(spdf::trim(temp));

				// Load Bits per Component
				temp = objectData;
				temp = temp.substr(temp.find("/BitsPerComponent") + 17);
				temp = regex_replace(temp, regex("/"), " ");
				while (true)
				{
					if (temp[0] == ' ') temp = temp.substr(1);
					else break;
				}
				temp = temp.substr(0, temp.find_first_of(" "));
				spdfPages.pageObjects[pg].imageObjects[i].bitsPerComponent = stoi(spdf::trim(temp));

				// Load Color Space
				temp = objectData;
				temp = temp.substr(temp.find("/ColorSpace") + 11);
				temp = regex_replace(temp, regex("/"), " ");
				while (true)
				{
					if (temp[0] == ' ') temp = temp.substr(1);
					else break;
				}
				temp = temp.substr(0, temp.find_first_of(" "));
				spdfPages.pageObjects[pg].imageObjects[i].colorSpace = spdf::trim(temp);

				// Load Image Width
				temp = objectData;
				temp = temp.substr(temp.find("/Width") + 6);
				temp = regex_replace(temp, regex("/"), " ");
				while (true)
				{
					if (temp[0] == ' ') temp = temp.substr(1);
					else break;
				}
				temp = temp.substr(0, temp.find_first_of(" "));
				spdfPages.pageObjects[pg].imageObjects[i].width = stoi(spdf::trim(temp));

				// Load Image Height
				temp = objectData;
				temp = temp.substr(temp.find("/Height") + 7);
				temp = regex_replace(temp, regex("/"), " ");
				while (true)
				{
					if (temp[0] == ' ') temp = temp.substr(1);
					else break;
				}
				temp = temp.substr(0, temp.find_first_of(" "));
				spdfPages.pageObjects[pg].imageObjects[i].height = stoi(spdf::trim(temp));

				// Load the decode Array
				temp = objectData;
				temp = temp.substr(temp.find("/Decode") + 7);
				temp = regex_replace(temp, regex("/"), " ");
				temp = temp.substr(temp.find_first_of("[") + 1);
				temp = temp.substr(0, temp.find_first_of("]"));
				spdfPages.pageObjects[pg].imageObjects[i].decodeArray = temp;

				// Load the applied filter
				temp = objectData;
				temp = temp.substr(temp.find("/Filter") + 7);
				temp = regex_replace(temp, regex("/"), " ");
				while (true)
				{
					if (temp[0] == ' ') temp = temp.substr(1);
					else break;
				}
				temp = temp.substr(0, temp.find_first_of(" "));
				spdfPages.pageObjects[pg].imageObjects[i].filter = spdf::trim(temp);
				
				// Load up the Image data into a stream
				string tempStream(objData.begin(), objData.end());
				int ssStart = tempStream.find("stream") + imgPos->startingOffset + 8;
				int ssEnd = imgPos->endingOffset - 11;
				BYTE * stream = new BYTE[spdfPages.pageObjects[pg].imageObjects[i].imageLength];

				// C Style File Handling to read unsigned char buffer
				FILE *fil;
				fil = fopen(pdfPath, "rb");
				fseek(fil, ssStart, ios::beg);
				fread(stream, 1, spdfPages.pageObjects[pg].imageObjects[i].imageLength, fil);
				fclose(fil);

				// Decompress the stream
				BYTE *dest = new BYTE[spdfPages.pageObjects[pg].imageObjects[i].imageLength*5];
				FreeImage_ZLibUncompress(dest, spdfPages.pageObjects[pg].imageObjects[i].imageLength * 5, stream, spdfPages.pageObjects[pg].imageObjects[i].imageLength);
				
				// Process and Save the Image
				FIBITMAP * pic = FreeImage_ConvertFromRawBits(dest, spdfPages.pageObjects[pg].imageObjects[i].width, spdfPages.pageObjects[pg].imageObjects[i].height, spdfPages.pageObjects[pg].imageObjects[i].width*3, spdfPages.pageObjects[pg].imageObjects[i].bitsPerComponent*3, 0U, 0U, 0U, 1L);
				string imgPth = pdfPath + to_string(pg+1);
				imgPth.append("_");
				imgPth.append(to_string(i + 1));
				imgPth.append(".png");
				SwapRedBlue32a(pic); // Sawp Red Blue Channels
				FreeImage_Save(FIF_PNG, pic, imgPth.c_str());

				// Save the resulting image stream
				spdfPages.pageObjects[pg].imageObjects[i].imageStream = FreeImage_GetBits(pic);
				spdfPages.pageObjects[pg].imageObjects[i].imagePath = imgPth;

				pdfFile.clear();

				// Work Finished!
			}
		}
	}
	return false;
}