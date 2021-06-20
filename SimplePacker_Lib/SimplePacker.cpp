#include <iostream>
#include <vector>
using namespace std;

#pragma pack(1)
struct SFileHeader {
	char name[256];
	unsigned int size;
};
#pragma pack()

void packing() {
	FILE* fpFileList = nullptr;
	vector<char*> fileList;
	char* buffer = nullptr;

	fopen_s(&fpFileList, "filelist.txt", "rb");
	if (fpFileList == nullptr) {
		cout << "Cannot open filelist.txt";
		return ;
	}
	fseek(fpFileList, 0, SEEK_END);
	int size = ftell(fpFileList);
	rewind(fpFileList);

	buffer = (char*)malloc(size + 1);
	if (buffer == nullptr) {
		cout << "Malloc Error";
		return;
	}
	fread(buffer, 1, size, fpFileList);
	fclose(fpFileList);
	buffer[size] = '\0';

	int fileNum = 0;
	char* fileNameBegin = &buffer[0];
	for (int bufCnt = 0; bufCnt <= size; ++bufCnt) {
		switch (buffer[bufCnt]) {
		case '\0':
		case 0x0D:

			if (&buffer[bufCnt] - fileNameBegin == 0) {
				break;
			}

			fileList.push_back((char*)malloc(256));
			if (fileList[fileNum] == nullptr) {
				cout << "Malloc Error";
				return;
			}

			for (char* pBuf = fileNameBegin; pBuf != &buffer[bufCnt]; ++pBuf) {
				fileList[fileNum][pBuf - fileNameBegin] = *pBuf;
			}
			fileNum += 1;
			fileNameBegin = &buffer[bufCnt + 2];
			break;
		}
	}
	free(buffer);
	
	char** fileData = (char**)malloc(fileNum);
	SFileHeader* fileHeader = (SFileHeader*)malloc(sizeof(SFileHeader) * fileNum);
	if (fileHeader == nullptr) {
		cout << "Malloc Error";
		return;
	}

	for (int fileCnt = 0; fileCnt < fileNum; ++fileCnt) {
		FILE* fpFile = nullptr;
		int size = 0;
		fopen_s(&fpFile, fileList[fileCnt], "rb");
		if (fpFile == nullptr) {
			cout << fileList[fileCnt] << ": Cannot open file.\n";
			return;
		}
		fseek(fpFile, 0, SEEK_END);
		size = ftell(fpFile);
		rewind(fpFile);

		fileData[fileCnt] = (char*)malloc(size);
		if (fileData[fileCnt] == nullptr) {
			cout << "Malloc Error";
			return;
		}
		fread(fileData[fileCnt], 1, size, fpFile);
		fclose(fpFile);
		fileHeader[fileCnt].size = size;
		memcpy(fileHeader[fileCnt].name, fileList[fileCnt], 255);
	}

	FILE* fileOutput = nullptr;
	fopen_s(&fileOutput, "output", "wb");
	if (fileOutput == nullptr) {
		cout << "Cannot make output file";
		return;
	}

	fwrite(&fileNum, sizeof(fileNum), 1, fileOutput);
	
	fwrite(fileHeader, sizeof(SFileHeader), fileNum, fileOutput);
	
	for (int fileCnt = 0; fileCnt < fileNum; ++fileCnt) {
		fwrite(fileData[fileCnt], fileHeader[fileCnt].size, 1, fileOutput);
	}

	fclose(fileOutput);

	free(fileHeader);
	for (int fileCnt = 0; fileCnt < fileNum; ++fileCnt) {
		free(fileList[fileCnt]);
		free(fileData[fileCnt]);
	}
	free(fileData);
}

void unpacking() {
	FILE* packedFile = nullptr;
	fopen_s(&packedFile, "output", "rb");
	if (packedFile == nullptr) {
		return;
	}

	int fileNum = 0;
	fread(&fileNum, sizeof(fileNum), 1, packedFile);
	SFileHeader* fileHeader = nullptr;
	char** fileData = nullptr;

	fileHeader = (SFileHeader*)malloc(sizeof(SFileHeader) * fileNum);
	if (fileHeader == nullptr) {
		cout << "Malloc Error";
		return;
	}

	fread(fileHeader, sizeof(SFileHeader), fileNum, packedFile);

	fileData = (char**)malloc(fileNum);
	if (fileData == nullptr) {
		cout << "malloc fail";
		return;
	}

	for (int fileCnt = 0; fileCnt < fileNum; ++fileCnt) {
		fileData[fileCnt] = (char*)malloc(fileHeader[fileCnt].size);
		if (fileData[fileCnt] == nullptr) {
			cout << "Malloc Error";
			return;
		}
		fread(fileData[fileCnt], fileHeader[fileCnt].size, 1, packedFile);
		FILE* makeFile = nullptr;
		char fileName[257] = {0,};
		memcpy(fileName, fileHeader[fileCnt].name, 255);
		fopen_s(&makeFile, fileName, "wb");
		if (makeFile == nullptr) {
			cout << fileHeader[fileCnt].name << ": 파일을 열 수 없습니다.";
			break;
		}
		fwrite(fileData[fileCnt], fileHeader[fileCnt].size, 1, makeFile);
		fclose(makeFile);
	}

	fclose(packedFile);
}

int main() {

	int select;
	cout << "1. Pack\n2. Unpack\n";
	cin >> select;

	switch (select) {
	case 1:
		packing();
		break;
	case 2:
		unpacking();
		break;
	}

	return 0;
}