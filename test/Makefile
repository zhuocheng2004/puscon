
LOG_DIR		:= logs/

LIB_TESTS	:= idmap
LIB_TESTS	:= $(addsuffix .lib_test, ${LIB_TESTS})
.PHONY: LIB_TESTS

RUN_TESTS	:= load_static
RUN_TESTS	:= $(addsuffix .run_test, ${RUN_TESTS})

.PHONY: all
all: ${LIB_TESTS} ${RUN_TESTS}

test_load_static.elf: test_load_static.o
	${LD} -static -o $@ $<

%.lib_test: test_%.elf_host
	@mkdir -p $(LOG_DIR)
	@echo ">>>> $@"
	-@if ./$< > $(LOG_DIR)/$<.txt 2>&1; then echo "P" > .tmp_pf; else echo "F" > .tmp_pf; fi;
	@if grep F .tmp_pf > /dev/null; then echo "\033[31mFAIL:"; else echo "\033[32mSUCCESS"; fi;
	@if grep F .tmp_pf > /dev/null; then cat $(LOG_DIR)/$<.txt; fi;
	@echo "\033[0m"

%.run_test: test_%.elf ../src/main/puscon ../src/kernel/kernel
	@mkdir -p $(LOG_DIR)
	@echo ">>>> $@"
	-@if ../src/main/puscon ../src/kernel/kernel $^ > $(LOG_DIR)/$<.txt 2>&1; then echo "P" > .tmp_pf; else echo "F" > .tmp_pf; fi;
	@if grep F .tmp_pf > /dev/null; then echo "\033[31mFAIL:"; else echo "\033[32mSUCCESS"; fi;
	@if grep F .tmp_pf > /dev/null; then cat $(LOG_DIR)/$<.txt; fi;
	@echo "\033[0m"

%.elf_host: %.o ../src/libpuscon.a ../src/util/libpuscon-util.a
	${CC} -o $@ $^

%.elf: %.o
	${CC} -o $@ $^

%.o: %.c
	${CC} ${CPPFLAGS} -o $@ -c $^

%.o: %.s
	${CC} ${CPPFLAGS} -o $@ -c $^

.PHONY: clean
clean:
	rm -rf $(LOG_DIR)
	rm -rf .tmp*
	rm -rf *.exe *.o
