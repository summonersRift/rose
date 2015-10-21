
###################################################################################################

check: check-test_0

clean: clean-test_0

###################################################################################################

KLT_REL_PATH=src/midend/KLT
KLT_INC=$(top_srcdir)/$(KLT_REL_PATH)/include
KLT_RTL=$(top_builddir)/$(KLT_REL_PATH)/lib/rtl/libKLT-RTL-Host.la

tilek_rc=$(builddir)/../../../tilek.rc

###################################################################################################

C_FLAGS=-O0 -g -I$(KLT_INC) -I$(srcdir)/../..
LD_FLAGS=
EXTRA_LIBS=$(KLT_RTL) -lrt

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
#	KLT_INCPATH="-I/media/ssd/ROSE/repos/tv-tilek-dev/src/midend/KLT/include/" KLT_LIBDIR="/media/ssd/ROSE/build/tv-tilek-dev/src/midend/KLT/lib/" ./test_0
	source $(tilek_rc) ; ./test_0

clean-test_0:
	rm -f test_0.o test_0-host-kernel.o test_0-static.o test_0

###################################################################################################

$(top_builddir)/$(KLT_REL_PATH)/lib/rtl/libKLT-RTL-Host.la:
	make -C $(top_builddir)/$(KLT_REL_PATH)/lib/rtl libKLT-RTL-Host.la

###################################################################################################

