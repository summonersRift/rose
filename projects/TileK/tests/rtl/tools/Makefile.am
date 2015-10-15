
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

###################################################################################################

check-local: klt-dump-host-devices klt-dump-opencl-devices
	./klt-dump-host-devices
	./klt-dump-opencl-devices

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
LD_FLAGS=-lrt

###################################################################################################

klt-dump-devices.o: $(top_srcdir)/$(KLT_REL_PATH)/tools/klt-dump-devices.c
	gcc $(C_FLAGS) -c $(top_srcdir)/$(KLT_REL_PATH)/tools/klt-dump-devices.c -o klt-dump-devices.o

empty-static.o: $(srcdir)/empty-static.c
	gcc $(C_FLAGS) -c $(srcdir)/empty-static.c -o empty-static.o

klt-dump-host-devices: klt-dump-devices.o empty-static.o $(KLT_HOST_RTL)
	libtool --mode=link gcc klt-dump-devices.o empty-static.o -lrt $(KLT_HOST_RTL) -o klt-dump-host-devices

klt-dump-opencl-devices: klt-dump-devices.o empty-static.o $(KLT_OPENCL_RTL)
	libtool --mode=link gcc klt-dump-devices.o empty-static.o -lrt -lOpenCL $(KLT_OPENCL_RTL) -o klt-dump-opencl-devices

###################################################################################################

$(top_builddir)/$(KLT_REL_PATH)/lib/rtl/libKLT-RTL-Host.la:
	make -C $(top_builddir)/$(KLT_REL_PATH)/lib/rtl libKLT-RTL-Host.la

$(top_builddir)/$(KLT_REL_PATH)/lib/rtl/libKLT-RTL-OpenCL.la:
	make -C $(top_builddir)/$(KLT_REL_PATH)/lib/rtl libKLT-RTL-OpenCL.la

###################################################################################################

