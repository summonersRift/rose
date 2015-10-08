
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

lib_LTLIBRARIES=libMDCG-Tools.la

cxxflags=-g -DVERBOSE=1 -O0

libMDCG_Tools_la_SOURCES= \
  model-builder.cpp \
  static-initializer.cpp \
  api.cpp
libMDCG_Tools_la_CXXFLAGS= \
  $(cxxflags) \
  -I$(top_srcdir)/src/midend/MDCG/include \
  -I$(top_srcdir)/src/midend/MFB/include \
  $(ROSE_INCLUDES)


