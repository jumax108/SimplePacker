#pragma once

class CSimplePacker {

public:

#pragma pack(1)
	struct stFileHeader {
		DWORD nameLen;
		WCHAR* name;
		DWORD fileSize;
		unsigned __int64 offset;
	};

	struct stFile {
		stFileHeader* header;
		BYTE* data;
	};
#pragma pack()

public:

	bool packing(stFile* files, DWORD fileNum, WCHAR* outputFileName);
	bool readDataAll(WCHAR* packedFileName);
	bool readDataSingleFile(WCHAR* packedFileName, int index);
	bool readHeader(WCHAR* packedFileName);
	inline bool haveFileData(int index) {

		if (_files == nullptr) {
			return false;
		}

		return _files[index].data != NULL;
	}

	inline DWORD fileNum(WCHAR* packedFileName) {

		if (_fileNum == 0) {
			readHeader(packedFileName);
		}
		return _fileNum;

	}

	CSimplePacker() {
		_files = nullptr;
		_fileNum = 0;
	}
	~CSimplePacker() {
		for (DWORD fileCnt = 0; fileCnt < _fileNum; ++fileCnt) {
			free(_files[fileCnt].header->name);
		}
		free(_files);
	}

	//stFileHeader* _headers;
	stFile* _files;

private:

	DWORD _fileNum;

};