
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

lib_LTLIBRARIES=libKLT-RTL-Host.la libKLT-RTL-Threads.la libKLT-RTL-OpenCL.la libKLT-RTL.la

KLT_RTL_sources= \
  data-environment.c \
  device.c \
  memory.c \
  kernel.c \
  data.c \
  build-context.c \
  init.c \
  context.c

KLT_RTL_cflags=-g -O0 -I$(top_srcdir)/src/midend/KLT/include

libKLT_RTL_Host_la_SOURCES=$(KLT_RTL_sources)
libKLT_RTL_Host_la_CFLAGS=$(KLT_RTL_cflags)

libKLT_RTL_Threads_la_SOURCES=$(KLT_RTL_sources)
libKLT_RTL_Threads_la_CFLAGS=$(KLT_RTL_cflags) -DKLT_THREADS_ENABLED=1

libKLT_RTL_OpenCL_la_SOURCES=$(KLT_RTL_sources)
libKLT_RTL_OpenCL_la_CFLAGS=$(KLT_RTL_cflags) -DKLT_OPENCL_ENABLED=1

libKLT_RTL_la_SOURCES=$(KLT_RTL_sources)
libKLT_RTL_la_CFLAGS=$(KLT_RTL_cflags) -DKLT_THREADS_ENABLED=1 -DKLT_OPENCL_ENABLED=1




