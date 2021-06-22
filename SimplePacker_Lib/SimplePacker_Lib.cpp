#include <iostream>6
#include <Windows.h>
#include <vector>
using namespace std;

DWORD fileNum = 0;
stFileHeader* headers;
DWORD totalHeaderSize = 0;


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

void packing(stFile* files, DWORD fileNum, WCHAR* outputFileName) {
	FILE* resultFile;
	_wfopen_s(&resultFile, outputFileName, L"wb");
	
	fwrite(&fileNum, sizeof(DWORD), 1, resultFile);

	DWORD* size = new DWORD[fileNum];

	for (int fileCnt = 0; fileCnt < fileNum; ++fileCnt) {
		stFileHeader* head = &files[fileCnt].header;
		fwrite(&head->nameLen, sizeof(DWORD), 1, resultFile);
		fwrite(head->name, sizeof(WCHAR), head->nameLen, resultFile);
		fwrite(&head->size, sizeof(DWORD), head->size, resultFile);
		size[fileCnt] = head->size;
	}

	for (int fileCnt = 0; fileCnt < fileNum; ++fileCnt) {
		stFile* file = &files[fileCnt];
		fwrite(file->data, sizeof(BYTE), size[fileCnt], resultFile);
	}
	
	fclose(resultFile);
}

void readHeader(WCHAR* packedFileName) {

	FILE* packedFile;
	_wfopen_s(&packedFile, packedFileName, L"rb");

	fread(&fileNum, sizeof(DWORD), 1, packedFile);

	headers = new stFileHeader[fileNum];

	for (int fileCnt = 0; fileCnt < fileNum; ++fileCnt) {
		stFileHeader* header = &headers[fileCnt];
		fread(&header->nameLen, sizeof(DWORD), 1, packedFile);
		fread(header->name, sizeof(WCHAR), header->nameLen, packedFile);
		fread(&header->size, sizeof(DWORD), 1, packedFile);

		totalHeaderSize += sizeof(DWORD) + header->nameLen * sizeof(WCHAR) + sizeof(DWORD);
	}
	
	fclose(packedFile);
}
	
DWORD getPackingFileNum(WCHAR* packedFileName) {
	
	if (fileNum == 0) {
		readHeader(packedFileName);
	}

	return fileNum;

}

WCHAR** getFilesName(WCHAR* packedFileName) {

	if (fileNum == 0) {
		readHeader(packedFileName);
	}

	WCHAR** names = (WCHAR**)malloc(sizeof(WCHAR*) * fileNum);

	for (int fileCnt = 0; fileCnt < fileNum; ++fileCnt) {
		names[fileCnt] = headers[fileCnt].name;
	}

	return names;

}

stFile* unpacking(WCHAR* packedFileName) {

	if (fileNum == 0) {
		readHeader(packedFileName);
	}


}