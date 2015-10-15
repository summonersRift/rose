
#include "KLT/RTL/opencl-utils.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <assert.h>

enum klt_opencl_property_type_e {
  e_klt_opencl_property_is_cl_bool,
  e_klt_opencl_property_is_cl_uint,
  e_klt_opencl_property_is_cl_ulong,
  e_klt_opencl_property_is_size_t,
  e_klt_opencl_property_is_array_size_t,
  e_klt_opencl_property_is_array_char,
  e_klt_opencl_property_is_array_char_split,
  e_klt_opencl_property_is_cl_device_type,
  e_klt_opencl_property_is_cl_device_local_mem_type,
  e_klt_opencl_property_is_cl_device_mem_cache_type,
  e_klt_opencl_property_is_cl_device_exec_capabilities,
  e_klt_opencl_property_is_cl_command_queue_properties,
  e_klt_opencl_property_is_cl_device_fp_config
};

struct klt_opencl_property_t {
   char * name;
   cl_device_info property;
   enum klt_opencl_property_type_e type;
};

#define KLT_OPENCL_NUM_PROPERTIES 51

struct klt_opencl_property_t klt_opencl_property[KLT_OPENCL_NUM_PROPERTIES] = {
  { "CL_DEVICE_ADDRESS_BITS" , CL_DEVICE_ADDRESS_BITS , e_klt_opencl_property_is_cl_uint },
  { "CL_DEVICE_AVAILABLE" , CL_DEVICE_AVAILABLE , e_klt_opencl_property_is_cl_bool },
  { "CL_DEVICE_COMPILER_AVAILABLE" , CL_DEVICE_COMPILER_AVAILABLE , e_klt_opencl_property_is_cl_bool },
  { "CL_DEVICE_ENDIAN_LITTLE" , CL_DEVICE_ENDIAN_LITTLE , e_klt_opencl_property_is_cl_bool },
  { "CL_DEVICE_ERROR_CORRECTION_SUPPORT" , CL_DEVICE_ERROR_CORRECTION_SUPPORT , e_klt_opencl_property_is_cl_bool },
  { "CL_DEVICE_GLOBAL_MEM_CACHE_SIZE" , CL_DEVICE_GLOBAL_MEM_CACHE_SIZE , e_klt_opencl_property_is_cl_ulong },
  { "CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE" , CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE , e_klt_opencl_property_is_cl_uint },
  { "CL_DEVICE_GLOBAL_MEM_SIZE" , CL_DEVICE_GLOBAL_MEM_SIZE , e_klt_opencl_property_is_cl_ulong },
  { "CL_DEVICE_IMAGE_SUPPORT" , CL_DEVICE_IMAGE_SUPPORT , e_klt_opencl_property_is_cl_bool },
  { "CL_DEVICE_IMAGE2D_MAX_HEIGHT" , CL_DEVICE_IMAGE2D_MAX_HEIGHT , e_klt_opencl_property_is_size_t },
  { "CL_DEVICE_IMAGE2D_MAX_WIDTH" , CL_DEVICE_IMAGE2D_MAX_WIDTH , e_klt_opencl_property_is_size_t },
  { "CL_DEVICE_IMAGE3D_MAX_DEPTH" , CL_DEVICE_IMAGE3D_MAX_DEPTH , e_klt_opencl_property_is_size_t },
  { "CL_DEVICE_IMAGE3D_MAX_HEIGHT" , CL_DEVICE_IMAGE3D_MAX_HEIGHT , e_klt_opencl_property_is_size_t },
  { "CL_DEVICE_IMAGE3D_MAX_WIDTH" , CL_DEVICE_IMAGE3D_MAX_WIDTH , e_klt_opencl_property_is_size_t },
  { "CL_DEVICE_LOCAL_MEM_SIZE" , CL_DEVICE_LOCAL_MEM_SIZE , e_klt_opencl_property_is_cl_ulong },
  { "CL_DEVICE_MAX_CLOCK_FREQUENCY" , CL_DEVICE_MAX_CLOCK_FREQUENCY , e_klt_opencl_property_is_cl_uint },
  { "CL_DEVICE_MAX_COMPUTE_UNITS" , CL_DEVICE_MAX_COMPUTE_UNITS , e_klt_opencl_property_is_cl_uint },
  { "CL_DEVICE_MAX_CONSTANT_ARGS" , CL_DEVICE_MAX_CONSTANT_ARGS , e_klt_opencl_property_is_cl_uint },
  { "CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE" , CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE , e_klt_opencl_property_is_cl_ulong },
  { "CL_DEVICE_MAX_MEM_ALLOC_SIZE" , CL_DEVICE_MAX_MEM_ALLOC_SIZE , e_klt_opencl_property_is_cl_ulong },
  { "CL_DEVICE_MAX_PARAMETER_SIZE" , CL_DEVICE_MAX_PARAMETER_SIZE , e_klt_opencl_property_is_size_t },
  { "CL_DEVICE_MAX_READ_IMAGE_ARGS" , CL_DEVICE_MAX_READ_IMAGE_ARGS , e_klt_opencl_property_is_cl_uint },
  { "CL_DEVICE_MAX_SAMPLERS" , CL_DEVICE_MAX_SAMPLERS , e_klt_opencl_property_is_cl_uint },
  { "CL_DEVICE_MAX_WORK_GROUP_SIZE" , CL_DEVICE_MAX_WORK_GROUP_SIZE , e_klt_opencl_property_is_size_t },
  { "CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS" , CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS , e_klt_opencl_property_is_cl_uint },
  { "CL_DEVICE_MAX_WORK_ITEM_SIZES" , CL_DEVICE_MAX_WORK_ITEM_SIZES , e_klt_opencl_property_is_array_size_t },
  { "CL_DEVICE_MAX_WRITE_IMAGE_ARGS" , CL_DEVICE_MAX_WRITE_IMAGE_ARGS , e_klt_opencl_property_is_cl_uint },
  { "CL_DEVICE_MEM_BASE_ADDR_ALIGN" , CL_DEVICE_MEM_BASE_ADDR_ALIGN , e_klt_opencl_property_is_cl_uint },
  { "CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE" , CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE , e_klt_opencl_property_is_cl_uint },
  { "CL_DEVICE_NAME" , CL_DEVICE_NAME , e_klt_opencl_property_is_array_char },
  { "CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR" , CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR , e_klt_opencl_property_is_cl_uint },
  { "CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT" , CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT , e_klt_opencl_property_is_cl_uint },
  { "CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT" , CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT , e_klt_opencl_property_is_cl_uint },
  { "CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG" , CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG , e_klt_opencl_property_is_cl_uint },
  { "CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT" , CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT , e_klt_opencl_property_is_cl_uint },
  { "CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE" , CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE , e_klt_opencl_property_is_cl_uint },
  { "CL_DEVICE_PROFILING_TIMER_RESOLUTION" , CL_DEVICE_PROFILING_TIMER_RESOLUTION , e_klt_opencl_property_is_size_t },
  { "CL_DEVICE_VENDOR" , CL_DEVICE_VENDOR , e_klt_opencl_property_is_array_char },
  { "CL_DEVICE_VENDOR_ID" , CL_DEVICE_VENDOR_ID , e_klt_opencl_property_is_cl_uint },
  { "CL_DEVICE_VERSION" , CL_DEVICE_VERSION , e_klt_opencl_property_is_array_char },
  { "CL_DRIVER_VERSION" , CL_DRIVER_VERSION , e_klt_opencl_property_is_array_char },
  { "CL_DEVICE_LOCAL_MEM_TYPE" , CL_DEVICE_LOCAL_MEM_TYPE , e_klt_opencl_property_is_cl_device_local_mem_type },
  { "CL_DEVICE_TYPE" , CL_DEVICE_TYPE , e_klt_opencl_property_is_cl_device_type },
  { "CL_DEVICE_EXTENSIONS" , CL_DEVICE_EXTENSIONS , e_klt_opencl_property_is_array_char_split },
  { "CL_DEVICE_PROFILE" , CL_DEVICE_PROFILE , e_klt_opencl_property_is_array_char },
  { "CL_DEVICE_DOUBLE_FP_CONFIG" , CL_DEVICE_DOUBLE_FP_CONFIG , e_klt_opencl_property_is_cl_device_fp_config },
  { "CL_DEVICE_EXECUTION_CAPABILITIES" , CL_DEVICE_EXECUTION_CAPABILITIES , e_klt_opencl_property_is_cl_device_exec_capabilities },
  { "CL_DEVICE_GLOBAL_MEM_CACHE_TYPE" , CL_DEVICE_GLOBAL_MEM_CACHE_TYPE , e_klt_opencl_property_is_cl_device_mem_cache_type },
  { "CL_DEVICE_QUEUE_PROPERTIES" , CL_DEVICE_QUEUE_PROPERTIES , e_klt_opencl_property_is_cl_command_queue_properties },
  { "CL_DEVICE_SINGLE_FP_CONFIG" , CL_DEVICE_SINGLE_FP_CONFIG , e_klt_opencl_property_is_cl_device_fp_config },
  { "CL_DEVICE_HALF_FP_CONFIG" , 0 & CL_DEVICE_HALF_FP_CONFIG , e_klt_opencl_property_is_cl_device_fp_config }
};

void klt_opencl_property_to_JSON(struct klt_opencl_property_t * property, cl_device_id device, const char * indent) {
  if (property->property == 0) {
    printf("\"\"");
    return;
  }

  size_t i;
  cl_int status;
  size_t size;

  status = clGetDeviceInfo(device, property->property, 0, NULL, &size);
  klt_check_opencl_status("[Error] clGetDeviceInfo (size) returns:", status);

  void * value = malloc(size);
  assert(value != NULL);

  status = clGetDeviceInfo(device, property->property, size, value, NULL);
  klt_check_opencl_status("[Error] clGetDeviceInfo (value) returns:", status);

  switch (property->type) {
    case e_klt_opencl_property_is_cl_bool:    printf("%u"    , *(cl_bool * )value); break;
    case e_klt_opencl_property_is_cl_uint:    printf("%u"    , *(cl_uint * )value); break;
    case e_klt_opencl_property_is_cl_ulong:   printf("%lu"   , *(cl_ulong *)value); break;
    case e_klt_opencl_property_is_size_t:     printf("%zd"   , *(size_t *  )value); break;
    case e_klt_opencl_property_is_array_char: printf("\"%s\"",  (char *    )value); break;
    case e_klt_opencl_property_is_array_size_t:
    {
      printf("[");
      for (i = 0; i < size / sizeof(size_t); i++) {
        if (i > 0) printf(", ");
        printf("%zd", ((size_t *)value)[i]);
      }
      printf("]");
      break;
    }
    case e_klt_opencl_property_is_array_char_split:
    {
      char * values = (char *)value;
      char * tok = strtok(values, " ");
      printf("[ ");
      if (tok != NULL) {
        printf("\n%s  \"%s\"", indent, tok);
        tok = strtok(NULL, " ");
        while (tok != NULL) {
          printf(",\n%s  \"%s\"", indent, tok);
          tok = strtok(NULL, " ");
        }
      }
      printf("\n%s]", indent);
      break;
    }
    case e_klt_opencl_property_is_cl_device_type:
    {
      cl_device_type device_type = *(cl_device_type *)value;
      printf("\"");
      if ((device_type & CL_DEVICE_TYPE_CPU        ) != 0) printf("CPU ");
      if ((device_type & CL_DEVICE_TYPE_GPU        ) != 0) printf("GPU ");
      if ((device_type & CL_DEVICE_TYPE_ACCELERATOR) != 0) printf("accelerator ");
      if ((device_type & CL_DEVICE_TYPE_DEFAULT    ) != 0) printf("default ");
      printf("\"");
      break;
    }
    case e_klt_opencl_property_is_cl_device_local_mem_type:
    {
      cl_device_local_mem_type device_local_mem_type = *(cl_device_local_mem_type *)value;
      switch (device_local_mem_type) {
        case CL_LOCAL:  printf("\"local\""); break;
        case CL_GLOBAL: printf("\"global\""); break;
        default:
          assert(0);
      }
      break;
    }
    case e_klt_opencl_property_is_cl_device_mem_cache_type:
    { //CL_NONE, CL_READ_ONLY_CACHE, and CL_READ_WRITE_CACHE.
      cl_device_mem_cache_type device_mem_cache_type = *(cl_device_mem_cache_type *)value;
      switch (device_mem_cache_type) {
        case CL_NONE:             printf("\"no\""); break;
        case CL_READ_ONLY_CACHE:  printf("\"ro\""); break;
        case CL_READ_WRITE_CACHE: printf("\"rw\""); break;
        default:
          assert(0);
      }
      break;
    }
    case e_klt_opencl_property_is_cl_device_exec_capabilities:
    {
      cl_device_exec_capabilities device_exec_capabilities = *(cl_device_exec_capabilities *)value;
      printf("\"");
      if ((device_exec_capabilities & CL_EXEC_KERNEL       ) != 0) printf("kernel ");
      if ((device_exec_capabilities & CL_EXEC_NATIVE_KERNEL) != 0) printf("native ");
      printf("\"");
      break;
    }
    case e_klt_opencl_property_is_cl_command_queue_properties:
    {
      cl_command_queue_properties command_queue_properties = *(cl_command_queue_properties *)value;
      printf("\"");
      if ((command_queue_properties & CL_QUEUE_PROFILING_ENABLE             ) != 0) printf("profiling ");
      if ((command_queue_properties & CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE) != 0) printf("out-of-order ");
      printf("\"");
      break;
    }
    case e_klt_opencl_property_is_cl_device_fp_config:
    {
      cl_device_fp_config device_fp_config = *(cl_device_fp_config *)value;
      printf("\"");
      if ((device_fp_config & CL_FP_DENORM          ) != 0) printf("CL_FP_DENORM ");
      if ((device_fp_config & CL_FP_INF_NAN         ) != 0) printf("CL_FP_INF_NAN ");
      if ((device_fp_config & CL_FP_ROUND_TO_NEAREST) != 0) printf("CL_FP_ROUND_TO_NEAREST ");
      if ((device_fp_config & CL_FP_ROUND_TO_ZERO   ) != 0) printf("CL_FP_ROUND_TO_ZERO ");
      if ((device_fp_config & CL_FP_ROUND_TO_INF    ) != 0) printf("CL_FP_ROUND_TO_INF ");
//    if ((device_fp_config & CP_FP_FMA             ) != 0) printf("CP_FP_FMA ");
      printf("\"");
      break;
    }
    default:
      assert(0);
  }

  free(value);
}

void klt_opencl_device_to_JSON(cl_platform_id platform, cl_device_id device, const char * indent) {
  char * new_indent = malloc(strlen(indent) + 3);
  memset(new_indent, 0, strlen(indent) + 3);
  strcat(new_indent, indent);
  strcat(new_indent, "  ");
  printf("{\n%s", new_indent);
  size_t i;
  for (i = 0; i < KLT_OPENCL_NUM_PROPERTIES; i++) {
    if (i > 0) printf(",\n%s", new_indent);
    printf("\"%s\":", klt_opencl_property[i].name);
    klt_opencl_property_to_JSON(&(klt_opencl_property[i]), device, new_indent);
  }
  free(new_indent);
  printf("\n%s}", indent);
}

char * klt_read_file(const char * filename) {

   FILE *fp;
   int err;
   int size;

   char *source;

   fp = fopen(filename, "rb");
   if(fp == NULL) {
      printf("[Error] Could not open kernel file: %s\n", filename);
      return NULL;
   }
   
   err = fseek(fp, 0, SEEK_END);
   if(err != 0) {
      printf("[Error] Seeking to end of file: %s\n", filename);
      return NULL;
   }

   size = ftell(fp);
   if(size < 0) {
      printf("[Error] Getting position in file: %s\n", filename);
      return NULL;
   }

   err = fseek(fp, 0, SEEK_SET);
   if(err != 0) {
      printf("[Error] Seeking to start of file: %s\n", filename);
      return NULL;
   }

   source = (char*)malloc(size+1);
   if(source == NULL) {
      printf("[Error] Allocating %d bytes to read file: %s\n", size+1, filename);
      assert(0);
   }

   err = fread(source, 1, size, fp);
   if(err != size) {
      printf("[Error] only read %d/%d bytes in file: %s\n", err, size, filename);
      return NULL;
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

