
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

lib_LTLIBRARIES=libDLX-Core.la

cxxflags=-g -DVERBOSE=1 -O0

libDLX_Core_la_SOURCES= \
  parser.cpp
libDLX_Core_la_CXXFLAGS= \
  $(cxxflags) \
  -I$(top_srcdir)/src/frontend/DLX/include \
  $(ROSE_INCLUDES)


