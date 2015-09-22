
###################################################################################################

check: check-test_0

clean: clean-test_0

###################################################################################################

KLT_REL_PATH=src/midend/KLT
KLT_INC=$(top_srcdir)/$(KLT_REL_PATH)/include
KLT_RTL=$(top_builddir)/$(KLT_REL_PATH)/lib/rtl/libKLT-RTL-Threads.la

###################################################################################################

C_FLAGS=-O0 -g -I$(KLT_INC)
LD_FLAGS=-lrt $(KLT_RTL)

###################################################################################################

test_%.o: $(srcdir)/test_%.c
	gcc $(C_FLAGS) -c $< -o $@

kernel_%.o: $(srcdir)/kernel_%.c
	gcc $(C_FLAGS) -c $< -o $@

static_%.o: $(srcdir)/static_%.c
	gcc $(C_FLAGS) -c $< -o $@

###################################################################################################

test_0: test_0.o kernel_0.o static_0.o $(KLT_RTL)
	libtool --mode=link gcc test_0.o kernel_0.o static_0.o $(LD_FLAGS) -o test_0

check-test_0: test_0
	./test_0

clean-test_0:
	rm -f test_0.o kernel_0.o static_0.o test_0

###################################################################################################

$(top_builddir)/$(KLT_REL_PATH)/lib/rtl/libKLT-RTL-Threads.la:
	make -C $(top_builddir)/$(KLT_REL_PATH)/lib/rtl libKLT-RTL-Threads.la

###################################################################################################

