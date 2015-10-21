
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

lib_LTLIBRARIES=libDLX-TileK.la

cxxflags=-g -DVERBOSE=1 -O0

libDLX_TileK_la_SOURCES= \
  dlx-language.cpp \
  dlx-constructs.cpp \
  dlx-clauses.cpp \
  dlx-frontend.cpp
libDLX_TileK_la_CXXFLAGS= \
  $(cxxflags) \
  -I$(top_srcdir)/src/frontend/DLX/include \
  -I$(srcdir)/../include \
  $(ROSE_INCLUDES)

