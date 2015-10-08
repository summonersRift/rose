
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

lib_LTLIBRARIES=libMFB-KLT.la

cxxflags=-g -DVERBOSE=1 -O0

libMFB_KLT_la_SOURCES= \
  driver.cpp
libMFB_KLT_la_CXXFLAGS= \
  $(cxxflags) \
  -I$(top_srcdir)/src/midend/MFB/include \
  -I$(top_srcdir)/src/midend/MDCG/include \
  -I$(top_srcdir)/src/midend/KLT/include \
  $(ROSE_INCLUDES)

