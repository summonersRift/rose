
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

lib_LTLIBRARIES=libKLT-Core.la

cxxflags=-g -DVERBOSE=1 -O0

libKLT_Core_la_SOURCES= \
  generator.cpp \
  looptree.cpp \
  kernel.cpp \
  descriptor.cpp \
  api.cpp \
  utils.cpp
libKLT_Core_la_CXXFLAGS= \
  $(cxxflags) \
  -I$(top_srcdir)/src/midend/MFB/include \
  -I$(top_srcdir)/src/midend/MDCG/include \
  -I$(top_srcdir)/src/midend/KLT/include \
  $(ROSE_INCLUDES)

