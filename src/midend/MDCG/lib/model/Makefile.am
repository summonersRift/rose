
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

lib_LTLIBRARIES=libMDCG-Model.la

cxxflags=-g -DVERBOSE=1 -O0

libMDCG_Model_la_SOURCES= \
  model.cpp \
  element.cpp \
  class.cpp \
  field.cpp
libMDCG_Model_la_CXXFLAGS= \
  $(cxxflags) \
  -I$(top_srcdir)/src/midend/MDCG/include \
  -I$(top_srcdir)/src/midend/MFB/include \
  $(ROSE_INCLUDES)


