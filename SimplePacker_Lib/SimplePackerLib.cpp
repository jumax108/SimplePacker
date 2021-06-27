#include <iostream>
#include <Windows.h>
#include <vector>
#include "SimplePackerLib.h"

using namespace std;

bool CSimplePacker::packing(stFile* files, DWORD fileNum, WCHAR* outputFileName) {

	FILE* resultFile;
	_wfopen_s(&resultFile, outputFileName, L"wb");
	if (resultFile == nullptr) {
		return false;
	}
	fwrite(&fileNum, sizeof(DWORD), 1, resultFile);

	DWORD* size = new DWORD[fileNum];
	unsigned __int64 offset = sizeof(DWORD);
	for (DWORD fileCnt = 0; fileCnt < fileNum; ++fileCnt) {
		stFileHeader* header = files[fileCnt].header;
		DWORD headerSize = sizeof(stFileHeader) - sizeof(WCHAR*) + sizeof(WCHAR) * header->nameLen;
		offset += headerSize;
	}

	for (DWORD fileCnt = 0; fileCnt < fileNum; ++fileCnt) {
		stFileHeader* header = files[fileCnt].header; 
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

	return true;
}

bool CSimplePacker::readHeader(WCHAR* packedFileName) {

	FILE* packedFile;
	_wfopen_s(&packedFile, packedFileName, L"rb");
	if (packedFile == nullptr) {
		return false;
	}

	fread(&_fileNum, sizeof(DWORD), 1, packedFile);

	if (_files == nullptr) {

		_files = (stFile*)malloc(sizeof(stFile) * _fileNum);
		if (_files == nullptr) {
			return false;
		}
		ZeroMemory(_files, sizeof(stFile) * _fileNum);

	}

	for (DWORD fileCnt = 0; fileCnt < _fileNum; ++fileCnt) {
		stFile* file = &_files[fileCnt];
		file->header = (stFileHeader*)malloc(sizeof(stFileHeader));
		if (file->header == nullptr) {
			return false;
		}

		stFileHeader* header = file->header;
		fread(&header->nameLen, sizeof(DWORD), 1, packedFile);
		unsigned __int64 nameSize = sizeof(WCHAR) * ((unsigned __int64)header->nameLen + 1);
		header->name = (WCHAR*)malloc(nameSize);
		if (header->name == nullptr) {
			return false;
		}
		header->name[header->nameLen] = '\0';
		fread_s(header->name, nameSize, sizeof(WCHAR), header->nameLen, packedFile);
		fread(&header->fileSize, sizeof(DWORD), 1, packedFile);
		fread(&header->offset, sizeof(unsigned __int64), 1, packedFile);
		
	}
	
	fclose(packedFile);

	return true;
}

bool CSimplePacker::readDataAll(WCHAR* packedFileName) {


	if (_files != nullptr) {
		this->~CSimplePacker();
		_fileNum = 0;
		_files = nullptr;
	}

	if (_fileNum == 0) {
		readHeader(packedFileName);
	}

	if (_files == nullptr) {
		return false;
	}

	FILE* packedFile;
	_wfopen_s(&packedFile, packedFileName, L"rb");
	if (packedFile == nullptr) {
		return false;
	}
	_fseeki64(packedFile, _files->header->offset, SEEK_SET);

	for (DWORD fileCnt = 0; fileCnt < _fileNum; ++fileCnt) {

		stFile* file = &_files[fileCnt];
		stFileHeader* header = file->header;
		unsigned __int64 dataBufSize = header->fileSize;
		file->data = (BYTE*)malloc(dataBufSize + 1);
		if (file->data == nullptr) {
			return false;
		}
		file->data[dataBufSize] = '\0';
		fread(file->data, dataBufSize, 1, packedFile);
	}

	fclose(packedFile);

	return true;
}

bool CSimplePacker::readDataSingleFile(WCHAR* packedFileName, int index) {

	if (_fileNum == 0) {
		readHeader(packedFileName);
	}

	if (_files == nullptr) {
		_files = (stFile*)malloc(sizeof(stFile) * _fileNum);
		if (_files == nullptr) {
			return false;
		}
		ZeroMemory(_files, sizeof(stFile) * _fileNum);
	}

	stFile* file = &_files[index];
	stFileHeader* header = file->header;

	if (file->data == nullptr) {

		unsigned __int64 dataBufSize = header->fileSize;
		file->data = (BYTE*)malloc(dataBufSize + 1);
		if (file->data == nullptr) {
			return false;
		}
		file->data[dataBufSize] = '\0';

		FILE* packedFile;
		_wfopen_s(&packedFile, packedFileName, L"rb");
		if (packedFile == nullptr) {
			return false;
		}
		_fseeki64(packedFile, header->offset, SEEK_SET);

		fread(file->data, dataBufSize, 1, packedFile);

		fclose(packedFile);
	}

	return true;
}