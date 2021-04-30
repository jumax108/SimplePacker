#include <iostream>
#include <string.h>
using namespace std;

struct SFileHeader {
	char* fileName;
	unsigned int size;
	unsigned char fileNameLength;
};

void packing() {
	FILE* fpFileList = nullptr;
	char fileList[100][255] = { 0, };
	char* buffer;

	fopen_s(&fpFileList, "filelist.txt", "rb");
	if (fpFileList == nullptr) {
		cout << "filelist.txt를 찾을 수 없습니다.";
		return ;
	}
	fseek(fpFileList, 0, SEEK_END);
	int size = ftell(fpFileList);
	rewind(fpFileList);

	size += 1;
	buffer = (char*)malloc(size);
	fread(buffer, 1, size - 1, fpFileList);
	fclose(fpFileList);
	buffer[size - 1] = '\0';

	int fileNum = 0;
	char* fileNameBegin = &buffer[0];
	for (int bufCnt = 0; bufCnt < size; ++bufCnt) {
		switch (buffer[bufCnt]) {
		case '\0':
		case 0x0D:

			if (&buffer[bufCnt] - fileNameBegin == 0) {
				break;
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
	SFileHeader* fileHeader = (SFileHeader*)malloc(fileNum);
	if (fileHeader == nullptr) {
		return;
	}

	for (int fileCnt = 0; fileCnt < fileNum; ++fileCnt) {
		FILE* fpFile = nullptr;
		int size = 0;
		fopen_s(&fpFile, fileList[fileCnt], "rb");
		if (fpFile == nullptr) {
			cout << fileList[fileCnt] << ": 파일을 읽을 수 없습니다.\n";
			return;
		}
		fseek(fpFile, 0, SEEK_END);
		size = ftell(fpFile);
		rewind(fpFile);

		fileData[fileCnt] = (char*)malloc(size);
		if (fileData == nullptr) {
			return;
		}
		fread(fileData[fileCnt], 1, size, fpFile);
		fclose(fpFile);

		fileHeader[fileCnt].fileNameLength = strlen(fileList[fileCnt]);
		fileHeader[fileCnt].fileName = (char*)malloc(fileHeader[fileCnt].fileNameLength);
		if (fileHeader[fileCnt].fileName == nullptr) {
			return;
		}
		memcpy(fileHeader[fileCnt].fileName, fileList[fileCnt], fileHeader[fileCnt].fileNameLength);
		cout << fileHeader[fileCnt].fileName << "\n";
		fileHeader[fileCnt].size = size;
	}

	FILE* fileOutput = nullptr;
	fopen_s(&fileOutput, "output", "wb");
	if (fileOutput == nullptr) {
		return;
	}

	fwrite(&fileNum, sizeof(fileNum), 1, fileOutput);

	for (int fileCnt = 0; fileCnt < fileNum; ++fileCnt) {
		fwrite(&(fileHeader[fileCnt].fileNameLength), sizeof(fileHeader[fileCnt].fileNameLength), 1, fileOutput);
		fwrite(fileHeader[fileCnt].fileName, fileHeader[fileCnt].fileNameLength, 1, fileOutput);
		fwrite(&(fileHeader[fileCnt].size), sizeof(fileHeader[fileCnt].size), 1, fileOutput);
	}
	
	for (int fileCnt = 0; fileCnt < fileNum; ++fileCnt) {
		fwrite(fileData[fileCnt], fileHeader[fileCnt].size, 1, fileOutput);
	}

	fclose(fileOutput);

	free(fileData);
	free(fileHeader);
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
		return;
	}

	for (int fileCnt = 0; fileCnt < fileNum; ++fileCnt) {
		fread(&(fileHeader[fileCnt].fileNameLength), sizeof(fileHeader[fileCnt].fileNameLength), 1, packedFile);
		fileHeader[fileCnt].fileName = (char*)malloc(fileHeader[fileCnt].fileNameLength);
		fread(fileHeader[fileCnt].fileName, fileHeader[fileCnt].fileNameLength, 1, packedFile);
		fread(&(fileHeader[fileCnt].size), sizeof(fileHeader[fileCnt].size), 1, packedFile);
	}

	fileData = (char**)malloc(fileNum);
	if (fileData == nullptr) {
		cout << "malloc fail";
		return;
	}

	for (int fileCnt = 0; fileCnt < fileNum; ++fileCnt) {
		fileData[fileCnt] = (char*)malloc(fileHeader[fileCnt].size);
		fread(fileData[fileCnt], fileHeader[fileCnt].size, 1, packedFile);
		FILE* makeFile = nullptr;
		char* fileName = (char*)malloc(fileHeader[fileCnt].fileNameLength + 1);
		memcpy(fileName, fileHeader[fileCnt].fileName, fileHeader[fileCnt].fileNameLength);
		fileName[fileHeader[fileCnt].fileNameLength] = '\0';
		fopen_s(&makeFile, fileName, "wb");
		if (makeFile == nullptr) {
			cout << fileHeader[fileCnt].fileName << ": 파일을 열 수 없습니다.";
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