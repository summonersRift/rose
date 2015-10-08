
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

lib_LTLIBRARIES=libMFB-Utils.la

cxxflags=-g -DVERBOSE=1 -O0

libMFB_Utils_la_SOURCES= \
  utils.cpp
libMFB_Utils_la_CXXFLAGS= \
  $(cxxflags) \
  -I$(top_srcdir)/src/midend/MFB/include \
  $(ROSE_INCLUDES)

