////
//// SPDF IMAGE STREAMS HANDLING ROUTINE
////
//
//#include"spdf-structure.h"
//
//using namespace std;
//
//spdfStream::spdfStream(int byteOffset, imageDef* entry, fstream* src) // hopefully breaks encapsulation
//{
//	// Stream Length
//	// We'll deal with exceptions handling later
//	streamLen = entry->imageLength;
//
//	// Load filter (dealing with single filters only)
//	while (true)
//	{
//		if (entry->filter == "ASCIIHexDecode") {
//			filter = (filter < 8) | (int)SPDF_FILTER_ASCIIHEX;
//			break;
//		}
//		if (entry->filter == "ASCII85Decode") {
//			filter = (filter << 8) | (int)SPDF_FILTER_ASCII85;
//			break;
//		}
//		if (entry->filter == "LZWDecode") {
//			filter = (filter << 8) | (int)SPDF_FILTER_LZW;
//			break;
//		}
//		if (entry->filter == "RunLengthDecode") {
//			filter = (filter << 8) | (int)SPDF_FILTER_RUNLENGTH;
//			break;
//		}
//		if (entry->filter == "CCITTFaxDecode") {
//			filter = (filter << 8) | (int)SPDF_FILTER_CCITTFAX;
//			break;
//		}
//		if (entry->filter == "DCTDecode") {
//			filter = (filter << 8) | (int)SPDF_FILTER_DCT;
//			break;
//		}
//		if (entry->filter == "JPXDecode") {
//			filter = (filter << 8) | (int)SPDF_FILTER_JPX;
//			break;
//		}
//		filter == (filter < 8) | (int)SPDF_FILTER_UNKNOWN;
//		break;
//	}
//
//	// Load Stream Position 
//	src->ignore();
//	streamPos = entry->streamStart;
//	src->seekg(streamPos);
//	if (src->peek() == '\n') streamPos++;
//
//	imgEnt = entry;
//	file = src;
//
//	streamPtr = 0;
//}
//
//string spdfStream::readSomeRaw(int amount)
//{
//	if (amount == -1) amount = streamLen*streamLen;
//	if (amount > streamLen - streamPtr) amount = streamLen - streamPtr;
//
//	file->seekg(streamPos + streamPtr);
//
//	char* buffer = new char[amount];
//	file->read(buffer, amount);