
#include "KLT/RTL/opencl-utils.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <assert.h>

char * klt_read_file(const char * filename) {

   FILE *fp;
   int err;
   int size;

   char *source;

   fp = fopen(filename, "rb");
   if(fp == NULL) {
      printf("Could not open kernel file: %s\n", filename);
      assert(0);
   }
   
   err = fseek(fp, 0, SEEK_END);
   if(err != 0) {
      printf("Error seeking to end of file\n");
      assert(0);
   }

   size = ftell(fp);
   if(size < 0) {
      printf("Error getting file position\n");
      assert(0);
   }

   err = fseek(fp, 0, SEEK_SET);
   if(err != 0) {
      printf("Error seeking to start of file\n");
      assert(0);
   }

   source = (char*)malloc(size+1);
   if(source == NULL) {
      printf("Error allocating %d bytes for the program source\n", size+1);
      assert(0);
   }

   err = fread(source, 1, size, fp);
   if(err != size) {
      printf("only read %d bytes\n", err);
      assert(0);
   }

   source[size] = '\0';

   return source;
}

void klt_get_ocl_build_log(cl_device_id device, cl_program program) {
  char * build_log;
  size_t build_log_size;
  clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &build_log_size);
  if (build_log_size == 0)
    printf("[warning] OpenCL return an empty log...\n");
  else {
    build_log = (char*)malloc(build_log_size);
    if (build_log == NULL) {
      perror("[fatal] malloc : build_log");
      exit(-1);
    }
    clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, build_log_size, build_log, NULL);
    build_log[build_log_size-1] = '\0';
    printf("*********************************\n%s\n*********************************\n", build_log);
    free(build_log);
  }
}

void klt_check_opencl_status(char * msg, cl_int status) {
  if (status != CL_SUCCESS) {
    printf("%s ", msg);
    klt_print_opencl_status(status);
    printf("\n");
  }
  assert(status == CL_SUCCESS);
}

void klt_print_opencl_status(cl_int status) {
  switch (status) {
      case CL_DEVICE_NOT_FOUND:                          printf("CL_DEVICE_NOT_FOUND");                          break;
      case CL_DEVICE_NOT_AVAILABLE:                      printf("CL_DEVICE_NOT_AVAILABLE");                      break;
      case CL_COMPILER_NOT_AVAILABLE:                    printf("CL_COMPILER_NOT_AVAILABLE");                    break;
      case CL_MEM_OBJECT_ALLOCATION_FAILURE:             printf("CL_MEM_OBJECT_ALLOCATION_FAILURE");             break;
      case CL_OUT_OF_RESOURCES:                          printf("CL_OUT_OF_RESOURCES");                          break;
      case CL_OUT_OF_HOST_MEMORY:                        printf("CL_OUT_OF_HOST_MEMORY");                        break;
      case CL_PROFILING_INFO_NOT_AVAILABLE:              printf("CL_PROFILING_INFO_NOT_AVAILABLE");              break;
      case CL_MEM_COPY_OVERLAP:                          printf("CL_MEM_COPY_OVERLAP");                          break;
      case CL_IMAGE_FORMAT_MISMATCH:                     printf("CL_IMAGE_FORMAT_MISMATCH");                     break;
      case CL_IMAGE_FORMAT_NOT_SUPPORTED:                printf("CL_IMAGE_FORMAT_NOT_SUPPORTED");                break;
      case CL_BUILD_PROGRAM_FAILURE:                     printf("CL_BUILD_PROGRAM_FAILURE");                     break;
      case CL_MAP_FAILURE:                               printf("CL_MAP_FAILURE");                               break;
      case CL_INVALID_VALUE:                             printf("CL_INVALID_VALUE");                             break;
      case CL_INVALID_DEVICE_TYPE:                       printf("CL_INVALID_DEVICE_TYPE");                       break;
      case CL_INVALID_PLATFORM:                          printf("CL_INVALID_PLATFORM");                          break;
      case CL_INVALID_DEVICE:                            printf("CL_INVALID_DEVICE");                            break;
      case CL_INVALID_CONTEXT:                           printf("CL_INVALID_CONTEXT");                           break;
      case CL_INVALID_QUEUE_PROPERTIES:                  printf("CL_INVALID_QUEUE_PROPERTIES");                  break;
      case CL_INVALID_COMMAND_QUEUE:                     printf("CL_INVALID_COMMAND_QUEUE");                     break;
      case CL_INVALID_HOST_PTR:                          printf("CL_INVALID_HOST_PTR");                          break;
      case CL_INVALID_MEM_OBJECT:                        printf("CL_INVALID_MEM_OBJECT");                        break;
      case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:           printf("CL_INVALID_IMAGE_FORMAT_DESCRIPTOR");           break;
      case CL_INVALID_IMAGE_SIZE:                        printf("CL_INVALID_IMAGE_SIZE");                        break;
      case CL_INVALID_SAMPLER:                           printf("CL_INVALID_SAMPLER");                           break;
      case CL_INVALID_BINARY:                            printf("CL_INVALID_BINARY");                            break;
      case CL_INVALID_BUILD_OPTIONS:                     printf("CL_INVALID_BUILD_OPTIONS");                     break;
      case CL_INVALID_PROGRAM:                           printf("CL_INVALID_PROGRAM");                           break;
      case CL_INVALID_PROGRAM_EXECUTABLE:                printf("CL_INVALID_PROGRAM_EXECUTABLE");                break;
      case CL_INVALID_KERNEL_NAME:                       printf("CL_INVALID_KERNEL_NAME");                       break;
      case CL_INVALID_KERNEL_DEFINITION:                 printf("CL_INVALID_KERNEL_DEFINITION");                 break;
      case CL_INVALID_KERNEL:                            printf("CL_INVALID_KERNEL");                            break;
      case CL_INVALID_ARG_INDEX:                         printf("CL_INVALID_ARG_INDEX");                         break;
      case CL_INVALID_ARG_VALUE:                         printf("CL_INVALID_ARG_VALUE");                         break;
      case CL_INVALID_ARG_SIZE:                          printf("CL_INVALID_ARG_SIZE");                          break;
      case CL_INVALID_KERNEL_ARGS:                       printf("CL_INVALID_KERNEL_ARGS");                       break;
      case CL_INVALID_WORK_DIMENSION:                    printf("CL_INVALID_WORK_DIMENSION");                    break;
      case CL_INVALID_WORK_GROUP_SIZE:                   printf("CL_INVALID_WORK_GROUP_SIZE");                   break;
      case CL_INVALID_WORK_ITEM_SIZE:                    printf("CL_INVALID_WORK_ITEM_SIZE");                    break;
      case CL_INVALID_GLOBAL_OFFSET:                     printf("CL_INVALID_GLOBAL_OFFSET");                     break;
      case CL_INVALID_EVENT_WAIT_LIST:                   printf("CL_INVALID_EVENT_WAIT_LIST");                   break;
      case CL_INVALID_EVENT:                             printf("CL_INVALID_EVENT");                             break;
      case CL_INVALID_OPERATION:                         printf("CL_INVALID_OPERATION");                         break;
      case CL_INVALID_GL_OBJECT:                         printf("CL_INVALID_GL_OBJECT");                         break;
      case CL_INVALID_BUFFER_SIZE:                       printf("CL_INVALID_BUFFER_SIZE");                       break;
      case CL_INVALID_MIP_LEVEL:                         printf("CL_INVALID_MIP_LEVEL");                         break;
      case CL_INVALID_GLOBAL_WORK_SIZE:                  printf("CL_INVALID_GLOBAL_WORK_SIZE");                  break;
#ifdef CL_VERSION_1_1
      case CL_MISALIGNED_SUB_BUFFER_OFFSET:              printf("CL_MISALIGNED_SUB_BUFFER_OFFSET");              break;
      case CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST: printf("CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST"); break;
      case CL_INVALID_PROPERTY:                          printf("CL_INVALID_PROPERTY");
#endif
      default:                                           printf("CL_UNKNOWN_ERROR_CODE");                        break;
    }
}

