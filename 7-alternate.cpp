#include <stdio.h>
#include <stdlib.h>
#include <CL/cl.h>

#define VECTOR_SIZE 1024

const char* saxpy_kernel = ""
"__kernel void saxpy_kernel(float alpha, __global float *A, __global float *B, __global float *C ){"
"    int id = get_global_id(0);"
"    C[id] = alpha*A[id] + B[id];}"
";";


int main(int argc, char **argv){

    
    float alpha = 2.0;
    float *A = (float*)malloc(sizeof(float)*VECTOR_SIZE);
    float *B = (float*)malloc(sizeof(float)*VECTOR_SIZE);
    float *C = (float*)malloc(sizeof(float)*VECTOR_SIZE);

    for(int i=0; i<VECTOR_SIZE; i++){
        A[i] = i;
        B[i] = VECTOR_SIZE - i;
        C[i] = 0;
    }

    cl_uint numplatforms;
    cl_platform_id* platforms = NULL;
    cl_int clStatus = clGetPlatformIDs(0, 0, &numplatforms);
    platforms = (cl_platform_id*)malloc(sizeof(cl_platform_id)*numplatforms);


    clStatus = clGetPlatformIDs(numplatforms, platforms, 0);


    cl_device_id* device_list = NULL;
    cl_uint num_devices;
    clStatus = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_GPU, 0, 0, &num_devices);
    device_list = (cl_device_id*)malloc(sizeof(cl_device_id)*num_devices);
    clStatus = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_GPU, num_devices, device_list, 0);

    //0 n d 0 0 &stat
    cl_context context;
    context = clCreateContext(0, num_devices, device_list, 0, 0, &clStatus);

    //c d[0] 0 &stat
    cl_command_queue command_queue;
    command_queue = clCreateCommandQueueWithProperties(context, device_list[0], 0, &clStatus);

    //c cl_mem_read_only s 0 &stat
    cl_mem A_clmem = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(float)*VECTOR_SIZE, 0, &clStatus);
    cl_mem B_clmem = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(float)*VECTOR_SIZE, 0, &clStatus);
    cl_mem C_clmem = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(float)*VECTOR_SIZE, 0, &clStatus);

    //cq A_mem cl_true 0 s A 0 0 0
    clStatus = clEnqueueWriteBuffer(command_queue, A_clmem, CL_TRUE, 0, sizeof(float)*VECTOR_SIZE, A, 0, 0, 0);
    clStatus = clEnqueueWriteBuffer(command_queue, B_clmem, CL_TRUE, 0, sizeof(float)*VECTOR_SIZE, B, 0, 0, 0);

    //c 1 (const char **)& 0 &stat
    cl_program program;
    program = clCreateProgramWithSource(context, 1, (const char **)&saxpy_kernel, 0, &clStatus);

    //p 1 d 0 0 0
    clStatus = clBuildProgram(program, 1, device_list, 0, 0, 0);

    //p "saxpy_kernel" &stat
    cl_kernel kernel = clCreateKernel(program, "saxpy_kernel", &clStatus);

    //k 0 s (void*)&obj
    clStatus = clSetKernelArg(kernel, 0, sizeof(float), (void*)&alpha);
    clStatus = clSetKernelArg(kernel, 1, sizeof(cl_mem),(void*)&A_clmem);
    clStatus = clSetKernelArg(kernel, 2, sizeof(cl_mem),(void*)&B_clmem);
    clStatus = clSetKernelArg(kernel, 3, sizeof(cl_mem),(void*)&C_clmem);


    size_t global_work_size = VECTOR_SIZE;
    size_t local_work_size = 64;

    //cq k 1 0 &global &local 0 0 0
    clStatus = clEnqueueNDRangeKernel(command_queue, kernel, 1, 0, &global_work_size, &local_work_size, 0, 0, 0);

    //cq C_mem cl_true 0 s C 0 0 0
    clStatus = clEnqueueReadBuffer(command_queue, C_clmem, CL_TRUE, 0, sizeof(float)*VECTOR_SIZE, C, 0, 0, 0);

    clStatus = clFlush(command_queue);
    clStatus = clFinish(command_queue);

    // Display the result to the screen
    for (int i = 0; i < VECTOR_SIZE; i++)
        printf("%f * %f + %f = %f\n", alpha, A[i], B[i], C[i]);

    clStatus = clReleaseKernel(kernel);
    clStatus = clReleaseProgram(program);
    clStatus = clReleaseMemObject(A_clmem);
    clStatus = clReleaseMemObject(B_clmem);
    clStatus = clReleaseMemObject(C_clmem);
    clStatus = clReleaseCommandQueue(command_queue);
    clStatus = clReleaseContext(context);
    free(A);
    free(B);
    free(C);
    free(platforms);
    free(device_list);
}
