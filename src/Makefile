
SO_3	:= libpuscon.so.${PUSCON_VERSION_MAJOR}.${PUSCON_VERSION_MINOR}.${PUSCON_VERSION_PATCH}
SO_1	:= libpuscon.so.${PUSCON_VERSION_MAJOR}

.PHONY: all
all: libpuscon.a ${SO_3} ${SO_1}

include ${SRC_ROOT}/main_rules.mk

OBJS	:= puscon.o syscall.o printk.o
SUBDIRS	:= lib util

libpuscon.a: ${OBJS} $(call targets_subdir,${SUBDIRS})
	${AR} rcs $@ $^

${SO_3}: ${OBJS} $(call targets_subdir,${SUBDIRS})
	${CC} -shared -o $@ $^

${SO_1}: ${SO_3}
	@rm -f $@
	ln -s $^ $@

.PHONY: clean
clean: $(call targets_subdir_clean,${SUBDIRS})
	@rm -rf *.so* *.a *.o
