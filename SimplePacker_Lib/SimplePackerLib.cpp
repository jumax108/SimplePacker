#include <iostream>
#include <Windows.h>
#include <vector>
#include "SimplePackerLib.h"

using namespace std;

void CSimplePacker::packing(stFile* files, DWORD fileNum, WCHAR* outputFileName) {
	
	FILE* resultFile;
	_wfopen_s(&resultFile, outputFileName, L"wb");
	fwrite(&fileNum, sizeof(DWORD), 1, resultFile);

	DWORD* size = new DWORD[fileNum];
	unsigned __int64 offset = sizeof(DWORD);
	for (DWORD fileCnt = 0; fileCnt < fileNum; ++fileCnt) {
		stFileHeader* header = &files[fileCnt].header;
		DWORD headerSize = sizeof(stFileHeader) - sizeof(WCHAR*) + sizeof(WCHAR) * header->nameLen;
		offset += headerSize;
	}

	for (DWORD fileCnt = 0; fileCnt < fileNum; ++fileCnt) {
		stFileHeader* header = &files[fileCnt].header; 
		header->offset = offset;
		fwrite(&header->nameLen, sizeof(DWORD), 1, resultFile);
		fwrite(header->name, sizeof(WCHAR), header->nameLen, resultFile);
		fwrite(&header->fileSize, sizeof(DWORD), 1, resultFile);
		fwrite(&header->offset, sizeof(unsigned __int64), 1, resultFile);
		size[fileCnt] = header->fileSize;
		offset += header->fileSize;
	}

	for (DWORD fileCnt = 0; fileCnt < fileNum; ++fileCnt) {
		stFile* file = &files[fileCnt];
		fwrite(file->data, sizeof(BYTE), size[fileCnt], resultFile);
	}
	
	fclose(resultFile);
}

void CSimplePacker::readHeader(WCHAR* packedFileName) {

	FILE* packedFile;
	_wfopen_s(&packedFile, packedFileName, L"rb");

	fread(&_fileNum, sizeof(DWORD), 1, packedFile);

	_headers = (stFileHeader*)malloc(sizeof(stFileHeader) * _fileNum);

	for (DWORD fileCnt = 0; fileCnt < _fileNum; ++fileCnt) {
		stFileHeader* header = &_headers[fileCnt];
		
		fread(&header->nameLen, sizeof(DWORD), 1, packedFile);
		header->name = (WCHAR*)malloc(sizeof(WCHAR) * (header->nameLen + 1));
		header->name[header->nameLen] = '\0';
		fread(header->name, sizeof(WCHAR), header->nameLen, packedFile);
		fread(&header->fileSize, sizeof(DWORD), 1, packedFile);
		fread(&header->offset, sizeof(unsigned __int64), 1, packedFile);
		
	}
	
	fclose(packedFile);
}

void CSimplePacker::unpackingAll(WCHAR* packedFileName) {

	if (_fileNum == 0) {
		readHeader(packedFileName);
	}

	if (_files != nullptr) {
		free(_files);
	}
	_files = (stFile*)malloc(sizeof(stFile) * _fileNum);
	ZeroMemory(_files, sizeof(stFile) * _fileNum);

	FILE* packedFile;
	_wfopen_s(&packedFile, packedFileName, L"rb");
	fseek(packedFile, _headers[0].offset, SEEK_SET);

	for (DWORD fileCnt = 0; fileCnt < _fileNum; ++fileCnt) {

		memcpy(&_files[fileCnt].header, &_headers[fileCnt], sizeof(stFileHeader));
		_files[fileCnt].data = (BYTE*)malloc(_files[fileCnt].header.fileSize);
		_files[fileCnt].data[_files[fileCnt].header.fileSize] = '\0';
		fread(_files[fileCnt].data, _files[fileCnt].header.fileSize, 1, packedFile);
	}

	fclose(packedFile);
}

void CSimplePacker::unpackingSingleFile(WCHAR* packedFileName, int index) {
	if (_fileNum == 0) {
		readHeader(packedFileName);
	}

	if (_files == nullptr) {
		_files = (stFile*)malloc(sizeof(stFile) * _fileNum);
		ZeroMemory(_files, sizeof(stFile) * _fileNum);
	}

	stFile* file = &_files[index];
	stFileHeader* header = &_headers[index];

	if (file->data == nullptr) {

		memcpy(&file->header, header, sizeof(stFileHeader));

		file->data = (BYTE*)malloc(header->fileSize + 1);
		file->data[header->fileSize] = '\0';

		FILE* packedFile;
		_wfopen_s(&packedFile, packedFileName, L"rb");
		_fseeki64(packedFile, header->offset, SEEK_SET);

		fread(file->data, header->fileSize, 1, packedFile);

		fclose(packedFile);
	}
}