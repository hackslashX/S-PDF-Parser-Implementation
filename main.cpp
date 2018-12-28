#include "spdf-structure.h"
#include <iomanip>

using namespace std;

void showHeader()
{
	system("cls");
	cout << "==============================================\n";
	cout << "sPDF (Smart PDF) Parser (End Semester Project)\n";
	cout << "==============================================\n";
	cout << "\nsPDF is a low level pdf parser aimed to extract the very basic elements from a pdf file like pages, fonts, images,";
	cout << "etc (as per the project requirements :D)\n";
	cout << "Please feel free to use it and expand it as per your requirements.\n\n";
}

void showPageInformation(spdf * spdfObject, int pageNumber)
{
	system("cls");
	cout << "Main Menu >> Show Page Information\n----------------------\n\n";
	cout << "Page Number: " << pageNumber << endl;
	cout << "Page Object (according to Xref): " << spdfObject->spdfPages.pageObjects[pageNumber - 1].pageObject << endl;
	cout << "Parent Object: " << spdfObject->spdfPages.pageObjects[pageNumber - 1].parentObj << endl;
	cout << "Media Box: " << spdfObject->spdfPages.pageObjects[pageNumber - 1].pageSize[0] << " "
		<< spdfObject->spdfPages.pageObjects[pageNumber - 1].pageSize[1] << " "
		<< spdfObject->spdfPages.pageObjects[pageNumber - 1].pageSize[2] << " "
		<< spdfObject->spdfPages.pageObjects[pageNumber - 1].pageSize[3] << endl;
	cout << "Total Fonts: " << spdfObject->spdfPages.pageObjects[pageNumber - 1].totalFonts << endl;
	cout << "Total Images: " << spdfObject->spdfPages.pageObjects[pageNumber - 1].totalImages << endl;
}

void showPageFonts(spdf * spdfObject, int pageNumber)
{
	system("cls");
	cout << "Main Menu >> Show Page Fonts\n----------------------\n\n";
	cout << "Page Number: " << pageNumber << endl;
	cout << "Total Fonts: " << spdfObject->spdfPages.pageObjects[pageNumber - 1].totalFonts << endl;
	cout << "..................................\n\n";
	
	if (spdfObject->spdfPages.pageObjects[pageNumber - 1].totalFonts == 0) return;
	
	for (int i = 0; i < spdfObject->spdfPages.pageObjects[pageNumber - 1].totalFonts; i++)
	{
		cout << "Font Object: " << spdfObject->spdfPages.pageObjects[pageNumber - 1].fontObjects[i].fontObject << endl;
		cout << "Font Type: " << spdfObject->spdfPages.pageObjects[pageNumber - 1].fontObjects[i].fontType << endl;
		cout << "Font Name (Basefont): " << spdfObject->spdfPages.pageObjects[pageNumber - 1].fontObjects[i].baseFont << endl;
		cout << endl;
	}
}

void showPageImages(spdf * spdfObject, int pageNumber)
{
	system("cls");
	cout << "Main Menu >> Show Page Images\n----------------------\n\n";
	cout << "Page Number: " << pageNumber << endl;
	cout << "Total Images: " << spdfObject->spdfPages.pageObjects[pageNumber - 1].totalImages << endl;
	cout << "..................................\n\n";

	if (spdfObject->spdfPages.pageObjects[pageNumber - 1].totalImages == 0) return;

	for (int i = 0; i < spdfObject->spdfPages.pageObjects[pageNumber - 1].totalImages; i++)
	{
		cout << "Image Object: " << spdfObject->spdfPages.pageObjects[pageNumber - 1].imageObjects[i].imageObject << endl;
		cout << "Image Size (either compressed or decompressed): " << spdfObject->spdfPages.pageObjects[pageNumber - 1].imageObjects[i].imageLength << endl;
		cout << "Bits per Component: " << spdfObject->spdfPages.pageObjects[pageNumber - 1].imageObjects[i].bitsPerComponent << endl;
		cout << "Color Space: " << spdfObject->spdfPages.pageObjects[pageNumber - 1].imageObjects[i].colorSpace << endl;
		cout << "Compression Filter: " << spdfObject->spdfPages.pageObjects[pageNumber - 1].imageObjects[i].filter << endl;
		cout << "Width (in pixels): " << spdfObject->spdfPages.pageObjects[pageNumber - 1].imageObjects[i].width << endl;
		cout << "Height (in pixels): " << spdfObject->spdfPages.pageObjects[pageNumber - 1].imageObjects[i].height << endl;
		cout << "Decompression Algorithm: " << spdfObject->spdfPages.pageObjects[pageNumber - 1].imageObjects[i].decodeArray << endl;
		cout << "Path (where image is exported): " << spdfObject->spdfPages.pageObjects[pageNumber - 1].imageObjects[i].imagePath << endl;
		cout << endl;
	}
}

void mainMenu(spdf * spdfObject)
{
	system("cls");
	size_t option{};

	cout << "Main Menu\n----------------------\n\n";
	cout << "1. Show PDF Information\n";
	cout << "2. Show Xref Table\n";
	cout << "3. Show Page Catalog\n";
	cout << "4. Show Page Information\n";
	cout << "5. Show Page Fonts\n";
	cout << "6. Show Page Images\n";
	cout << "7. Exit\n\n";
	cout << "Enter you choice: ";
	cin >> option;

	switch(option)
	{
	case 1:
		system("cls");
		cout << "Main Menu >> PDF Information\n----------------------\n\n";
		cout << "PDF Path: " << spdfObject->pdfPath << endl;
		cout << "PDF Version: " << spdfObject->pdfVersion << endl;
		cout << endl;
		system("pause");
		break;

	case 2:
		system("cls");
		cout << "Main Menu >> Show Xref Table\n----------------------\n\n";
		cout << setw(20) << "Object Number" << setw(20) << "Starting Offset" << setw(20) << "Ending Offset" << setw(20) << "Generation" << setw(20) << "Object Type\n";
		for (vector<xrefEntry>::iterator i = spdfObject->spdfXref.begin(); i != spdfObject->spdfXref.end(); i++)
		{
			cout << setw(20) << i->objectNumber << setw(20) << i->startingOffset << setw(20) << i->endingOffset << setw(20) << i->objectGeneration
				<< setw(20) << i->objectType << endl;
		}
		cout << endl;
		system("pause");
		break;

	case 3:
		system("cls");
		cout << "Main Menu >> Page Catalog\n----------------------\n\n";
		cout << "Root Catalog Object: " << spdfObject->spdfPages.pageCatalogObject << endl;
		cout << "Object Containing Kids Information: " << spdfObject->spdfPages.pageKidsObject << endl;
		cout << "Total Pages: " << spdfObject->spdfPages.totalPages << endl;
		cout << endl;
		system("pause");
		break;

	case 4:
		cout << "Enter the page number: ";
		cin >> option;
		if (option <= spdfObject->spdfPages.totalPages)
		{
			showPageInformation(spdfObject, option);
			cout << endl;
		}
		else
		{
			cout << "\nInvalid Option\n";
			cout << endl;
		}
		system("pause");
		break;

	case 5:
		cout << "Enter the page number: ";
		cin >> option;
		if (option <= spdfObject->spdfPages.totalPages)
		{
			showPageFonts(spdfObject, option);
			cout << endl;
		}
		else
		{
			cout << "\nInvalid Option\n";
			cout << endl;
		}
		system("pause");
		break;

	case 6:
		cout << "Enter the page number: ";
		cin >> option;
		if (option <= spdfObject->spdfPages.totalPages)
		{
			showPageImages(spdfObject, option);
			cout << endl;
		}
		else
		{
			cout << "\nInvalid Option\n";
			cout << endl;
		}
		system("pause");
		break;

	case 7:
		exit(1);

	default:
		cout << "\nInvalid Option!\n";
		system("pause");
		return;
	}
}

int main(void)
{
	showHeader();
	cout << endl << "Enter the path to the PDF file: ";
	string pathToFile{};
	cin >> pathToFile;

	cout << "\n* Loading File ...\n";
	spdf *mainPDF;
	try {
		mainPDF = new spdf(pathToFile.c_str());
		cout << "* Loading PDF structure\n";
		mainPDF->init(); // Start initialization
	}
	catch (string &error)
	{
		cout << "Exception Thrown: " << error << endl;
	}

	// Call Show Header again
	while (true)
	{
		mainMenu(mainPDF);
	}
	
	return 0;
}