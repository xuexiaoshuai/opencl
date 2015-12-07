//创建上下文练习

#include <iostream>
#include <CL/cl.h>

using namespace std;


int main1()
{
	cl_int errNum;
	cl_uint numPlatforms;
	cl_platform_id *platformIds;


	errNum = clGetPlatformIDs(0, NULL, &numPlatforms);//获取可用的平台数目，保存在numplatforms中
	if (errNum != CL_SUCCESS || numPlatforms <= 0){
		cerr << "get the available platforms error: " << endl;
		return -1;
	}
	cout << "实际可用的opencl平台数目为： " << numPlatforms << endl;

	//为平台分配空间
	cout << sizeof(cl_platform_id) << endl;
	platformIds = (cl_platform_id*)alloca(sizeof(cl_platform_id)* numPlatforms);
	if (!platformIds){
		cerr << "alloca error" << endl;
		return -1;
	}
	errNum = clGetPlatformIDs(numPlatforms, platformIds, NULL);//把找到的平台全部填充到platformIds
	if (errNum != CL_SUCCESS){
		cerr << "failed to find opencl platform" << endl;
		return -1;
	}

	size_t size = 0;
	cout << size << endl;
	errNum = clGetPlatformInfo(platformIds[0], CL_PLATFORM_NAME, 0, NULL, &size);
	cout << size << endl;
	char* name = (char*)alloca(sizeof(char)* size);
	errNum = clGetPlatformInfo(platformIds[0], CL_PLATFORM_NAME, size, name, 0);
	cout << "platform name: " << name << endl;

	char platform_name[640];
	size_t r_size;
	errNum = clGetPlatformInfo(platformIds[0], CL_PLATFORM_PROFILE, 640, platform_name, &r_size);
	cout << platform_name << endl;
	cout << r_size << endl;

    //获取设备
	cl_device_id *devices;
	cl_uint num_devices;
	errNum = clGetDeviceIDs(platformIds[0], CL_DEVICE_TYPE_GPU, 0, NULL, &num_devices);
	if (errNum != CL_SUCCESS || num_devices <= 0){
		cerr << "NO GPU device found for platform" << endl;
		return -1;
	}
	cout << "num_devices: " << num_devices << endl;
	devices = (cl_device_id*)alloca(sizeof(cl_device_id)* num_devices);
	cout << "sizeof(cl_device_id): " << sizeof(cl_device_id) << endl;
	errNum = clGetDeviceIDs(platformIds[0], CL_DEVICE_TYPE_GPU, num_devices, devices, NULL);
	if (errNum != CL_SUCCESS){
		cerr << "get device error" << endl;
		return -1;
	}

	//创建上下文
	
	cl_context context = NULL;
	
	/*cl_context_properties properties[] = {CL_CONTEXT_PLATFORM, (cl_context_properties)platformIds[0], 0};
	context = clCreateContext(properties, num_devices,devices,NULL, NULL, &errNum);
	if (context == NULL || errNum != CL_SUCCESS){
		cerr << "failed to create context" << endl;
		return -1;
	}
	else{
		cout << "context OK" << endl;
	}*/

	cl_context_properties properties[] = { CL_CONTEXT_PLATFORM, (cl_context_properties)platformIds[0], 0 };
	context = clCreateContextFromType(properties, CL_DEVICE_TYPE_GPU, NULL, NULL, &errNum);
	if (context == NULL || errNum != CL_SUCCESS){
		cerr << "failed to create context" << endl;
		return -1;
	}
	else{
		cout << "context OK" << endl;
	}
	errNum = clGetContextInfo(context, CL_CONTEXT_DEVICES, 0, NULL, &size);
	cout << "size: " << size << endl;
	errNum = clGetContextInfo(context, CL_CONTEXT_DEVICES, size, devices, NULL);

	cout << errNum << endl;
	for (size_t i = 0; i < size / sizeof(cl_device_id); i++){
		cl_device_type type;
		errNum = clGetDeviceInfo(devices[i], CL_DEVICE_TYPE, sizeof(cl_device_type), &type, 0);
		switch (type){
		case CL_DEVICE_TYPE_GPU:
			cout << "gpu" << endl;
			break;
		case CL_DEVICE_TYPE_CPU:
			cout << "cpu" << endl;
			break;
		case CL_DEVICE_TYPE_ACCELERATOR:
			cout << "ACCELERATOR" << endl;
			break;
		default:
			cout << "default" << endl;
			break;
		}
	}





	return 0;
}
