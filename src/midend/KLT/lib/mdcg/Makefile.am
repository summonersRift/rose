
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

lib_LTLIBRARIES=libKLT-MDCG.la

cxxflags=-g -DVERBOSE=1 -O0

libKLT_MDCG_la_SOURCES= \
  static-initializer.cpp
libKLT_MDCG_la_CXXFLAGS= \
  $(cxxflags) \
  -I$(top_srcdir)/src/midend/MFB/include \
  -I$(top_srcdir)/src/midend/MDCG/include \
  -I$(top_srcdir)/src/midend/KLT/include \
  $(ROSE_INCLUDES)



