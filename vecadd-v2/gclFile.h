#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <malloc.h>

class gclFile
{
public:
	gclFile();
	~gclFile();

	//��opencl kernelԴ�ļ����ı�ģʽ��
	bool open(const char* filename);
	//��д������kernel�ļ�
	bool writeBinaryToFile(const char* filename, const char* binary, size_t numBytes);
	bool readBinaryFromFile(const char* filename);

	//�ÿո����س����з�
	void replaceNewlineWithSpaces(){
		size_t pos = source_.find_first_of('\n', 0);
		while (pos != -1){
			source_.replace(pos, 1, " ");
			pos = source_.find_first_of('\n', pos+1);
		}

		pos = source_.find_first_of('\r', 0);
		while (pos != -1){
			source_.replace(pos, 1, " ");
			pos = source_.find_first_of('\r', pos + 1);
		}
	}

	//����Դ��string
	const std::string& source() const{
		return source_;
	}
private:
	//��ֹcopy����
	gclFile(const gclFile&);
	//��ֹ��ֵ����
	gclFile& operator=(const gclFile&);

	std::string source_;     //Դ�������
};