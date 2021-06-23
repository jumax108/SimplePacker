#include <iostream>
#include <Windows.h>
#include <vector>
#include "SimplePackerLib.h"

using namespace std;

DWORD fileNum = 0;
stFileHeader* headers;
DWORD totalHeaderSize = 0;


void packing(stFile* files, DWORD fileNum, WCHAR* outputFileName) {
	FILE* resultFile;
	_wfopen_s(&resultFile, outputFileName, L"wb");
	
	fwrite(&fileNum, sizeof(DWORD), 1, resultFile);

	DWORD* size = new DWORD[fileNum];

	for (DWORD fileCnt = 0; fileCnt < fileNum; ++fileCnt) {
		stFileHeader* head = &files[fileCnt].header;
		fwrite(&head->nameLen, sizeof(DWORD), 1, resultFile);
		fwrite(head->name, sizeof(WCHAR), head->nameLen, resultFile);
		fwrite(&head->size, sizeof(DWORD), head->size, resultFile);
		size[fileCnt] = head->size;
	}

	for (DWORD fileCnt = 0; fileCnt < fileNum; ++fileCnt) {
		stFile* file = &files[fileCnt];
		fwrite(file->data, sizeof(BYTE), size[fileCnt], resultFile);
	}
	
	fclose(resultFile);
}

void readHeader(WCHAR* packedFileName) {

	FILE* packedFile;
	_wfopen_s(&packedFile, packedFileName, L"rb");

	fread(&fileNum, sizeof(DWORD), 1, packedFile);

	headers = (stFileHeader*)malloc(sizeof(stFileHeader) * fileNum);

	for (DWORD fileCnt = 0; fileCnt < fileNum; ++fileCnt) {
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

	for (DWORD fileCnt = 0; fileCnt < fileNum; ++fileCnt) {
		names[fileCnt] = headers[fileCnt].name;
	}

	return names;

}

stFile* unpacking(WCHAR* packedFileName) {

	if (fileNum == 0) {
		readHeader(packedFileName);
	}
	
	stFile* files = (stFile*)malloc(sizeof(stFile) * fileNum);

	FILE* packedFile;
	_wfopen_s(&packedFile, packedFileName, L"rb");
	fseek(packedFile, totalHeaderSize, SEEK_CUR);

	for (DWORD fileCnt = 0; fileCnt < fileNum; ++fileCnt) {

		files[fileCnt].header = headers[fileCnt];
		files[fileCnt].data = (BYTE*)malloc(sizeof(BYTE) * files[fileCnt].header.size);
		fread(files[fileCnt].data, sizeof(BYTE), files[fileCnt].header.size, packedFile);
	}

	return files;

}