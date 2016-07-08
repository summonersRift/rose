
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

###################################################################################################

check-local: check-test_0 check-test_1 check-test_2

clean-local: clean-test_0 clean-test_1 clean-test_2
	rm  -f *.dot *.svg *.json klt-dump-static.o 

###################################################################################################

KLT_REL_PATH=src/midend/KLT
TILEK_REL_PATH=../../..

TILEK=$(builddir)/$(TILEK_REL_PATH)/src/tilek

###################################################################################################

KLT_INC=$(top_srcdir)/$(KLT_REL_PATH)/include

KLT_RTL=$(top_builddir)/$(KLT_REL_PATH)/lib/rtl/libKLT-RTL-Host.la

tilek_rc=$(builddir)/../../../tilek.rc

###################################################################################################

ROSE_FLAGS= # -DSKIP_ROSE_BUILTIN_DECLARATIONS
TILEK_FLAGS=--tilek-target=threads -I$(KLT_INC) -I$(srcdir)/../..

C_FLAGS=-O0 -g -I$(KLT_INC) -I$(srcdir)/../..
LD_FLAGS=
EXTRA_LIBS= $(KLT_RTL) -lrt

###################################################################################################

test_0-host-kernel.c: rose_test_0.c
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
	libtool --mode=link gcc $(LD_FLAGS) rose_test_0.o test_0-host-kernel.o test_0-static.o $(EXTRA_LIBS) -o test_0

check-test_0: test_0
	source $(tilek_rc) ; ./test_0

##################################

dump_test_0: klt-dump-static.o test_0-host-kernel.o test_0-static.o $(KLT_RTL)
	libtool --mode=link gcc klt-dump-static.o test_0-host-kernel.o test_0-static.o $(LD_FLAGS) -o dump_test_0

check-dump_test_0: dump_test_0
	./dump_test_0

##################################

clean-test_0:
	rm -f test_0-host-kernel.c rose_test_0.c test_0-static.c
	rm -f test_0-host-kernel.o rose_test_0.o test_0-static.o
	rm -f test_0 dump_test_0

###################################################################################################

test_1-host-kernel.c: rose_test_1.c
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
	libtool --mode=link gcc $(LD_FLAGS) rose_test_1.o test_1-host-kernel.o test_1-static.o $(EXTRA_LIBS) -o test_1

check-test_1: test_1
	source $(tilek_rc) ; ./test_1

##################################

dump_test_1: klt-dump-static.o test_1-host-kernel.o test_1-static.o $(KLT_RTL)
	libtool --mode=link gcc $(LD_FLAGS) klt-dump-static.o test_1-host-kernel.o test_1-static.o $(EXTRA_LIBS) -o dump_test_1

check-dump_test_1: dump_test_1
	source $(tilek_rc) ; ./dump_test_1

##################################

clean-test_1:
	rm -f test_1-host-kernel.c rose_test_1.c test_1-static.c
	rm -f test_1-host-kernel.o rose_test_1.o test_1-static.o
	rm -f test_1 dump_test_1

###################################################################################################

test_2-host-kernel.c: rose_test_2.c
test_2-static.c: rose_test_2.c
rose_test_2.c: $(srcdir)/test_2.c $(TILEK)
	$(TILEK) $(TILEK_FLAGS) $(ROSE_FLAGS) -c $(srcdir)/test_2.c

rose_test_2.o: rose_test_2.c
	gcc $(C_FLAGS) -c rose_test_2.c -o rose_test_2.o

test_2-host-kernel.o: test_2-host-kernel.c
	gcc $(C_FLAGS) -c test_2-host-kernel.c -o test_2-host-kernel.o

test_2-static.o: test_2-static.c
	gcc $(C_FLAGS) -c test_2-static.c -o test_2-static.o

test_2: rose_test_2.o test_2-host-kernel.o test_2-static.o $(KLT_RTL)
	libtool --mode=link gcc $(LD_FLAGS) rose_test_2.o test_2-host-kernel.o test_2-static.o $(EXTRA_LIBS) -o test_2

check-test_2: test_2
	source $(tilek_rc) ; ./test_2

##################################

dump_test_2: klt-dump-static.o test_2-host-kernel.o test_2-static.o $(KLT_RTL)
	libtool --mode=link gcc $(LD_FLAGS) klt-dump-static.o test_2-host-kernel.o test_2-static.o $(EXTRA_LIBS) -o dump_test_2

check-dump_test_2: dump_test_2
	./dump_test_2

##################################

clean-test_2:
	rm -f test_2-host-kernel.c rose_test_2.c test_2-static.c
	rm -f test_2-host-kernel.o rose_test_2.o test_2-static.o
	rm -f test_2 dump_test_2

###################################################################################################

klt-dump-static.o: $(top_srcdir)/$(KLT_REL_PATH)/tools/klt-dump-static.c
	gcc $(C_FLAGS) -c $(top_srcdir)/$(KLT_REL_PATH)/tools/klt-dump-static.c -o klt-dump-static.o

$(top_builddir)/$(KLT_REL_PATH)/lib/rtl/libKLT-RTL-Host.la:
	make -C $(top_builddir)/$(KLT_REL_PATH)/lib/rtl libKLT-RTL-Host.la

###################################################################################################

