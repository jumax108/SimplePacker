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
		stFileHeader header;
		BYTE* data;
	};
#pragma pack()

public:

	void packing(stFile* files, DWORD fileNum, WCHAR* outputFileName);
	void unpackingAll(WCHAR* packedFileName);
	void unpackingSingleFile(WCHAR* packedFileName, int index);
	void readHeader(WCHAR* packedFileName);
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
		_headers = nullptr;
		_files = nullptr;
		_fileNum = 0;
	}
	~CSimplePacker() {
		for (DWORD fileCnt = 0; fileCnt < _fileNum; ++fileCnt) {
			free(_headers[fileCnt].name);
		}
		free(_headers);
		free(_files);
	}

	stFileHeader* _headers;
	stFile* _files;

private:

	DWORD _fileNum;

};