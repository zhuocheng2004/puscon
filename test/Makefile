
LOG_DIR		:= logs/

LIB_TESTS	:= idmap
LIB_TESTS	:= $(addsuffix .lib_test, ${LIB_TESTS})
.PHONY: LIB_TESTS

.PHONY: all
all: ${LIB_TESTS}

%.lib_test: %.exe
	@mkdir -p $(LOG_DIR)
	@echo ">>>> $@"
	-@if ./$< > $(LOG_DIR)/$<.txt 2>&1; then echo "P" > .tmp_pf; else echo "F" > .tmp_pf; fi;
	@if grep F .tmp_pf > /dev/null; then echo "\033[31mFAIL:"; else echo "\033[32mSUCCESS"; fi;
	@if grep F .tmp_pf > /dev/null; then cat $(LOG_DIR)/$<.txt; fi;
	@echo "\033[0m"

%.exe: %.o ../src/libpuscon.a ../src/util/libpuscon-util.a
	${CC} -o $@ $^

%.o: %_test.c
	${CC} ${CPPFLAGS} -o $@ -c $^

.PHONY: clean
clean:
	rm -rf $(LOG_DIR)
	rm -rf .tmp*
	rm -rf *.exe *.o
