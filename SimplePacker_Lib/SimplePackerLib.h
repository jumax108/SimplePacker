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

	static CSimplePacker* instance() {
		static CSimplePacker inst;
		return &inst;
	}

	void packing(stFile* files, DWORD fileNum, WCHAR* outputFileName);
	void unpackingAll(WCHAR* packedFileName);
	void unpackingSingleFile(WCHAR* packedFileName, int index);
	void readHeader(WCHAR* packedFileName);

	inline DWORD fileNum(WCHAR* packedFileName) {

		if (_fileNum == 0) {
			readHeader(packedFileName);
		}
		return _fileNum;

	}

	stFileHeader* _headers;
	stFile* _files;

private:

	CSimplePacker() {
		_headers = nullptr;
		_files = nullptr;
		_fileNum = 0;
	}
	~CSimplePacker() {
		free(_headers);
		free(_files);
	}

	DWORD _fileNum;

};