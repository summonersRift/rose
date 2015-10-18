
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

#########################################

DLX_REL_PATH=src/frontend/DLX
MFB_REL_PATH=src/midend/MFB
MDCG_REL_PATH=src/midend/MDCG
KLT_REL_PATH=src/midend/KLT
TILEK_REL_PATH=projects/TileK

#########################################

KLT_INC_PATHS= \
  -I$(top_srcdir)/$(DLX_REL_PATH)/include  \
  -I$(top_srcdir)/$(MFB_REL_PATH)/include  \
  -I$(top_srcdir)/$(MDCG_REL_PATH)/include \
  -I$(top_srcdir)/$(KLT_REL_PATH)/include

TILEK_INC_PATHS= \
  -I$(top_srcdir)/$(TILEK_REL_PATH)/include
KLT_LIB_PATHS= \
  -L$(top_builddir)/$(DLX_REL_PATH)/lib/core \
  -L$(top_builddir)/$(MFB_REL_PATH)/lib/sage   -L$(top_builddir)/$(MFB_REL_PATH)/lib/klt -L$(top_builddir)/$(MFB_REL_PATH)/lib/utils \
  -L$(top_builddir)/$(MDCG_REL_PATH)/lib/model -L$(top_builddir)/$(MDCG_REL_PATH)/lib/tools \
  -L$(top_builddir)/$(KLT_REL_PATH)/lib/core   -L$(top_builddir)/$(KLT_REL_PATH)/lib/mdcg

TILEK_LIB_PATHS= \
  -L$(top_builddir)/$(TILEK_REL_PATH)/lib

LIB_PATHS=$(KLT_LIB_PATHS) $(TILEK_LIB_PATHS) $(LIBS_WITH_RPATH)

#########################################
  
INC_PATHS= $(KLT_INC_PATHS) $(TILEK_INC_PATHS) $(ROSE_INCLUDES)

KLT_LIBS=-lDLX-Core -lMFB-Sage -lMFB-KLT -lMFB-Utils -lMDCG-Model -lMDCG-Tools -lKLT-Core -lKLT-MDCG

cxxflags=-g -DVERBOSE=1 -O0

TILEK_cxxflags=$(cxxflags) $(INC_PATHS) -DKLT_PATH=\"$(abs_top_srcdir)/$(KLT_REL_PATH)\"
TILEK_ldadd=$(LIB_PATHS) $(KLT_LIBS) $(ROSE_LIBS)

#########################################

bin_PROGRAMS=tilek

tilek_SOURCES=tilek.cpp
tilek_CXXFLAGS=$(TILEK_cxxflags)
tilek_LDADD=$(TILEK_ldadd) -lDLX-TileK

#########################################

