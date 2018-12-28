/*

	==============================================
	sPDF (Smart PDF) Parser (End Semester Project)
	==============================================

	sPDF is a low level pdf parser aimed to extract the very basic elements from a pdf file like pages, fonts, images, 
	etc (as per the project requirements :D)
	Please feel free to use it and expand it as per your requirements.

	Developers : Muhammad Fahad Baig, Saad Aftab, Taimur Baber

	--------------------------------------------------------------------------------------------------------------------------------------
	This code is licensed under the GPL (General Public License) 3.0. Please read the license carefully before using this library
	or ammending it any way. The authors shall not be responsible for any physical or virtual damage caused due to use of this program.
	USE IT AT YOUR OWN DISCRETION!

*/


#ifndef SPDF_STRUCTURE_H
#define SPDF_STRUCTURE_H

// Common Includes
#include <string>
#include <fstream>
#include <vector>
#include <iostream>
#include <algorithm>
#include <string>
#include <sstream>
#include <regex>

// SPDF Common Definitions
#include "spdf-commonDefinitions.h"

// FreeImage Library Header file
#include "FreeImage.h"
#include "FreeImage-Utilities.h"

// Data Structure to hold Image information for processing
struct imageDef
{
	int imageObject; // Represents image's object, object number according to xref
	int imageLength; // Stores thwe compressed image length to help in decompression (represents buffer size)
	int bitsPerComponent; // Ratio of bits per image component to extract total chars by line and independently process them
	std::string colorSpace; // Colorspace (like RGB, sRGB, Mono) used by the image
	std::string filter; // A stream can have multiple filters but we'll be dealing with one only to avoid complexity
	int width; // Width of the image in pixels
	int height; // Height of the image in pixels
	std::string decodeArray; // Store the decompression algorithm applied on the stream
	unsigned char * imageStream; // Character buffer to hold the decompressed image for decoding, processing, etc
	std::string imagePath; // Represents where in file storage image is exported
};

// Data Structure to hold all font related information for a single instance
struct fontDef
{
	int fontObject; // Represents font object location
	std::string fontType; // Represents Font Type
	std::string baseFont; // Represents Font Name
};

// Data structure to hold all page related information for a single instance
struct pageDef
{
	int pageObject; // Represents where object is stored
	int parentObj; // Represents parent object
	int pageSize[4]; // Array of four integers that represent the page's media box
	std::vector<fontDef> fontObjects;
	int totalFonts; // To better iterate on the loop
	int pageNumber; // To better store the page number to ease information extraction
	int totalImages; // Formerly XObjects
	std::vector<imageDef> imageObjects; // Vector to hold all image definition and data
};

// Data Structure to represent the page tree hierarchy in a PDF
struct pageTree
{
	int pageCatalogObject; // Root Catalog Object
	int pageKidsObject; // Object containing kids (pages) location
	int totalPages; // Total number of kids
	std::vector<pageDef> pageObjects; // Vector to hold all page definition and data
};

struct xrefEntry
{
	int startingOffset; // Starting offset of the object
	int endingOffset; // Ending offset of the object
	int objectNumber; // Numbering the object based on location in xref
	int objectGeneration; // Object Generation (ignored as not useful)
	char objectType; // Represents whether object is free or in use
};

// Our main SPDF - PDF Parsing Implementation main class
class spdf
{
private:
	std::fstream pdfFile; // Filestream to access the pdf file
    int eofLocaction; // Useful pointer to EOF location in PDF file (ignoring multiple EOF since multiple revised files not yet supported)
	int xrefTableLocation; // Useful pointer to a xref table in PDF (ignoring multiple xref for the same reason above)

	// Supplementary PDF specfic and assistance functions
	bool performEOFCheck();
	std::string getline(std::fstream&);
	std::string trim(std::string);
	std::string cleanFontDescriptors(std::string&);

	// Xref Functions
	void loadXref(int);

	// Page Tree and Pages loading functions
	bool loadPageTree(std::vector<xrefEntry>::iterator);

	// Object Manipulation Functions
	//bool isIndirectObject(const std::string &, size_t); // Indirect Objects like # # obj - FUNCTION DISCONTINUED -
	//bool isIndirectObject(const std::string &, size_t, size_t&); // Overloaded version of the previous - FUNCTION DISCONTINUED -
	void resolveObject(std::vector<xrefEntry>::iterator);
	std::string loadDictionary(std::string&);

	// Simple Font finding and loading functions
	bool loadPageFonts(int);
	void loadFontInformation(int);

	// Simple Image finding and loading functions
	bool loadPageImage(int);
	bool loadImageInformation(int);

	// Driver function to help loading all the information easily
	void loadObjectStructure();

public:
	// Public Data Types which represent the whole PDF as a class object
	char pdfVersion[9]; // Array to hold PDF version information (really useful in more advanced implementations)
	const char * pdfPath; // Full resolved path of the currently loaded PDF
	std::vector<xrefEntry> spdfXref; // Xref table to contain object number along with their offset, generation number, type and ending offset
	pageTree spdfPages; // Represents a single instance of a page tree (multiple catalogs not yet supported)
	
	// Constructors and Destructors
    spdf(const char * pathToFile);
    virtual ~spdf();

	// Initializer
    void init(); // Calls all necessary functions and routines to load up the complete PDF file to an object
};

#endif
