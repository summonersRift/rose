
SUBDIRS=

# Headers dir

SUBDIRS+=include/DLX/TileK include/RTL/Host include/RTL/Kernel/OpenCL include/RTL/Kernel/CUDA

# Library and Binaries

SUBDIRS+=lib src

# KLT Runtime Tests

SUBDIRS+=tests/rtl/host tests/rtl/threads
if ROSE_HAVE_OPENCL_INC
  SUBDIRS+=tests/rtl/opencl
endif

# TileK Compile Tests

SUBDIRS+=tests/tilek/host tests/tilek/threads
if ROSE_HAVE_OPENCL_INC
  SUBDIRS+=tests/tilek/opencl
endif

# Documentation

SUBDIRS+=doc

