#pragma once

#pragma pack(1)
struct stFileHeader {
	DWORD nameLen;
	WCHAR* name;
	DWORD size;
};

struct stFile {
	stFileHeader header;
	BYTE* data;
};
#pragma pack()

void packing(stFile* files, DWORD fileNum, WCHAR* outputFileName);
void readHeader(WCHAR* packedFileName);
DWORD getPackingFileNum(WCHAR* packedFileName);
WCHAR** getFilesName(WCHAR* packedFileName);
stFile* unpacking(WCHAR* packedFileName);