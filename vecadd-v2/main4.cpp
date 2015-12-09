//������vecadd���룬v2--����������ں��ļ��ͼ�ʱ��
#include <iostream>
#include <fstream>
#include <CL/cl.h>
#include <time.h>
#include "gclFile.h"
#include "gclTimer.h"


#pragma warning( disable : 4996 )
using namespace std;

#define BUFFER_SIZE 10000000



int main(int argc, char* argv[])
{
	float *buf1 = 0;
	float *buf2 = 0;
	float *buf = 0;

	buf1 = (float*)malloc(BUFFER_SIZE * sizeof(float));
	buf2 = (float*)malloc(BUFFER_SIZE * sizeof(float));
	buf = (float*)malloc(BUFFER_SIZE * sizeof(float));

	//��һЩ�������ʼ��buf1 buf2 ������
	srand((unsigned)time(NULL));
	for (int i = 0; i < BUFFER_SIZE; i++){
		buf1[i] = rand() % 65535;
	}

	srand((unsigned)time(NULL) + 1000);
	for (int i = 0; i < BUFFER_SIZE; i++){
		buf2[i] = rand() % 65535;
	}


	gclTimer myTimer;
	myTimer.Start();
	//cpu����buf1��buf2�ĺ�
	for (int i = 0; i < BUFFER_SIZE; i++){
		buf[i] = buf1[i] + buf2[i];
	}
	myTimer.Stop();
	double cpuTime = myTimer.GetElapsedTime();


	//gpu����
	cl_int errNum;
	cl_uint numPlatforms;
	cl_uint numDevices;
	cl_platform_id* platforms;
	cl_device_id* devices;
	cl_context context = NULL;
	cl_command_queue command_queue;

	errNum = clGetPlatformIDs(0, NULL, &numPlatforms);
	if (errNum != CL_SUCCESS || numPlatforms <= 0){
		cerr << "Failed to found platforms." << endl;
		return -1;
	}
	platforms = (cl_platform_id*)alloca(sizeof(cl_platform_id)* numPlatforms);
	errNum = clGetPlatformIDs(numPlatforms, platforms, NULL);
	if (errNum != CL_SUCCESS){
		cerr << "Failed to found platforms." << endl;
		return -1;
	}

	errNum = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_GPU, 0, NULL, &numDevices);
	if (errNum != CL_SUCCESS || numPlatforms <= 0){
		cerr << "Failed to found devices." << endl;
		return -1;
	}
	devices = (cl_device_id*)alloca(sizeof(cl_device_id)* numDevices);
	errNum = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_GPU, numDevices, devices, NULL);
	if (errNum != CL_SUCCESS){
		cerr << "Failed to found devices." << endl;
		return -1;
	}

	cl_context_properties properties[] = { CL_CONTEXT_PLATFORM, (cl_context_properties)platforms[0], 0 };

	context = clCreateContext(properties, numDevices, devices, NULL, NULL, &errNum);
	if (errNum != CL_SUCCESS || context == NULL){
		cerr << "Failed to create context." << endl;
		return -1;
	}

	command_queue = clCreateCommandQueue(context, devices[0], CL_QUEUE_PROFILING_ENABLE, &errNum);
	if (errNum != CL_SUCCESS || command_queue == NULL){
		cerr << "Failed to create command queue." << endl;
		return -1;
	}



	//����3���ڴ���󣬲���buf1������ͨ����ʽ�����ķ�ʽ������clbuf1��buf2������ͨ����ʾ������buf2
	cl_mem clbuf1 = clCreateBuffer(context,
		CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
		BUFFER_SIZE*sizeof(cl_float),
		buf1,
		NULL
		);
	cl_mem clbuf2 = clCreateBuffer(context,
		CL_MEM_READ_ONLY,
		BUFFER_SIZE*sizeof(cl_float),
		NULL,
		NULL
		);
	cl_event writeEvt;
	errNum = clEnqueueWriteBuffer(command_queue, clbuf2, 1, 0, BUFFER_SIZE*sizeof(cl_float), buf2, 0, 0, &writeEvt);
	//errNum = clFlush(command_queue);
	////�ȴ����ݴ�������ټ�������ִ��
	//waitForEventAndRelease(&writeEvt);

	errNum = clWaitForEvents(1, &writeEvt);
	if (errNum != CL_SUCCESS){
		cout << "wait fail" << endl;
		return -1;
	}

	cl_mem buffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, BUFFER_SIZE*sizeof(cl_float), NULL, NULL);

	//const char* filename = "add.cl";
	//std::string sourceStr;
	//errNum = convertToString(filename, sourceStr);
	gclFile kernelFile;
	if (!kernelFile.open("add.cl")){
		cout << "fail to open add.cl" << endl;
		exit(0);
	}

	const char* source = kernelFile.source().c_str();
	size_t sourceSize[] = { strlen(source) };

	//�����������
	cl_program  program = clCreateProgramWithSource(context, 1, &source, sourceSize, NULL);

	//����������
	errNum = clBuildProgram(program, 1, &devices[0], NULL, NULL, NULL);
	if (errNum != 0){
		cout << "build error: " << errNum << endl;
		char tmpbuf[0x10000];
		clGetProgramBuildInfo(program, devices[0], CL_PROGRAM_BUILD_LOG, 0x10000, tmpbuf, 0);
		cout << tmpbuf << endl;
		return -1;
	}
	//�洢����õ�kernel�ļ�
	char** binaries = (char**)malloc(sizeof(char*)* 1);//ֻ��һ���豸
	size_t *binarySizes = (size_t*)malloc(sizeof(size_t)* 1);
	errNum = clGetProgramInfo(program,
		CL_PROGRAM_BINARY_SIZES,
		sizeof(size_t)*1,
		binarySizes,
		NULL
		);
	binaries[0] = (char *)malloc(sizeof(char)* binarySizes[0]);
	errNum = clGetProgramInfo(program,
		CL_PROGRAM_BINARIES,
		sizeof(char *)* 1,
		binaries,
		NULL);
	kernelFile.writeBinaryToFile("vecadd.bin", binaries[0], binarySizes[0]);

	//�����ں˶���
	cl_kernel kernel = clCreateKernel(program, "vecadd", NULL);

	//�����ں˲���
	cl_int clnum = BUFFER_SIZE;
	clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*)&clbuf1);
	clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*)&clbuf2);
	clSetKernelArg(kernel, 2, sizeof(cl_mem), (void*)&buffer);

	myTimer.Reset();
	myTimer.Start();
	//ִ��kernel��Range��1ά�� work items sizeΪBUFFER_SIZE
	cl_event ev;
	size_t global_work_size = BUFFER_SIZE;
	clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_work_size, NULL, 0, NULL, &ev);
	errNum = clWaitForEvents(1, &ev);
	if (errNum != CL_SUCCESS){
		cout << "wait fail" << endl;
		return -1;
	}
	myTimer.Stop();
	double kernelTime = myTimer.GetElapsedTime();
	myTimer.Start();
	//���ݿ�����host�ڴ�
	cl_float* ptr;
	cl_event mapevt;
	ptr = (cl_float*)clEnqueueMapBuffer(command_queue, buffer, CL_TRUE, CL_MAP_READ, 0, BUFFER_SIZE*sizeof(cl_float), 0, NULL, &mapevt, &errNum);
	clWaitForEvents(1, &mapevt);
	myTimer.Stop();
	double gpuTime = myTimer.GetElapsedTime();

	cout << "cpu time: " << cpuTime << "s"<<endl;
	cout << "kernel time: " << kernelTime << "s" << endl;
	cout << "gpu time: " << gpuTime << "s"<<endl;

	//�����֤
	if (!memcmp(buf, ptr, BUFFER_SIZE))
		cout << "equal" << endl;
	else
		cout << "not equal" << endl;

	for (int i = 0; i < 10; i++){
		cout << " " << buf[i];
	}
	cout << endl;
	for (int i = 0; i < 10; i++){
		cout << " " << ptr[i];
	}
	cout << endl;
	if (buf)
		free(buf);
	if (buf1)
		free(buf1);
	if (buf2)
		free(buf2);

	//ɾ��opencl������Դ
	clReleaseMemObject(clbuf1);
	clReleaseMemObject(clbuf2);
	clReleaseMemObject(buffer);
	clReleaseProgram(program);
	clReleaseCommandQueue(command_queue);
	clReleaseContext(context);


	return 0;
}