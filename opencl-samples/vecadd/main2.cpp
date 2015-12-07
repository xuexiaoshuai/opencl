//创建命令队列 
#include <iostream>
#include <CL/cl.h>

#pragma warning( disable : 4996 )

using namespace std;

int main2()
{
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

	command_queue = clCreateCommandQueue(context, devices[0], 0, &errNum);
	if (errNum != CL_SUCCESS || command_queue == NULL){
		cerr << "Failed to create command queue." << endl;
		return -1;
	}

	cout << "create command queue successfully." << endl;

	return 0;
}
