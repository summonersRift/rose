
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

###################################################################################################

check-local: check-test_0 check-test_1

clean-local: clean-test_0 clean-test_1
	rm  -f *.dot

###################################################################################################

TILEK_REL_PATH=../../..

TILEK=$(builddir)/$(TILEK_REL_PATH)/src/tilek

###################################################################################################

KLT_INC=$(top_srcdir)/src/midend/KLT/include

KLT_RTL=$(top_builddir)/src/midend/KLT/lib/rtl/libKLT-RTL-OpenCL.la

###################################################################################################

ROSE_FLAGS= # -DSKIP_ROSE_BUILTIN_DECLARATIONS
TILEK_FLAGS=--tilek-target=opencl -I$(KLT_INC)
C_FLAGS=-O0 -g -I$(KLT_INC)
LD_FLAGS=-lrt -lOpenCL $(KLT_RTL)

###################################################################################################

test_0-host-kernel.c: rose_test_0.c
test_0-opencl-kernel.cl: rose_test_0.c
test_0-static.c: rose_test_0.c
rose_test_0.c: $(srcdir)/test_0.c $(TILEK)
	$(TILEK) $(TILEK_FLAGS) $(ROSE_FLAGS) -c $(srcdir)/test_0.c

rose_test_0.o: rose_test_0.c
	gcc $(C_FLAGS) -c rose_test_0.c -o rose_test_0.o

test_0-host-kernel.o: test_0-host-kernel.c
	gcc $(C_FLAGS) -c test_0-host-kernel.c -o test_0-host-kernel.o

test_0-static.o: test_0-static.c
	gcc $(C_FLAGS) -c test_0-static.c -o test_0-static.o

test_0: rose_test_0.o test_0-host-kernel.o test_0-static.o $(KLT_RTL)
	libtool --mode=link gcc rose_test_0.o test_0-host-kernel.o test_0-static.o $(LD_FLAGS) -o test_0

check-test_0: test_0
	./test_0

clean-test_0:
	rm -f test_0-host-kernel.c rose_test_0.c test_0-static.c
	rm -f rose_test_0.o test_0-host-kernel.o test_0-static.o
	rm -f test_0

###################################################################################################

test_1-host-kernel.c: rose_test_1.c
test_1-opencl-kernel.cl: rose_test_1.c
test_1-static.c: rose_test_1.c
rose_test_1.c: $(srcdir)/test_1.c $(TILEK)
	$(TILEK) $(TILEK_FLAGS) $(ROSE_FLAGS) -c $(srcdir)/test_1.c

rose_test_1.o: rose_test_1.c
	gcc $(C_FLAGS) -c rose_test_1.c -o rose_test_1.o

test_1-host-kernel.o: test_1-host-kernel.c
	gcc $(C_FLAGS) -c test_1-host-kernel.c -o test_1-host-kernel.o

test_1-static.o: test_1-static.c
	gcc $(C_FLAGS) -c test_1-static.c -o test_1-static.o

test_1: rose_test_1.o test_1-host-kernel.o test_1-static.o $(KLT_RTL)
	libtool --mode=link gcc rose_test_1.o test_1-host-kernel.o test_1-static.o $(LD_FLAGS) -o test_1

check-test_1: test_1
	./test_1

clean-test_1:
	rm -f test_1-host-kernel.c rose_test_1.c test_1-static.c
	rm -f rose_test_1.o test_1-host-kernel.o test_1-static.o
	rm -f test_1

###################################################################################################

$(top_builddir)/$(KLT_REL_PATH)/lib/rtl/libKLT-RTL-Host.la:
	make -C $(top_builddir)/$(KLT_REL_PATH)/lib/rtl libKLT-RTL-Host.la

###################################################################################################

