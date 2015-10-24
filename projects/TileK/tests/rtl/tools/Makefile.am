
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

###################################################################################################

TARGET=check-klt-dump-host-devices
if ROSE_HAVE_OPENCL_INC
if ROSE_HAVE_OPENCL_LIB
TARGET+=check-klt-dump-opencl-devices
endif
endif

tilek_rc=$(builddir)/../../../tilek.rc

check-local: $(TARGET)

check-klt-dump-host-devices: klt-dump-host-devices
	source $(tilek_rc) ; ./klt-dump-host-devices

check-klt-dump-opencl-devices: klt-dump-opencl-devices
	source $(tilek_rc) ; ./klt-dump-opencl-devices

clean-local:
	rm -f klt-dump-devices.o klt-dump-host-devices klt-dump-opencl-devices

###################################################################################################

KLT_REL_PATH=src/midend/KLT

###################################################################################################

KLT_INC=$(top_srcdir)/$(KLT_REL_PATH)/include

KLT_HOST_RTL=$(top_builddir)/$(KLT_REL_PATH)/lib/rtl/libKLT-RTL-Host.la
KLT_OPENCL_RTL=$(top_builddir)/$(KLT_REL_PATH)/lib/rtl/libKLT-RTL-OpenCL.la

###################################################################################################

C_FLAGS=-O0 -g -I$(KLT_INC) -I$(srcdir)/../..
if ROSE_HAVE_OPENCL_INC
C_FLAGS+=-L@OPENCL_INC_DIR@
endif
if ROSE_HAVE_OPENCL_LIB
LD_FLAGS=-L@OPENCL_LIB_DIR@
endif
EXTRA_LIBS=-lrt

###################################################################################################

klt-dump-devices.o: $(top_srcdir)/$(KLT_REL_PATH)/tools/klt-dump-devices.c
	gcc $(C_FLAGS) -c $(top_srcdir)/$(KLT_REL_PATH)/tools/klt-dump-devices.c -o klt-dump-devices.o

empty-static.o: $(srcdir)/empty-static.c
	gcc $(C_FLAGS) -c $(srcdir)/empty-static.c -o empty-static.o

klt-dump-host-devices: klt-dump-devices.o empty-static.o $(KLT_HOST_RTL)
	libtool --mode=link gcc  $(LD_FLAGS) klt-dump-devices.o empty-static.o $(KLT_HOST_RTL) $(EXTRA_LIBS) -o klt-dump-host-devices

klt-dump-opencl-devices: klt-dump-devices.o empty-static.o $(KLT_OPENCL_RTL)
	libtool --mode=link gcc $(LD_FLAGS) klt-dump-devices.o empty-static.o $(KLT_OPENCL_RTL) $(EXTRA_LIBS) -lOpenCL -o klt-dump-opencl-devices

###################################################################################################

$(top_builddir)/$(KLT_REL_PATH)/lib/rtl/libKLT-RTL-Host.la:
	make -C $(top_builddir)/$(KLT_REL_PATH)/lib/rtl libKLT-RTL-Host.la

$(top_builddir)/$(KLT_REL_PATH)/lib/rtl/libKLT-RTL-OpenCL.la:
	make -C $(top_builddir)/$(KLT_REL_PATH)/lib/rtl libKLT-RTL-OpenCL.la

###################################################################################################

