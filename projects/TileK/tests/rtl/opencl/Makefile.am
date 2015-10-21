
###################################################################################################
if ROSE_HAVE_OPENCL_LIB
TARGETS=check-test_0
else
TARGETS=test_0.o test_0-host-kernel.o test_0-static.o
endif
check: $(TARGETS)

clean: clean-test_0

###################################################################################################

KLT_REL_PATH=src/midend/KLT
KLT_INC=$(top_srcdir)/$(KLT_REL_PATH)/include
KLT_RTL=$(top_builddir)/$(KLT_REL_PATH)/lib/rtl/libKLT-RTL-OpenCL.la

tilek_rc=$(builddir)/../../../tilek.rc

###################################################################################################

C_FLAGS=-O0 -g -I@OPENCL_INC_DIR@ -I$(KLT_INC) -I$(srcdir)/../..
if ROSE_HAVE_OPENCL_LIB
LD_FLAGS=-I@OPENCL_LIB_DIR@
endif
EXTRA_LIBS= $(KLT_RTL) -lrt -lOpenCL

###################################################################################################

test_0.o: $(srcdir)/test_0.c
	gcc $(C_FLAGS) -c $< -o $@

test_0-host-kernel.o: $(srcdir)/test_0-host-kernel.c
	gcc $(C_FLAGS) -c $< -o $@

test_0-static.o: $(srcdir)/test_0-static.c
	gcc $(C_FLAGS) -c $< -o $@

test_0: test_0.o test_0-host-kernel.o test_0-static.o $(KLT_RTL)
	libtool --mode=link gcc $(LD_FLAGS) test_0.o test_0-host-kernel.o test_0-static.o $(EXTRA_LIBS) -o test_0

check-test_0: test_0
	source $(tilek_rc) ; ./test_0

clean-test_0:
	rm -f test_0.o test_0-host-kernel.o test_0-static.o test_0

###################################################################################################

$(top_builddir)/$(KLT_REL_PATH)/lib/rtl/libKLT-RTL-OpenCL.la:
	make -C $(top_builddir)/$(KLT_REL_PATH)/lib/rtl libKLT-RTL-OpenCL.la

###################################################################################################

