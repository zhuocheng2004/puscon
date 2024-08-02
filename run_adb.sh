
TARGET_DIR=/data/local/tmp

adb $@ push ./src/libpuscon.so.0 ${TARGET_DIR}
adb $@ push ./src/main/puscon ${TARGET_DIR}
adb $@ push ./src/kernel/kernel ${TARGET_DIR}
adb $@ shell "cd ${TARGET_DIR} && LD_LIBRARY_PATH=\${PWD}:\${LD_LIBRARY_PATH} ./puscon --ansi-color --level 7 ./kernel ."
